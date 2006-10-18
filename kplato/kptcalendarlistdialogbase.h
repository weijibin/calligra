/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#ifndef KPTCALENDARLISTDIALOGBASE_H
#define KPTCALENDARLISTDIALOGBASE_H

#include "ui_kptcalendarlistpanel.h"

#include <QVariant>
#include <QWidget>
#include <QLabel>


namespace KPlato
{

class CalendarEdit;

class CalendarListDialogBase : public QWidget, public Ui_CalendarListPanel
{
    Q_OBJECT
public:
    CalendarListDialogBase( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );

    CalendarEdit* calendar;


};

} //KPlato namespace

#endif // CALENDARLISTDIALOGBASE_H
