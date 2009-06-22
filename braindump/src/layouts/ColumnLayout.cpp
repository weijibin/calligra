/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ColumnLayout.h"

#include <QRectF>

#include <kdebug.h>
#include <klocale.h>
#include <Utils.h>
#include <KoShape.h>

ColumnLayout::ColumnLayout() : Layout("columnlayout"), m_isUpdating(false)
{
}

ColumnLayout::~ColumnLayout() {
}

QRectF ColumnLayout::boundingBox() const {
  QRectF rect = Layout::boundingBox();
  rect.adjust(-10,0,10,0);
  return rect;
}

void ColumnLayout::shapesAdded(QList<KoShape*> _shapes) {
  foreach(KoShape* shape, _shapes) {
    m_shapes.push_back(shape);
  }
  updateShapesPosition();
}

void ColumnLayout::shapeAdded(KoShape* _shape) {
  m_shapes.push_back(_shape);
  updateShapesPosition();
}

void ColumnLayout::shapeRemoved(KoShape* _shape) {
  m_shapes.removeAll(_shape);
  updateShapesPosition();
}

bool contains( const QList<KoShape*> list1, const QList<KoShape*> list2) {
  foreach(KoShape* shape, list2) {
    if( list1.contains(shape) ) {
      return true;
    }
  }
  return false;
}

void ColumnLayout::shapesGeometryChanged(const QList<KoShape*>& _shape) {
  Q_ASSERT(contains(m_shapes, _shape));
  updateShapesPosition();
}


bool shapeIsLessThan(KoShape* s1, KoShape* s2)
{
  return s1->absolutePosition().y() < s2->absolutePosition().y();
}

void ColumnLayout::updateShapesPosition() {
  if(m_isUpdating) return;
  m_isUpdating = true;
  // First sort them
  kDebug() << "<moh>";
  foreach(KoShape* _shape, m_shapes) {
    kDebug() << _shape << _shape->absolutePosition(KoFlake::TopLeftCorner).y() << " " << _shape->position().y();
  }
  kDebug() << "</moh>";
  qSort(m_shapes.begin(),m_shapes.end(), shapeIsLessThan);
  // Update position
  qreal y = 0;
  kDebug() << "<Updating>";
  foreach(KoShape* shape, m_shapes) {
    bool dependOnOtherShape = false;
    foreach(KoShape* otherShape, m_shapes) {
      if(otherShape->hasDependee(shape)) {
        kDebug() << shape << " depends on " << otherShape;
        dependOnOtherShape = true;
        break;
      }
    }
    if(not dependOnOtherShape) {
      shape->update();
      QRectF b;
      Utils::containerBoundRec(shape, b);
      QPointF transfo = QPointF(0.0, y) - b.topLeft();
      shape->setAbsolutePosition( transfo + shape->absolutePosition());
      kDebug() << shape << y << shape->absolutePosition().y() << " " << b;
      y += b.height();
      shape->update();
    }
  }
  kDebug() << "</Updating>";
  emit(boundingBoxChanged(boundingBox()));
  m_isUpdating = false;
}

ColumnLayoutFactory::ColumnLayoutFactory() : LayoutFactory("columnlayout", i18n("Column")) {
}

ColumnLayoutFactory::~ColumnLayoutFactory() {
}

Layout* ColumnLayoutFactory::createLayout() const {
  return new ColumnLayout;
}
