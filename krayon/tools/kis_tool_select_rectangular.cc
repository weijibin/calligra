/*
 *  selecttool.cpp - part of Krayon
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
 *                2001 John Califf <jcaliff@compuzone.net>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qpainter.h>
#include <kdebug.h>

#include "kis_doc.h"
#include "kis_view.h"
#include "kis_canvas.h"
#include "kis_cursor.h"
#include "kis_tool_select_rectangular.h"


RectangularSelectTool::RectangularSelectTool( KisDoc* _doc, KisView* _view, 
    KisCanvas* _canvas )
  : KisTool( _doc, _view)
  , m_dragging( false ) 
  , m_view( _view )  
  , m_canvas( _canvas )

{
      m_drawn = false;
      m_dragStart = QPoint(-1,-1);
      m_dragEnd   = QPoint(-1,-1);
      
      m_Cursor = KisCursor::selectCursor();
}

RectangularSelectTool::~RectangularSelectTool()
{
}

void RectangularSelectTool::clearOld()
{
   if (m_pDoc->isEmpty()) return;
        
   if(m_dragStart.x() != -1)
        drawRect( m_dragStart, m_dragEnd ); 

    QRect updateRect(0, 0, m_pDoc->current()->width(), 
        m_pDoc->current()->height());
    m_view->updateCanvas(updateRect);

    m_dragStart = QPoint(-1,-1);
    m_dragEnd =   QPoint(-1,-1);
}

void RectangularSelectTool::mousePress( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( event->button() == LeftButton )
    {
        // erase old rectangle
        if(m_drawn) 
        {
            m_drawn = false;
           
            if(m_dragStart.x() != -1)
                drawRect( m_dragStart, m_dragEnd ); 
        }
                
        m_dragging = true;
        m_dragStart = event->pos();
        m_dragEnd = event->pos();
    }
}


void RectangularSelectTool::mouseMove( QMouseEvent* event )
{
    if ( m_pDoc->isEmpty() )
        return;

    if( m_dragging )
    {
        drawRect( m_dragStart, m_dragEnd );
        m_dragEnd = event->pos();
        drawRect( m_dragStart, m_dragEnd );
    }
}


void RectangularSelectTool::mouseRelease( QMouseEvent* event )
{
    if (m_pDoc->isEmpty()) return;
    
    if( ( m_dragging ) && ( event->button() == LeftButton ) )
    {
        m_dragging = false;
        m_drawn = true;
        
        QPoint zStart = zoomed(m_dragStart);
        QPoint zEnd   = zoomed(m_dragEnd);
                
        /* jwc - leave selection rectange boundary on screen
        it is only drawn to canvas, not to retained imagePixmap,
        and therefore will disappear when another tool action is used */
        
        /* get selection rectangle after mouse is released
        there always is one, even if width and height are 0 
        left and right, top and bottom are sometimes reversed! 
        I think there is a Qt method we can use to do this, though */
        
        if(zStart.x() <= zEnd.x())
        {
            m_selectRect.setLeft(zStart.x());
            m_selectRect.setRight(zEnd.x());
        }    
        else 
        {
            m_selectRect.setLeft(zEnd.x());                   
            m_selectRect.setRight(zStart.x());
        }
        
        if(zStart.y() <= zEnd.y())
        {
            m_selectRect.setTop(zStart.y());
            m_selectRect.setBottom(zEnd.y());            
        }    
        else
        {
            m_selectRect.setTop(zEnd.y());
            m_selectRect.setBottom(zStart.y());            
        }
                    
        KisImage *img = m_pDoc->current();
        if(!img) return;
        
        KisLayer *lay = img->getCurrentLayer();
        if(!lay) return;
        
        // if there are several partially overlapping or interior
        // layers we must be sure to draw only on the current one
        if (m_selectRect.intersects(lay->layerExtents()))
        {
            m_selectRect = m_selectRect.intersect(lay->layerExtents());

            // the selection class handles getting the selection
            // content from the given rectangular area
            m_pDoc->getSelection()->setRectangularSelection(m_selectRect, lay);

            kdDebug(0) << "selectRect" 
            << " left: "   << m_selectRect.left() 
            << " top: "    << m_selectRect.top()
            << " right: "  << m_selectRect.right() 
            << " bottom: " << m_selectRect.bottom() << endl;
        }    
    }
}


void RectangularSelectTool::drawRect( const QPoint& start, const QPoint& end )
{
    QPainter p, pCanvas;

    p.begin( m_canvas );
    p.setRasterOp( Qt::NotROP );
    p.setPen( QPen( Qt::DotLine ) );

    float zF = m_view->zoomFactor();
    
    /* adjust for scroll ofset as this draws on the canvas, not on
    the image itself QRect(left, top, width, height) */
    
    p.drawRect( QRect(start.x() + m_view->xPaintOffset() 
                                - (int)(zF * m_view->xScrollOffset()),
                      start.y() + m_view->yPaintOffset() 
                                - (int)(zF * m_view->yScrollOffset()), 
                      end.x() - start.x(), 
                      end.y() - start.y()) );
    p.end();
}
