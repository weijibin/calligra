/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/index.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>

#include <assert.h>

#include <kdebug.h>

using namespace KexiDB;

Index::Index()
	: FieldList()
	, m_conn(0)
	, m_primary(0)
{
	//fields are not owned by Index object!
	m_fields.setAutoDelete( false );
}

Index::~Index()
{
}

const QString& Index::name() const
{
	return m_name;
}

void Index::setName(const QString& name)
{
	m_name=name;
}

bool isPrimary();

KexiDB::Table* table();

/*! Retrieves table schema via connection. */
Index::Index(const QString & name, Connection *conn)
	: m_name( name )
	, m_conn( conn )
	, m_primary( false )
	, m_isAutoGenerated( false )
{
	assert(conn);
}

bool Index::isPrimary()
{
	return m_primary;
}

bool Index::isAutoGenerated()
{
	return m_isAutoGenerated;
}

void Index::setAutoGenerated(bool set)
{
	m_isAutoGenerated = set;
}

void Index::setPrimary(bool set)
{
	m_primary = set;
}


/*void Table::debug()
{
	QString dbg = "TABLE " + m_name + "\n";
	for (int i = 0; i < (int)m_fields.count(); i++) {
		if (i>0)
			dbg += ",\n";
		dbg += "  ";
		Field f = m_fields[i];
		dbg += f.m_name + " ";
		if (f.m_options & Field::Unsigned)
			dbg += " UNSIGNED ";
		dbg += m_conn ? m_conn->driver()->sqlTypeName(f.m_type) : Driver::defaultSQLTypeName(f.m_type);
		if (f.m_length > 0)
			dbg += "(" + QString::number(f.m_length) + ")";
		if (f.m_precision > 0)
			dbg += " PRECISION(" + QString::number(f.m_precision) + ")";
		if (f.m_constraints & Field::AutoInc)
			dbg += " AUTOINC";
		if (f.m_constraints & Field::Unique)
			dbg += " UNIQUE";
		if (f.m_constraints & Field::PrimaryKey)
			dbg += " PKEY";
		if (f.m_constraints & Field::ForeignKey)
			dbg += " FKEY";
		if (f.m_constraints & Field::NotNull)
			dbg += " NOTNULL";
	}
	kdDebug() << dbg << endl;

}*/
