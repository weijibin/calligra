/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kpgroupobject.h>

#include <kpresenter_doc.h>
#include <kplineobject.h>
#include <kprectobject.h>
#include <kpellipseobject.h>
#include <kpautoformobject.h>
#include <kpclipartobject.h>
#include <kptextobject.h>
#include <kppixmapobject.h>
#include <kppieobject.h>
#include <kpfreehandobject.h>
#include <kppolylineobject.h>
#include <kpquadricbeziercurveobject.h>
#include <kpcubicbeziercurveobject.h>
#include <kppolygonobject.h>

#include <kdebug.h>

#include <qpainter.h>
using namespace std;

/******************************************************************/
/* Class: KPGroupObject                                           */
/******************************************************************/

/*================================================================*/
KPGroupObject::KPGroupObject()
    : KPObject(), objects( QPtrList<KPObject>() ), updateObjs( false )
{
    objects.setAutoDelete( false );
}

/*================================================================*/
KPGroupObject::KPGroupObject( const QPtrList<KPObject> &objs )
    : KPObject(), objects( objs ), updateObjs( false )
{
    objects.setAutoDelete( false );
}

/*================================================================*/
KPGroupObject &KPGroupObject::operator=( const KPGroupObject & )
{
    return *this;
}

void KPGroupObject::selectAllObj()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->setSelected(true);
    }
}

void KPGroupObject::deSelectAllObj()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->setSelected(false);
    }

}

/*================================================================*/
void KPGroupObject::setSize( double _width, double _height )
{
    KPObject::setSize( _width, _height );

    double fx = (double)ext.width() / (double)origSizeInGroup.width();
    double fy = (double)ext.height() / (double)origSizeInGroup.height();

    updateSizes( fx, fy );
}

/*================================================================*/
void KPGroupObject::setOrig( const KoPoint &_point )
{
    setOrig( _point.x(), _point.y() );
}

/*================================================================*/
void KPGroupObject::setOrig( double _x, double _y )
{
    double dx = 0;
    double dy = 0;
    if ( !orig.isNull() ) {
        dx = _x - orig.x();
        dy = _y - orig.y();
    }

    KPObject::setOrig( _x, _y );

    if ( dx != 0 && dy != 0 )
        updateCoords( dx, dy );
}

/*================================================================*/
void KPGroupObject::moveBy( const KoPoint &_point )
{
    moveBy( _point.x(), _point.y() );
}

/*================================================================*/
void KPGroupObject::moveBy( double _dx, double _dy )
{
    KPObject::moveBy( _dx, _dy );
    updateCoords( _dx, _dy );
}

/*================================================================*/
void KPGroupObject::resizeBy( const KoSize &_size )
{
    resizeBy( _size.width(), _size.height() );
}

/*================================================================*/
void KPGroupObject::resizeBy( double _dx, double _dy )
{
    KPObject::resizeBy( _dx, _dy );

    double fx = (double)ext.width() / (double)origSizeInGroup.width();
    double fy = (double)ext.height() / (double)origSizeInGroup.height();

    updateSizes( fx, fy );
}

/*================================================================*/
QDomDocumentFragment KPGroupObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KPObject::save(doc, offset);
    QDomElement objs=doc.createElement("OBJECTS");
    fragment.appendChild(objs);
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->getType() == OT_PART )
            continue;
        QDomElement object=doc.createElement("OBJECT");
        object.setAttribute("type", static_cast<int>( it.current()->getType() ));
        object.appendChild(it.current()->save( doc,offset ));
        objs.appendChild(object);
    }
    return fragment;
}

/*================================================================*/
double KPGroupObject::load(const QDomElement &element, KPresenterDoc *doc)
{
    //FIXME
    double offset=KPObject::load(element);
    updateObjs = false;
    QDomElement group=element.namedItem("OBJECTS").toElement();
    if(!group.isNull()) {
        QDomElement current=group.firstChild().toElement();
        while(!current.isNull()) {
            ObjType t = OT_LINE;
            if(current.tagName()=="OBJECT") {
                if(current.hasAttribute("type"))
                    t=static_cast<ObjType>(current.attribute("type").toInt());
                double objOffset;
                switch ( t ) {
                    case OT_LINE: {
                        KPLineObject *kplineobject = new KPLineObject();
                        objOffset = kplineobject->load(current);
                        kplineobject->setOrig(kplineobject->getOrig().x(),objOffset);
                        objects.append( kplineobject );
                    } break;
                    case OT_RECT: {
                        KPRectObject *kprectobject = new KPRectObject();
                        objOffset = kprectobject->load(current);
                        kprectobject->setOrig(kprectobject->getOrig().x(),objOffset);
                        objects.append( kprectobject );
                    } break;
                    case OT_ELLIPSE: {
                        KPEllipseObject *kpellipseobject = new KPEllipseObject();
                        objOffset = kpellipseobject->load(current);
                        kpellipseobject->setOrig(kpellipseobject->getOrig().x(),objOffset);
                        objects.append( kpellipseobject );
                    } break;
                    case OT_PIE: {
                        KPPieObject *kppieobject = new KPPieObject();
                        objOffset = kppieobject->load(current);
                        kppieobject->setOrig(kppieobject->getOrig().x(),objOffset);
                        objects.append( kppieobject );
                    } break;
                    case OT_AUTOFORM: {
                        KPAutoformObject *kpautoformobject = new KPAutoformObject();
                        objOffset = kpautoformobject->load(current);
                        kpautoformobject->setOrig(kpautoformobject->getOrig().x(),objOffset);
                        objects.append( kpautoformobject );
                    } break;
                    case OT_CLIPART: {
                        KPClipartObject *kpclipartobject = new KPClipartObject( doc->getClipartCollection() );
                        objOffset = kpclipartobject->load(current);
                        kpclipartobject->setOrig(kpclipartobject->getOrig().x(),objOffset);
                        objects.append( kpclipartobject );
                    } break;
                    case OT_TEXT: {
                        KPTextObject *kptextobject = new KPTextObject( doc );
                        objOffset = kptextobject->load(current);
                        kptextobject->setOrig(kptextobject->getOrig().x(),objOffset);
                        objects.append( kptextobject );
                    } break;
                    case OT_PICTURE: {
                        KPPixmapObject *kppixmapobject = new KPPixmapObject( doc->getImageCollection() );
                        objOffset = kppixmapobject->load(current);
                        kppixmapobject->setOrig(kppixmapobject->getOrig().x(),objOffset);
                        objects.append( kppixmapobject );
                    } break;
                    case OT_FREEHAND: {
                        KPFreehandObject *kpfreehandobject = new KPFreehandObject();
                        objOffset = kpfreehandobject->load( current );
                        kpfreehandobject->setOrig(kpfreehandobject->getOrig().x(),objOffset);
                        objects.append( kpfreehandobject );
                    } break;
                    case OT_POLYLINE: {
                        KPPolylineObject *kppolylineobject = new KPPolylineObject();
                        objOffset = kppolylineobject->load( current );
                        kppolylineobject->setOrig(kppolylineobject->getOrig().x(),objOffset);
                        objects.append( kppolylineobject );
                    } break;
                    case OT_QUADRICBEZIERCURVE: {
                        KPQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPQuadricBezierCurveObject();
                        objOffset = kpQuadricBezierCurveObject->load( current );
                        kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),objOffset);
                        objects.append( kpQuadricBezierCurveObject );
                    } break;
                    case OT_CUBICBEZIERCURVE: {
                        KPCubicBezierCurveObject *kpCubicBezierCurveObject = new KPCubicBezierCurveObject();
                        objOffset = kpCubicBezierCurveObject->load( current );
                        kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),objOffset);
                        objects.append( kpCubicBezierCurveObject );
                    } break;
                    case OT_POLYGON: {
                        KPPolygonObject *kpPolygonObject = new KPPolygonObject();
                        objOffset = kpPolygonObject->load( current );
                        kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),objOffset);
                        objects.append( kpPolygonObject );
                    } break;
                    case OT_GROUP: {
                        KPGroupObject *kpgroupobject = new KPGroupObject();
                        objOffset = kpgroupobject->load(current, doc);
                        kpgroupobject->setOrig(kpgroupobject->getOrig().x(),objOffset);
                        objects.append( kpgroupobject );
                    } break;
                    default: break;
                }
            }
            current=current.nextSibling().toElement();
        }
    }
    updateObjs = true;
    return offset;
}

/*================================================================*/
void KPGroupObject::draw( QPainter *_painter,KoZoomHandler *_zoomhandler,
			  SelectionMode selectionMode, bool drawContour )
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
	it.current()->draw( _painter, _zoomhandler, selectionMode, drawContour );

    KPObject::draw( _painter, _zoomhandler, selectionMode, drawContour );
}

/*================================================================*/
void KPGroupObject::updateSizes( double fx, double fy )
{
    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        //kdDebug() << "Group X: " << origTopLeftPointInGroup.x() << "   Group Y: " << origTopLeftPointInGroup.y() << endl;
        //kdDebug() << "Object X: " << it.current()->getOrigPointInGroup().x() << "   Object Y: " << it.current()->getOrigPointInGroup().y() << endl;
        double _x = ( it.current()->getOrigPointInGroup().x() - origTopLeftPointInGroup.x() ) * fx;
        double _y = ( it.current()->getOrigPointInGroup().y() - origTopLeftPointInGroup.y() ) * fy;

        //kdDebug() << "X: " << _x << "   Y: " << _y << endl;

        KoRect origObjectRect = KoRect( KoPoint( it.current()->getOrigPointInGroup().x(),
                                                 it.current()->getOrigPointInGroup().y() ),
                                        it.current()->getOrigSizeInGroup() );

        KoPoint bottomRight = origObjectRect.bottomRight();
        double _bottomRightX = ( bottomRight.x() - origTopLeftPointInGroup.x() ) * fx;
        double _bottomRightY = ( bottomRight.y() - origTopLeftPointInGroup.y() ) * fy;

        KoRect objectRect = KoRect( KoPoint( _x, _y ), KoPoint( _bottomRightX, _bottomRightY ) );
        double _w = objectRect.width();
        double _h = objectRect.height();

        _x = orig.x() + _x;
        _y = orig.y() + _y;
        it.current()->setOrig( _x, _y );
        it.current()->setSize( _w, _h );
    }
}

/*================================================================*/
void KPGroupObject::updateCoords( double dx, double dy )
{
    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->moveBy( dx, dy );
}

/*================================================================*/
void KPGroupObject::rotate( float _angle )
{
    KPObject::rotate( _angle );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->rotate( _angle );
}

void KPGroupObject::setShadowParameter( int _distance, ShadowDirection _direction, const QColor &_color )
{
    KPObject::setShadowParameter( _distance, _direction, _color );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowParameter( _distance, _direction, _color );
}


/*================================================================*/
void KPGroupObject::setShadowDistance( int _distance )
{
    KPObject::setShadowDistance( _distance );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowDistance( _distance );
}

/*================================================================*/
void KPGroupObject::setShadowDirection( ShadowDirection _direction )
{
    KPObject::setShadowDirection( _direction );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowDirection( _direction );
}

/*================================================================*/
void KPGroupObject::setShadowColor( const QColor &_color )
{
    KPObject::setShadowColor( _color );
    kdDebug() << "KPGroupObject::setShadowColor"<<updateObjs << endl;
    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowColor( _color );
}

/*================================================================*/
void KPGroupObject::setEffect( Effect _effect )
{
    KPObject::setEffect( _effect );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setEffect( _effect );
}

/*================================================================*/
void KPGroupObject::setEffect2( Effect2 _effect2 )
{
    KPObject::setEffect2( _effect2 );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setEffect2( _effect2 );
}

/*================================================================*/
void KPGroupObject::setPresNum( int _presNum )
{
    KPObject::setPresNum( _presNum );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setPresNum( _presNum );
}

/*================================================================*/
void KPGroupObject::setDisappear( bool b )
{
    KPObject::setDisappear( b );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setDisappear( b );
}

/*================================================================*/
void KPGroupObject::setDisappearNum( int num )
{
    KPObject::setDisappearNum( num );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setDisappearNum( num );
}

/*================================================================*/
void KPGroupObject::setEffect3( Effect3 _effect3)
{
    KPObject::setEffect3( _effect3 );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setEffect3( _effect3 );
}

/*================================================================*/
void KPGroupObject::setAppearTimer( int _appearTimer )
{
    KPObject::setAppearTimer( _appearTimer );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setAppearTimer( _appearTimer );
}

/*================================================================*/
void KPGroupObject::setDisappearTimer( int _disappearTimer )
{
    KPObject::setDisappearTimer( _disappearTimer );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setDisappearTimer( _disappearTimer );
}


/*================================================================*/
void KPGroupObject::setOwnClipping( bool _ownClipping )
{
    KPObject::setOwnClipping( _ownClipping );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setOwnClipping( _ownClipping );
}

/*================================================================*/
void KPGroupObject::setSubPresStep( int _subPresStep )
{
    KPObject::setSubPresStep( _subPresStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setSubPresStep( _subPresStep );
}

/*================================================================*/
void KPGroupObject::doSpecificEffects( bool _specEffects, bool _onlyCurrStep )
{
    KPObject::doSpecificEffects( _specEffects, _onlyCurrStep );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->doSpecificEffects( _specEffects, _onlyCurrStep );
}

/*================================================================*/
void KPGroupObject::setAppearSoundEffect( bool b )
{
    KPObject::setAppearSoundEffect( b );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setAppearSoundEffect( b );
}

/*================================================================*/
void KPGroupObject::setDisappearSoundEffect( bool b )
{
    KPObject::setDisappearSoundEffect( b );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->setDisappearSoundEffect( b );
}

/*================================================================*/
void KPGroupObject::setAppearSoundEffectFileName( const QString &_a_fileName )
{
    KPObject::setAppearSoundEffectFileName( _a_fileName );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->setAppearSoundEffectFileName( _a_fileName );
}

/*================================================================*/
void KPGroupObject::setDisappearSoundEffectFileName( const QString &_d_fileName )
{
    KPObject::setDisappearSoundEffectFileName( _d_fileName );

    if ( !updateObjs )
        return;
    QPtrListIterator<KPObject> it( objects);
    for ( ; it.current() ; ++it )
        it.current()->setDisappearSoundEffectFileName( _d_fileName );
}
