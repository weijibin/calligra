/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net>

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

#ifndef KPTCALENDAREDITOR_H
#define KPTCALENDAREDITOR_H

#include "kplatoui_export.h"

#include "kptviewbase.h"
#include "kptitemmodelbase.h"
#include "kptcalendar.h" 
#include "kptcalendarmodel.h"

#include <QTableView>

class QPoint;
class KUndo2Command;

class KoDocument;

namespace KPlato
{

class View;
class Project;
class Calendar;
class CalendarDay;
class DateTableDataModel;
class KDatePicker;

class KPLATOUI_EXPORT CalendarTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    explicit CalendarTreeView(QWidget *parent);

    CalendarItemModel *model() const { return static_cast<CalendarItemModel*>( TreeViewBase::model() ); }

    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }

    Calendar *currentCalendar() const;
    Calendar *selectedCalendar() const;
    QList<Calendar*> selectedCalendars() const;

Q_SIGNALS:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( const QModelIndex&, const QModelIndex& );
    void selectionChanged( const QModelIndexList& );

    void contextMenuRequested( const QModelIndex&, const QPoint& );
    void focusChanged();

protected Q_SLOTS:
    void headerContextMenuRequested( const QPoint &pos );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );

protected:
    void contextMenuEvent ( QContextMenuEvent * event );
    void focusInEvent ( QFocusEvent * event );
    void focusOutEvent ( QFocusEvent * event );
    
    void dragMoveEvent(QDragMoveEvent *event);
};

class KPLATOUI_EXPORT CalendarDayView : public QTableView
{
    Q_OBJECT
public:
    explicit CalendarDayView(QWidget *parent);

    CalendarDayItemModel *model() const { return m_model; }

    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }

    CalendarDay *selectedDay() const;
    TimeInterval *selectedInterval() const;
    
    QSize sizeHint() const;
    
    void setReadWrite( bool on ) { m_readwrite = on; }
    bool isReadWrite() const { return m_readwrite; }

Q_SIGNALS:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( const QModelIndex&, const QModelIndex& );
    void selectionChanged( const QModelIndexList& );

    void contextMenuRequested( const QModelIndex&, const QPoint& );
    void focusChanged();
    
    void executeCommand( KUndo2Command *cmd );
    
public Q_SLOTS:
    void setCurrentCalendar( Calendar *calendar );
    
protected Q_SLOTS:
    void headerContextMenuRequested( const QPoint &pos );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );

    void slotSetWork();
    void slotSetVacation();
    void slotSetUndefined();

    void slotIntervalEditDialogFinished( int result );

protected:
    void contextMenuEvent ( QContextMenuEvent * event );
    void focusInEvent ( QFocusEvent * event );
    void focusOutEvent ( QFocusEvent * event );
    
private:
    CalendarDayItemModel *m_model;
    
    QAction *actionSetUndefined;
    QAction *actionSetVacation;
    QAction *actionSetWork;

    bool m_readwrite;
};

class KPLATOUI_EXPORT CalendarEditor : public ViewBase
{
    Q_OBJECT
public:
    CalendarEditor(KoPart *part, KoDocument *doc, QWidget *parent);
    
    void setupGui();
    Project *project() const { return m_calendarview->project(); }
    virtual void draw( Project &project );
    virtual void draw();

    virtual void updateReadWrite( bool readwrite );

    virtual Calendar *currentCalendar() const;

    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &/*context*/ ) const;

Q_SIGNALS:
    void addCalendar( Calendar *calendar );
    void deleteCalendar( const QList<Calendar*>& );
    
public Q_SLOTS:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected:
    void updateActionsEnabled( bool on );
    void insertCalendar( Calendar *calendar, Calendar *parent, int pos = -1 );
    
protected Q_SLOTS:
    void slotIntervalEditDialogFinished( int result );
    void slotOptions();

private Q_SLOTS:
    void slotContextMenuCalendar( const QModelIndex& index, const QPoint& pos );
    void slotContextMenuDay( const QModelIndex& index, const QPoint& pos );
    void slotContextMenuDate( QMenu*, const QDate& );
    void slotContextMenuDate( QMenu*, const QList<QDate>& );
    
    void slotCalendarSelectionChanged( const QModelIndexList& );
    void slotCurrentCalendarChanged( const QModelIndex& );
    
    void slotDaySelectionChanged( const QModelIndexList& );
    void slotCurrentDayChanged( const QModelIndex& );
    
    void slotEnableActions();

    void slotAddCalendar();
    void slotAddSubCalendar();
    void slotDeleteCalendar();

    void slotAddDay();
    void slotAddInterval();
    void slotDeleteDaySelection();

    void slotSetWork();
    void slotSetVacation();
    void slotSetUndefined();

private:
    CalendarTreeView *m_calendarview;
    CalendarDayView *m_dayview;
    KDatePicker *m_datePicker;
    DateTableDataModel *m_model;
    
    QAction *actionAddCalendar;
    QAction *actionAddSubCalendar;
    QAction *actionDeleteSelection;

    QAction *actionAddDay;
    QAction *actionAddWorkInterval;
    QAction *actionDeleteDaySelection;

    QAction *actionSetUndefined;
    QAction *actionSetVacation;
    QAction *actionSetWork;
    
    QList<QDate> m_currentMenuDateList;

};


}  //KPlato namespace

#endif
