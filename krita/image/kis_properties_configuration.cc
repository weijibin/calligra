/*
 *  Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kis_properties_configuration.h"

#include <kdebug.h>
#include <qdom.h>
#include <QString>

#include "kis_transaction.h"
#include "kis_undo_adapter.h"
#include "kis_painter.h"
#include "kis_selection.h"
#include "KoID.h"
#include "kis_types.h"

struct KisPropertiesConfiguration::Private {
  QMap<QString, QVariant> properties;
};

KisPropertiesConfiguration::KisPropertiesConfiguration() : d(new Private)
{

}

KisPropertiesConfiguration::KisPropertiesConfiguration(const KisPropertiesConfiguration& rhs)
    : KisSerializableConfiguration( rhs )
    , d(new Private(*rhs.d))
{
}

void KisPropertiesConfiguration::fromXML(const QString & s )
{
    clearProperties();

    QDomDocument doc;
    doc.setContent( s );
    QDomElement e = doc.documentElement();
    fromXML(e);
}

void KisPropertiesConfiguration::fromXML(const QDomElement& e)
{
    QDomNode n = e.firstChild();


    while (!n.isNull()) {
        // We don't nest elements in filter configuration. For now...
        QDomElement e = n.toElement();

        if (!e.isNull()) {
            if (e.tagName() == "param") {
                // XXX Convert the variant pro-actively to the right type?
                d->properties[e.attribute("name")] = QVariant(e.text());
            }
        }
        n = n.nextSibling();
    }
    //dump();
}

void KisPropertiesConfiguration::toXML(QDomDocument& doc, QDomElement& root) const
{
    QMap<QString, QVariant>::Iterator it;
    for ( it = d->properties.begin(); it != d->properties.end(); ++it ) {
        QDomElement e = doc.createElement( "param" );
        e.setAttribute( "name", QString(it.key().toLatin1()) );
        QVariant v = it.value();
        QDomText text = doc.createCDATASection(v.toString() ); // XXX: Unittest this!
        root.appendChild(text);
    }
}

QString KisPropertiesConfiguration::toXML() const
{
    QDomDocument doc = QDomDocument("params");
    QDomElement root = doc.createElement( "params" );
    doc.appendChild( root );
    toXML(doc, root);
    return doc.toString();
}

void KisPropertiesConfiguration::setProperty(const QString & name, const QVariant & value)
{
    if ( d->properties.find( name ) == d->properties.end() ) {
        d->properties.insert( name, value );
    }
    else {
        d->properties[name] = value;
    }
}

bool KisPropertiesConfiguration::getProperty(const QString & name, QVariant & value) const
{
   if ( d->properties.find( name ) == d->properties.end() ) {
       return false;
   }
   else {
       value = d->properties[name];
       return true;
   }
}

QVariant KisPropertiesConfiguration::getProperty(const QString & name) const
{
    if ( d->properties.find( name ) == d->properties.end() ) {
        return QVariant();
    }
    else {
        return d->properties[name];
    }
}


int KisPropertiesConfiguration::getInt(const QString & name, int def) const
{
    QVariant v = getProperty(name);
    if (v.isValid())
        return v.toInt();
    else
        return def;

}

double KisPropertiesConfiguration::getDouble(const QString & name, double def) const
{
    QVariant v = getProperty(name);
    if (v.isValid())
        return v.toDouble();
    else
        return def;
}

bool KisPropertiesConfiguration::getBool(const QString & name, bool def) const
{
    QVariant v = getProperty(name);
    if (v.isValid())
        return v.toBool();
    else
        return def;
}

QString KisPropertiesConfiguration::getString(const QString & name, const QString & def) const
{
    QVariant v = getProperty(name);
    if (v.isValid())
        return v.toString();
    else
        return def;
}

void KisPropertiesConfiguration::dump()
{
    QMap<QString, QVariant>::Iterator it;
    for ( it = d->properties.begin(); it != d->properties.end(); ++it ) {
    }

}

void KisPropertiesConfiguration::clearProperties()
{
    d->properties.clear();
}

QMap<QString, QVariant> KisPropertiesConfiguration::getProperties() const
{
    return d->properties;
}
