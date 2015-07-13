/*
 *  Copyright (c) 2015 Dmitry Kazakov <dimula73@gmail.com>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __KIS_IMAGE_ANIMATION_INTERFACE_H
#define __KIS_IMAGE_ANIMATION_INTERFACE_H

#include <QObject>
#include <QScopedPointer>

#include "kis_types.h"
#include "krita_export.h"

class KisUpdatesFacade;
class KisTimeRange;


class KRITAIMAGE_EXPORT KisImageAnimationInterface : public QObject
{
    Q_OBJECT

public:
    KisImageAnimationInterface(KisImage *image);
    ~KisImageAnimationInterface();

    /**
     * Returns currently active frame of the underlying image. Some strokes
     * can override this value and it will report a different value.
     */
    int currentTime() const;

    /**
     * While any non-current frame is being regenerated by the
     * strategy, the image is kept in a special state, named
     * 'externalFrameActive'. Is this state the following applies:
     *
     * 1) All the animated paint devices switch its state into
     *    frameId() defined by global time.
     *
     * 2) All animation-not-capable devices switch to a temporary
     *    content device, which *is in undefined state*. The stroke
     *    should regenerate the image projection manually.
     */
    bool externalFrameActive() const;

    /**
     * Switches current frame (synchronously) and starts an
     * asynchronous regeneration of the entire image.
     */
    void switchCurrentTimeAsync(int frameId);

    /**
     * Start a backgroud thread that will recalculate some extra frame.
     * The result will be reported using two types of signals:
     *
     * 1) KisImage::sigImageUpdated() will be emitted for every chunk
     *    of updated area.
     *
     * 2) sigFrameReady() will be emitted in the end of the operation.
     *    IMPORTANT: to get the result you must connect to this signal
     *    with Qt::DirectConnection and fetch the result from
     *    frameProjection().  After the signal handler is exited, the
     *    data will no longer be available.
     */
    void requestFrameRegeneration(int frameId, const QRegion &dirtyRegion);

    void notifyNodeChanged(const KisNode *node, const QRect &rect, bool recursive);
    void invalidateFrames(const KisTimeRange &range, const QRect &rect);

    /**
     * The current time range selected by user.
     * @return current time range
     */
    const KisTimeRange &currentRange() const;
    void setRange(const KisTimeRange range);

    float framerate();
    void setFramerate(float fps);
private:
    // interface for:
    friend class KisRegenerateFrameStrokeStrategy;
    void saveAndResetCurrentTime(int frameId, int *savedValue);
    void restoreCurrentTime(int *savedValue);
    void notifyFrameReady();
    KisUpdatesFacade* updatesFacade() const;

    void blockFrameInvalidation(bool value);

Q_SIGNALS:
    void sigFrameReady();
    void sigTimeChanged(int newTime);
    void sigFramesChanged(const KisTimeRange &range, const QRect &rect);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif /* __KIS_IMAGE_ANIMATION_INTERFACE_H */
