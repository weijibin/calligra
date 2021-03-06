/* This file is part of the KDE project
 * Copyright (C) 2009 Dag Andersen <danders@get2net.dk>
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

#ifndef PLANTJPLUGIN_H
#define PLANTJPLUGIN_H

#include "kplatotj_export.h"

#include "kptschedulerplugin.h"

#include <QVariantList>


namespace KPlato
{
    class Project;
    class ScheduleManager;
    class Schedule;
}

using namespace KPlato;

class KPLATOTJ_EXPORT PlanTJPlugin : public SchedulerPlugin
{
    Q_OBJECT

public:
    PlanTJPlugin( QObject * parent,  const QVariantList & );
    ~PlanTJPlugin();

    virtual QString description() const;
    virtual int capabilities() const;

    /// Calculate the project
    virtual void calculate( Project &project, ScheduleManager *sm, bool nothread = false );

    /// Return the scheduling granularity in milliseconds
    ulong currentGranularity() const;

Q_SIGNALS:
    void sigCalculationStarted(Project*, ScheduleManager*);
    void sigCalculationFinished(Project*, ScheduleManager*);

public Q_SLOTS:
    void stopAllCalculations();
    void stopCalculation( SchedulerThread *sch );

protected Q_SLOTS:
    void slotStarted( SchedulerThread *job );
    void slotFinished( SchedulerThread *job );
};


#endif // PLANTJPLUGIN_H
