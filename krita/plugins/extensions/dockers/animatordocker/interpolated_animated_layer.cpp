/*
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "interpolated_animated_layer.h"
#include "animator_manager.h"
#include "animator_manager_factory.h"

InterpolatedAnimatedLayer::InterpolatedAnimatedLayer(const KisGroupLayer& source): FramedAnimatedLayer(source)
{
}

InterpolatedAnimatedLayer::InterpolatedAnimatedLayer(KisImageWSP image, const QString& name, quint8 opacity): FramedAnimatedLayer(image, name, opacity)
{
}


void InterpolatedAnimatedLayer::updateFrame(int num)
{
    if (isKeyFrame(num))
        return;

    AnimatorManager* manager = AnimatorManagerFactory::instance()->getManager(image().data());
    if (!manager->ready())
        return;

    int inxt = getNextKey(num);
    KisCloneLayerSP next = 0;
    if (isKeyFrame(inxt)) {
        FrameLayer *frameLayer = const_cast<FrameLayer*>(getKeyFrame(inxt));
        SimpleFrameLayer *simpleFrameLayer = qobject_cast<SimpleFrameLayer*>(frameLayer);
        next = qobject_cast<KisCloneLayer*>(simpleFrameLayer->getContent().data());
    }

    int ipre = getPreviousKey(num);
    KisNodeSP prev = 0;
    if (isKeyFrame(ipre))
        prev = qobject_cast<SimpleFrameLayer*>(getKeyFrame(ipre))->getContent();

    if (prev && next && next->inherits("KisCloneLayer"))
    {
        // interpolation here!
        double cur = num;
        double pre = ipre;
        double nxt = inxt;
        double p = (cur-pre) / (nxt-pre);

        SimpleFrameLayer* frame = qobject_cast<SimpleFrameLayer*>(frameAt(num));
        if (!frame) {
            manager->createFrame(this, num, "", false);           // this will create frame without content
            frame = qobject_cast<SimpleFrameLayer*>(frameAt(num));
        }
        frame->setContent(interpolate(prev, next, p));
    }
}

QString InterpolatedAnimatedLayer::getNameForFrame(int num, bool iskey) const
{
    QString t = FramedAnimatedLayer::getNameForFrame(num, iskey);
    if (iskey)
        return t;
    else
        return t+"_";
}

int InterpolatedAnimatedLayer::getFrameFromName(const QString& name, bool& iskey) const
{
    int result = FramedAnimatedLayer::getFrameFromName(name, iskey);
    return result;
}

#include "moc_interpolated_animated_layer.cpp"
