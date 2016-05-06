/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "MctAuthor.h"

MctAuthor::MctAuthor(QString userName, QString forename , QString surname )
{
    this->userName = userName;
    this->forename = forename;
    this->surname = surname;
}

MctAuthor::~MctAuthor()
{

}

void MctAuthor::setName(QString name)
{
    this->userName = name;
}

void MctAuthor::setForename(QString name)
{
    this->forename = name;
}

void MctAuthor::setSurname(QString name)
{
    this->surname = name;
}

QString MctAuthor::getName() const
{
    return userName;
}

QString MctAuthor::getForename() const
{
    return forename;
}

QString MctAuthor::getSurname() const
{
    return surname;
}
