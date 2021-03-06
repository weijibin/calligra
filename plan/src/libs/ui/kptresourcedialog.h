/* This file is part of the KDE project
   Copyright (C) 2003 - 2011 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTRESOURCEDIALOG_H
#define KPTRESOURCEDIALOG_H

#include "kplatoui_export.h"

#include "ui_resourcedialogbase.h"
#include "kptresource.h"

#include <KoDialog.h>

#include <QMap>

namespace KPlato
{

class Project;
class Resource;
class Calendar;
class MacroCommand;

class ResourceDialogImpl : public QWidget, public Ui_ResourceDialogBase {
    Q_OBJECT
public:
    explicit ResourceDialogImpl( const Project &m_project, Resource &resource, bool baselined, QWidget *parent );

public Q_SLOTS:
    void slotChanged();
    void slotTypeChanged(int);
    void slotCalculationNeeded(const QString&);
    void slotChooseResource();
    
    void setCurrentIndexes( const QModelIndexList &lst );

Q_SIGNALS:
    void changed();
    void calculate();

protected Q_SLOTS:
    void slotAvailableFromChanged(const QDateTime& dt);
    void slotAvailableUntilChanged(const QDateTime& dt);
    
    void slotUseRequiredChanged( int state );
    void slotTeamChanged( const QModelIndex &index );

private:
    const Project &m_project;
    Resource &m_resource;
    QList<QPersistentModelIndex> m_currentIndexes;
};

class KPLATOUI_EXPORT ResourceDialog : public KoDialog {
    Q_OBJECT
public:
    ResourceDialog(Project &project, Resource *resource, QWidget *parent=0, const char *name=0);

    bool calculationNeeded() {  return m_calculationNeeded; }

    Calendar *calendar() { return m_calendars[dia->calendarList->currentIndex()]; }
    MacroCommand *buildCommand();
    
    static MacroCommand *buildCommand(Resource *original, Resource &resource);
    
protected Q_SLOTS:
    void enableButtonOk();
    void slotCalculationNeeded();
    void slotOk();
    void slotCalendarChanged(int);
    void slotButtonClicked(int button);
    void slotAccountChanged( const QString &name );

    void slotResourceRemoved( const Resource *resource );

private:
    Project &m_project;
    Resource *m_original;
    Resource m_resource;
    ResourceDialogImpl *dia;
    bool m_calculationNeeded;
    
    QMap<int, Calendar*> m_calendars;
    QMap<QString, ResourceGroup*> m_groups;
};

} //KPlato namespace

#endif
