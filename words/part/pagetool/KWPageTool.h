/*
 *  Copyright (C) 2011 Jignesh Kakadiya <jigneshhk1992@gmail.com>
 *  
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#ifndef KWPAGETOOL_H
#define KWPAGETOOL_H

#include <KoToolBase.h>

class KWCanvas;
class KWDocument;

class KWPageTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KWPageTool(KoCanvasBase *canvas);
    virtual ~KWPageTool();

public:
    virtual void paint(QPainter &painter, const KoViewConverter &converter);

public Q_SLOTS:
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);

public: // Events

    virtual void mousePressEvent(KoPointerEvent *event);
    virtual void mouseMoveEvent(KoPointerEvent *event);
    virtual void mouseReleaseEvent(KoPointerEvent *event);
//  virtual void mouseDoubleClickEvent(KoPointerEvent *event);

//  virtual void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
    ///Force the remaining content on the page to next page.
    void insertPageBreak();

private:
    KWCanvas *getCanvas() const;
    KWDocument *getDocument() const;

protected:
    QList<QPointer<QWidget> > createOptionWidgets();

private:
    KWCanvas *m_canvas;
    KWDocument *m_document;
};

#endif

