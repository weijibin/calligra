/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef KARBON_DRAG_H
#define KARBON_DRAG_H

#include <QDrag>
#include <QMimeData>
#include <Q3CString>

#include "vgroup.h"

class KarbonDocument;

#define NumEncodeFmts 1
#define NumDecodeFmts 1

class KarbonDrag : public QDrag
{
	Q_OBJECT
public:
	KarbonDrag( QWidget* dragSource = 0, const char* name = 0 );
	const char* format( int i ) const;
	QByteArray encodedData( const char* mimetype ) const;
	static bool canDecode( const QMimeData * );
	static bool decode( const QMimeData* e, VObjectList& sl, KarbonDocument& vdoc );
	void setObjectList( VObjectList l );

private:
	static Q3CString m_encodeFormats[NumEncodeFmts];
	static Q3CString m_decodeFormats[NumDecodeFmts];
	VObjectList m_objects;
};

#endif

