/* This file is part of the KDE project
   Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoMarker.h"

#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include "KoPathShape.h"
#include "KoPathShapeLoader.h"
#include "KoShapeLoadingContext.h"
#include "KoShapeSavingContext.h"

#include <QString>
#include <QPainterPath>

class KoMarker::Private
{
public:
    Private()
    {
    }

    ~Private()
    {
    }

    QString name;
    QString d;
    QPainterPath path;
    QRectF viewBox;
};

KoMarker::KoMarker()
: d(new Private())
{
}

KoMarker::~KoMarker()
{
    delete d;
}

bool KoMarker::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    // A shape uses draw:marker-end="Arrow" draw:marker-end-width="0.686cm" draw:marker-end-center="true" which marker and how the marker is used

    //<draw:marker draw:name="Arrow" svg:viewBox="0 0 20 30" svg:d="m10 0-10 30h20z"/>
    //<draw:marker draw:name="Arrowheads_20_1" draw:display-name="Arrowheads 1" svg:viewBox="0 0 10 10" svg:d="m0 0h10v10h-10z"/>

    d->d =element.attributeNS(KoXmlNS::svg, "d");
    if (d->d.isEmpty()) {
        return false;
    }

    KoPathShape pathShape;
    KoPathShapeLoader loader(&pathShape);
    loader.parseSvg(d->d, true);

    d->path = pathShape.outline();
    d->viewBox = KoPathShape::loadOdfViewbox(element);

    // TODO should the adding to the context be done here?
    QString displayName(element.attributeNS(KoXmlNS::draw, "display-name"));
    if (displayName.isEmpty()) {
        displayName = element.attributeNS(KoXmlNS::draw, "name");
    }
    return true;
}

void KoMarker::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter &writer(context.xmlWriter());
    writer.startElement("draw:marker");
    writer.addAttribute("svg:d", d->d);
    QString viewBox = QString("0 0 %1 %2").arg(qRound(d->viewBox.width())).arg(qRound(d->viewBox.height()));
    writer.addAttribute("svg:viewBox", viewBox);
    writer.addAttribute("draw:name", ""); // TODO how to save the name via the context?
    writer.addAttribute("draw:display-name", d->name);
    writer.endElement();
}

QString KoMarker::name() const
{
    return d->name;
}

QPainterPath KoMarker::path() const
{
    // TODO add transformation for the position it is shown at
    return d->path;
}
