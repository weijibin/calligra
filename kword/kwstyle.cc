/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kwstyle.h"
#include <qdom.h>
#include <kdebug.h>
#include <klocale.h>

KWStyle::KWStyle( const QString & name )
{
    m_name = name;
    m_followingStyle = this;
    m_format.setColor( Qt::black ); // ### why ? what about other color schemes ?
    // This way, KWTextParag::setParagLayout also sets the style pointer, to this style
    m_paragLayout.style = this;
}

KWStyle::KWStyle( QDomElement & styleElem, KWDocument * doc, const QFont & defaultFont )
    : m_paragLayout( styleElem, doc, false ) // Load the paraglayout from the <STYLE> element
{
    // This way, KWTextParag::setParagLayout also sets the style pointer, to this style
    m_paragLayout.style = this;

    QDomElement nameElem = styleElem.namedItem("NAME").toElement();
    if ( !nameElem.isNull() )
    {
        m_name = nameElem.attribute("value");
        //kdDebug() << "KWStyle::KWStyle " << this << " name=" << m_name << endl;
    } else
        kdWarning() << "No NAME tag in LAYOUT -> no name for this style!" << endl;

    // m_followingStyle is set by KWDocument::loadStyleTemplates after loading all the styles
    m_followingStyle = this;

    QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
    if ( !formatElem.isNull() )
        m_format = KWTextParag::loadFormat( formatElem, 0L, defaultFont );
    else
        kdWarning(32001) << "No FORMAT tag in <STYLE>" << endl; // This leads to problems in applyStyle().
}

void KWStyle::operator=( const KWStyle &rhs )
{
    m_paragLayout = rhs.m_paragLayout;
    m_name = rhs.m_name;
    m_format = rhs.m_format;
    m_followingStyle = rhs.m_followingStyle;
    m_paragLayout.style = this; // must always be "this"
}

void KWStyle::save( QDomElement parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement styleElem = doc.createElement( "STYLE" );
    parentElem.appendChild( styleElem );

    m_paragLayout.save( styleElem );

    if ( m_followingStyle )
    {
        QDomElement element = doc.createElement( "FOLLOWING" );
        styleElem.appendChild( element );
        element.setAttribute( "name", m_followingStyle->name() );
    }

    QDomElement formatElem = KWTextParag::saveFormat( doc, &m_format, 0L, 0, 0 );
    styleElem.appendChild( formatElem );
}

QString KWStyle::translatedName() const
{
    return i18n( "KWord style", name().utf8() );
}
