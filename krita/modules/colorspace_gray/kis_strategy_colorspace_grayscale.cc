/*
 *  Copyright (c) 2002 Patrick Julien  <freak@codepimps.org>
 *  Copyright (c) 2004 Cyrille Berger
 *  Copyright (c) 2004 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <limits.h>
#include <stdlib.h>
#include <lcms.h>

#include <qimage.h>

#include <klocale.h>
#include <kdebug.h>

#include "kis_image.h"
#include "kis_strategy_colorspace_grayscale.h"
#include "tiles/kispixeldata.h"
#include "kis_iterators_pixel.h"

namespace {
	const Q_INT32 MAX_CHANNEL_GRAYSCALE = 1;
	const Q_INT32 MAX_CHANNEL_GRAYSCALEA = 2;
}


KisStrategyColorSpaceGrayscale::KisStrategyColorSpaceGrayscale(bool alpha) :
	KisStrategyColorSpace()
{
//	setProfile(cmsCreateGrayProfile());

	m_channels.push_back(new KisChannelInfo(i18n("gray"), 0, COLOR));
	m_alpha = alpha;
	if (alpha) {
		m_name = "Grayscale/Alpha"; // XXX: This is not
					    // i18n-able because we
					    // use it as an id in
					    // files. Fix this!
		m_channels.push_back(new KisChannelInfo(i18n("alpha"), 1, ALPHA));
	}
	else {
		m_name = "Grayscale";
	}
}


KisStrategyColorSpaceGrayscale::~KisStrategyColorSpaceGrayscale()
{
}

void KisStrategyColorSpaceGrayscale::nativeColor(const KoColor& c, QUANTUM *dst)
{
	dst[PIXEL_GRAY] = upscale((c.R() + c.G() + c.B() )/3);
}

void KisStrategyColorSpaceGrayscale::nativeColor(const KoColor& c, QUANTUM opacity, QUANTUM *dst)
{
	dst[PIXEL_GRAY] = upscale((c.R() + c.G() + c.B() )/3);
	if (m_alpha) dst[PIXEL_GRAY_ALPHA] = opacity;
}

void KisStrategyColorSpaceGrayscale::toKoColor(const QUANTUM *src, KoColor *c)
{
	c -> setRGB(downscale(src[PIXEL_GRAY]), downscale(src[PIXEL_GRAY]), downscale(src[PIXEL_GRAY]));
}

void KisStrategyColorSpaceGrayscale::toKoColor(const QUANTUM *src, KoColor *c, QUANTUM *opacity)
{
	c -> setRGB(downscale(src[PIXEL_GRAY]), downscale(src[PIXEL_GRAY]), downscale(src[PIXEL_GRAY]));
	if (m_alpha)
		*opacity = src[PIXEL_GRAY_ALPHA];
	else
		*opacity = OPACITY_OPAQUE;
}

vKisChannelInfoSP KisStrategyColorSpaceGrayscale::channels() const
{
	return m_channels;
}

bool KisStrategyColorSpaceGrayscale::alpha() const
{
	return m_alpha;
}

Q_INT32 KisStrategyColorSpaceGrayscale::depth() const
{
	if (m_alpha)
		return MAX_CHANNEL_GRAYSCALEA;
	else
		return MAX_CHANNEL_GRAYSCALE;
}

Q_INT32 KisStrategyColorSpaceGrayscale::nColorChannels() const
{
	return MAX_CHANNEL_GRAYSCALE;
}


QImage KisStrategyColorSpaceGrayscale::convertToQImage(const QUANTUM *data, Q_INT32 width, Q_INT32 height, Q_INT32 stride) const 
{
	QImage img(width, height, 32, 0, QImage::LittleEndian);

	Q_INT32 i = 0;
	uchar *j = img.bits();

	while ( i < stride * height ) {
		QUANTUM q = *( data + i + PIXEL_GRAY );

		// XXX: Moved here to get rid of these global constants
		const PIXELTYPE PIXEL_BLUE = 0;
		const PIXELTYPE PIXEL_GREEN = 1;
		const PIXELTYPE PIXEL_RED = 2;
		const PIXELTYPE PIXEL_ALPHA = 3;

		if (m_alpha)
			*( j + PIXEL_ALPHA ) = *( data + i + PIXEL_GRAY_ALPHA );
		else
			*( j + PIXEL_ALPHA ) = OPACITY_OPAQUE;

		*( j + PIXEL_RED )   = q;
		*( j + PIXEL_GREEN ) = q;
		*( j + PIXEL_BLUE )  = q;
		
		if (m_alpha)
			i += MAX_CHANNEL_GRAYSCALEA;
		else
			i += MAX_CHANNEL_GRAYSCALE;

		j += 4; // Because we're hard-coded 32 bits deep, 4 bytes
		
	}

	return img;
}

void KisStrategyColorSpaceGrayscale::bitBlt(Q_INT32 stride,
					    QUANTUM *dst, 
					    Q_INT32 dststride,
					    QUANTUM *src, 
					    Q_INT32 srcstride,
					    QUANTUM opacity,
					    Q_INT32 rows, 
					    Q_INT32 cols, 
					    CompositeOp op)
{
	QUANTUM *d;
	QUANTUM *s;
	Q_INT32 i;
	Q_INT32 linesize;

	if (rows <= 0 || cols <= 0)
		return;
	switch (op) {
	case COMPOSITE_COPY:
		linesize = stride * sizeof(QUANTUM) * cols;
		d = dst;
		s = src;
		while (rows-- > 0) {
			memcpy(d, s, linesize);
			d += dststride;
			s += srcstride;
		}
		return;
	case COMPOSITE_CLEAR:
		linesize = stride * sizeof(QUANTUM) * cols;
		d = dst;
		while (rows-- > 0) {
			memset(d, 0, linesize);
			d += dststride;
		}
		return;
	case COMPOSITE_OVER:
	default:
		if (opacity == OPACITY_TRANSPARENT) 
			return;
		if (opacity != OPACITY_OPAQUE) {
			while (rows-- > 0) {
				d = dst;
				s = src;
				for (i = cols; i > 0; i--, d += stride, s += stride) {
					if (m_alpha) {
						if (s[PIXEL_GRAY_ALPHA] == OPACITY_TRANSPARENT)
							continue;

						int srcAlpha = (s[PIXEL_GRAY_ALPHA] * opacity + QUANTUM_MAX / 2) / QUANTUM_MAX;
						int dstAlpha = (d[PIXEL_GRAY_ALPHA] * (QUANTUM_MAX - srcAlpha) + QUANTUM_MAX / 2) / QUANTUM_MAX;
						d[PIXEL_GRAY] = (d[PIXEL_GRAY]   * dstAlpha + s[PIXEL_GRAY]   * srcAlpha + QUANTUM_MAX / 2) / QUANTUM_MAX;
					
						d[PIXEL_GRAY_ALPHA] = (d[PIXEL_GRAY_ALPHA] * (QUANTUM_MAX - srcAlpha) + srcAlpha * QUANTUM_MAX + QUANTUM_MAX / 2) / QUANTUM_MAX;
						if (d[PIXEL_GRAY_ALPHA] != 0) {
							d[PIXEL_GRAY] = (d[PIXEL_GRAY] * QUANTUM_MAX) / d[PIXEL_GRAY_ALPHA];
						}
					}
					else {
						d[PIXEL_GRAY] = (d[PIXEL_GRAY] * OPACITY_OPAQUE + s[PIXEL_GRAY] * opacity + QUANTUM_MAX / 2) / QUANTUM_MAX;
					}
				}
				dst += dststride;
				src += srcstride;
			}
		}
		else {
			while (rows-- > 0) {
				d = dst;
				s = src;
				for (i = cols; i > 0; i--, d += stride, s += stride) {
					if (m_alpha) {
						if (s[PIXEL_GRAY_ALPHA] == OPACITY_TRANSPARENT)
							continue;
						if (d[PIXEL_GRAY_ALPHA] == OPACITY_TRANSPARENT || s[PIXEL_GRAY_ALPHA] == OPACITY_OPAQUE) {
							memcpy(d, s, stride * sizeof(QUANTUM));
							continue;
						}
						int srcAlpha = s[PIXEL_GRAY_ALPHA];
						int dstAlpha = (d[PIXEL_GRAY_ALPHA] * (QUANTUM_MAX - srcAlpha) + QUANTUM_MAX / 2) / QUANTUM_MAX;
						d[PIXEL_GRAY]   = (d[PIXEL_GRAY]   * dstAlpha + s[PIXEL_GRAY]   * srcAlpha + QUANTUM_MAX / 2) / QUANTUM_MAX;
						d[PIXEL_GRAY_ALPHA] = (d[PIXEL_GRAY_ALPHA] * (QUANTUM_MAX - srcAlpha) + srcAlpha * QUANTUM_MAX + QUANTUM_MAX / 2) / QUANTUM_MAX;
						if (d[PIXEL_GRAY_ALPHA] != 0) {
							d[PIXEL_GRAY] = (d[PIXEL_GRAY] * QUANTUM_MAX) / d[PIXEL_GRAY_ALPHA];
						}
					}
					else {
						d[PIXEL_GRAY]   = s[PIXEL_GRAY];
					}
				}
				dst += dststride;
				src += srcstride;
			}
		}

	}
}

