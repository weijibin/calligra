/*
 *  kfloatingdialog.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
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

#ifndef __kfloatingdialog_h__
#define __kfloatingdialog_h__

#include <qpoint.h>
#include <qframe.h>
#include <qpushbutton.h>

#define TITLE_HEIGHT 18
#define MIN_HEIGHT 18
#define MIN_WIDTH 60
#define GRADIENT_HEIGHT 14
#define FRAMEBORDER 2

class KFloatingDialog : public QFrame
{
  Q_OBJECT
 
 public:
  KFloatingDialog(QWidget *parent = 0L);
  ~KFloatingDialog();

  // usable client space:
  int _left() { return FRAMEBORDER; }
  int _top() { return TITLE_HEIGHT; }
  int _width() { return width() - 2*FRAMEBORDER; }
  int _height() { return height() - TITLE_HEIGHT - FRAMEBORDER; }

 protected:
  virtual void paintEvent(QPaintEvent *);
  virtual void resizeEvent(QResizeEvent *);
  virtual void leaveEvent(QEvent *);
  virtual void mousePressEvent(QMouseEvent *);
  virtual void mouseMoveEvent(QMouseEvent *);
  virtual void mouseReleaseEvent(QMouseEvent *);
  virtual void mouseDoubleClickEvent (QMouseEvent *); 

  const QRect titleRect()  { return QRect(0,0, width(), TITLE_HEIGHT); }
  const QRect bottomRect() { return QRect(FRAMEBORDER, height()-FRAMEBORDER, width()-2*FRAMEBORDER, FRAMEBORDER); }
  const QRect rightRect() { return QRect(width()-FRAMEBORDER, FRAMEBORDER, FRAMEBORDER, height() - 2*FRAMEBORDER); }
  const QRect lowerRightRect() { return QRect(width()-FRAMEBORDER, height()-FRAMEBORDER , FRAMEBORDER, FRAMEBORDER); }
  
 protected:
  enum resizeMode { horizontal, vertical, diagonal };
  bool     m_dragging;
  bool     m_resizing;
  bool     m_shaded;
  bool     m_cursor;

  QPoint   m_start;
  int      m_unshadedHeight;
  int      m_resizeMode;

  QWidget     *m_pParent;
  QPushButton *m_pCloseButton, *m_pDockButton, *m_pMinButton;
};

#endif
