/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXI_MIGRATION_PART_H
#define KEXI_MIGRATION_PART_H

#include <core/kexiinternalpart.h>

/*! Internal part for import wizard. */
class KexiMigrationPart : public KexiInternalPart
{
	public:
		KexiMigrationPart(QObject *parent, const char *name, const QStringList &args);
		virtual ~KexiMigrationPart();

		/*! Reimplement this if your internal part has to return widgets 
		 or QDialog objects. */
		virtual QWidget *createWidget(const char* /*widgetClass*/, KexiMainWindow* mainWin, 
		 QWidget *parent, const char *objName = 0 );
};

#endif
