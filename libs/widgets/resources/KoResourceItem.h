/* This file is part of the KDE project
   Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
   Copyright (c) 2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (c) 2007 Sven Langkamp <sven.langkamp@gmail.com>

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

#ifndef KO_RESOURCE_ITEM
#define KO_RESOURCE_ITEM

#include <QWidget>
#include <QTableWidgetItem>
#include "kowidgets_export.h"

class QButtonGroup;
class KoResourceChooser;
class KoResource;

class KOWIDGETS_EXPORT KoResourceItem : public QTableWidgetItem {

public:
    KoResourceItem(KoResource *resource);
    virtual ~KoResourceItem();

    KoResource *resource() const;

    virtual int compare(const QTableWidgetItem *other) const;

    virtual QVariant data(int role) const;

protected:
    QImage thumbnail( const QSize &thumbSize ) const;

private:
    KoResource *m_resource;
};

#endif // KO_RESOURCE_ITEM
