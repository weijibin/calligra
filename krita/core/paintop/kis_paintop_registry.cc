/*
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

#include "kdebug.h"

#include "kis_generic_registry.h"
#include "kis_types.h"
#include "kis_paintop_registry.h"
#include "kis_paintop.h"

#include "kis_airbrushop.h"
#include "kis_brushop.h"
#include "kis_convolveop.h"
#include "kis_duplicateop.h"
#include "kis_eraseop.h"
#include "kis_filterop.h"
#include "kis_penop.h"

KisPaintOpRegistry * KisPaintOpRegistry::m_singleton = 0;

KisPaintOpRegistry::KisPaintOpRegistry()
{
// 	kdDebug() << " creating a KisPaintOpRegistry" << endl;
	Q_ASSERT(KisPaintOpRegistry::m_singleton == 0);
	KisPaintOpRegistry::m_singleton = this;

	// Add hard-coded paint ops. Plugin paintops will add
	// themselves in the plugin initialization code.

 	add(new KisAirbrushOpFactory);
 	add(new KisBrushOpFactory);
 	add(new KisConvolveOpFactory);
 	add(new KisDuplicateOpFactory);
 	add(new KisEraseOpFactory);
 	add(new KisFilterOpFactory);
 	add(new KisPenOpFactory);

}

KisPaintOpRegistry::~KisPaintOpRegistry()
{
}

KisPaintOpRegistry* KisPaintOpRegistry::instance()
{
	if(KisPaintOpRegistry::m_singleton == 0)
	{
		KisPaintOpRegistry::m_singleton = new KisPaintOpRegistry();
	}
	return KisPaintOpRegistry::m_singleton;
}

KisPaintOp * KisPaintOpRegistry::paintOp(const QString & name, KisPainter * painter) const
{
	KisPaintOpFactorySP f = get(name);
	if (f) {
		return f -> createOp(painter);
	}
	else {
		return 0;
	}
}
