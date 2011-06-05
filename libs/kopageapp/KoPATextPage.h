/* This file is part of the KDE project
   Copyright (C) 2009-2010 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KOPATEXTPAGE_H
#define KOPATEXTPAGE_H

#include <KoTextPage.h>

#include "kopageapp_export.h"

class KoPAPageBase;

class KOPAGEAPP_EXPORT KoPATextPage : public KoTextPage
{
public:
    KoPATextPage(int pageNumber, KoPAPageBase *page);

    virtual ~KoPATextPage();

    virtual int pageNumber() const;
    virtual int visiblePageNumber(PageSelection select = CurrentPage, int adjustment = 0) const;

    KoPAPageBase *page() const;

    virtual QRectF rect() const;

private:
    int m_pageNumber;
    KoPAPageBase * m_page;
};

#endif /* KOPATEXTPAGE_H */
