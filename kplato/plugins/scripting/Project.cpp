/* This file is part of the Calligra project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
 * Copyright (c) 2008 Dag Andersen <kplato@kde.org>
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

#include "Project.h"
#include "Account.h"
#include "Calendar.h"
#include "Resource.h"
#include "ResourceGroup.h"
#include "Schedule.h"

#include "kptglobal.h"
#include "kptaccount.h"
#include "kptcalendar.h"
#include "kptproject.h"
#include "kptschedule.h"
#include "kptresource.h"
#include "kptcommand.h"

#include <QMetaEnum>

Scripting::Project::Project( Scripting::Module* module, KPlato::Project *project )
    : Node( this, project, module ), m_module( module ), m_command( new MacroCommand( QString() ) )
{
    kDebug()<<this<<"KPlato::"<<project;
    m_nodeModel.setProject( project );
    m_resourceModel.setProject( project );
}

Scripting::Project::~Project()
{
    kDebug()<<this;
    delete m_command;
    qDeleteAll( m_nodes );
    qDeleteAll( m_groups );
    qDeleteAll( m_resources );
    qDeleteAll( m_calendars );
    qDeleteAll( m_schedules );
    qDeleteAll( m_accounts );
}

QVariant Scripting::Project::data( QObject *object, const QString &property )
{
    return data( object, property, "DisplayRole", -1 );
}

QVariant Scripting::Project::data( QObject *object, const QString &property, const QString &role, qlonglong scheduleId )
{
    Node *n = qobject_cast<Node*>( object );
    if ( n ) {
        return nodeData( n->kplatoNode(), property, role, scheduleId );
    }
    Resource *r = qobject_cast<Resource*>( object );
    if ( r ) {
        return resourceData( r->kplatoResource(), property, role, scheduleId );
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( object );
    if ( g ) {
        return resourceGroupData( g->kplatoResourceGroup(), property, role );
    }
    Account *a = qobject_cast<Account*>( object );
    if ( a ) {
        return accountData( a->kplatoAccount(), property, role );
    }
    Calendar *c = qobject_cast<Calendar*>( object );
    if ( c ) {
        return calendarData( c->kplatoCalendar(), property, role );
    }
    // TODO Schedule (if needed)
    return QVariant();
}

bool Scripting::Project::setData( QObject *object, const QString &property, const QVariant &data, const QString &role )
{
    Node *n = qobject_cast<Node*>( object );
    if ( n ) {
        return setNodeData( n->kplatoNode(), property, data, role );
    }
    Resource *r = qobject_cast<Resource*>( object );
    if ( r ) {
        return setResourceData( r->kplatoResource(), property, data, role );
    }
    ResourceGroup *g = qobject_cast<ResourceGroup*>( object );
    if ( g ) {
        return setResourceGroupData( g->kplatoResourceGroup(), property, data, role );
    }
    Account *a = qobject_cast<Account*>( object );
    if ( a ) {
        return setAccountData( a->kplatoAccount(), property, data, role );
    }
    Calendar *c = qobject_cast<Calendar*>( object );
    if ( c ) {
        return setCalendarData( c->kplatoCalendar(), property, data, role );
    }
    return false;
}

int Scripting::Project::scheduleCount() const
{
    return project()->numScheduleManagers();
}

QObject *Scripting::Project::scheduleAt( int index )
{
    return schedule( project()->scheduleManagers().value( index ) );
}

QObject *Scripting::Project::schedule( KPlato::ScheduleManager *sch )
{
    if ( ! m_schedules.contains( sch ) ) {
        m_schedules[ sch ] = new Schedule( this, sch, parent() );
    }
    return m_schedules[ sch ];
}


QStringList Scripting::Project::nodePropertyList()
{
    QStringList lst;
    QMetaEnum e = m_nodeModel.columnMap();
    for ( int i = 0; i < e.keyCount(); ++i ) {
        lst << QString( e.key( i ) );
    }
    return lst;
}

QVariant Scripting::Project::nodeHeaderData( const QString &property )
{
    int col = nodeColumnNumber( property );
    return m_nodeModel.headerData( col );
}

int Scripting::Project::nodeColumnNumber( const QString &property ) const
{
    QString prop = property;
    if ( prop.left( 4 ) != "Node" ) {
        prop.prepend( "Node" );
    }
    return m_nodeModel.columnMap().keyToValue( prop.toUtf8() );
}

int Scripting::Project::resourceColumnNumber( const QString &property ) const
{
    QString prop = property;
    if ( prop.left( 8 ) != "Resource" ) {
        prop.prepend( "Resource" );
    }
    return m_resourceModel.columnMap().keyToValue( property.toUtf8() );
}

Scripting::Node *Scripting::Project::node( KPlato::Node *node )
{
    if ( node == 0 ) {
        return 0;
    }
    if ( ! m_nodes.contains( node ) ) {
        m_nodes[ node ] = new Node( this, node, parent() );
    }
    return m_nodes[ node ];
}

int Scripting::Project::nodeCount() const
{
    return project()->nodeCount();
}

QObject *Scripting::Project::nodeAt( int index )
{
    return node( project()->allNodes().value( index ) );
}

QVariant Scripting::Project::nodeData( const KPlato::Node *node, const QString &property, const QString &role, long schedule )
{
    m_nodeModel.setManager( project()->scheduleManager( schedule ) );
    return m_nodeModel.data( node, nodeColumnNumber( property ), stringToRole( role ) ).toString();
}

bool Scripting::Project::setNodeData( KPlato::Node *node, const QString &property, const QVariant &data, const QString &role )
{
    KUndo2Command *cmd = m_nodeModel.setData( node, nodeColumnNumber( property ), data, stringToRole( role ) );
    if ( ! cmd ) {
        kDebug()<<"Failed:"<<node->name()<<property<<data<<role;
        kDebug()<<"Failed:"<<node->name()<<nodeColumnNumber(property)<<data<<stringToRole(role);
        return false;
    }
    cmd->redo();
    m_command->addCommand( cmd );
    return true;
}


QObject *Scripting::Project::findNode( const QString &id )
{
    return node( project()->findNode( id ) );
}

QObject *Scripting::Project::createTaskCopy(const QObject* copy, QObject* parent, QObject* after)
{
    const Node *cpy = static_cast<const Node*>( copy );
    KPlato::Node *t = 0;
    if ( cpy ) {
        t = project()->createTask( static_cast<KPlato::Task&>( *( cpy->kplatoNode() ) ) );
    } else {
        t = project()->createTask();
    }
    KPlato::NamedCommand *cmd;
    if ( parent ) {
        KPlato::Node *par = static_cast<Node*>( parent )->kplatoNode();
        cmd = new SubtaskAddCmd( project(), t, par, i18nc( "(qtundo_format)", "Add task" ) );
    } else {
        KPlato::Node *aft = after ? static_cast<Node*>( after )->kplatoNode() : 0;
        cmd = new TaskAddCmd( project(), t, aft, i18nc( "(qtundo_format)", "Add task" ) );
    }
    slotAddCommand( cmd );
    return node( t );
}

QObject *Scripting::Project::createTask( QObject* parent, QObject* after )
{
    KPlato::Task *t = project()->createTask();
    KPlato::NamedCommand *cmd;
    if ( parent ) {
        KPlato::Node *par = static_cast<Node*>( parent )->kplatoNode();
        cmd = new SubtaskAddCmd( project(), t, par, i18nc( "(qtundo_format)", "Add task" ) );
    } else {
        KPlato::Node *aft = after ? static_cast<Node*>( after )->kplatoNode() : 0;
        cmd = new TaskAddCmd( project(), t, aft, i18nc( "(qtundo_format)", "Add task" ) );
    }
    slotAddCommand( cmd );
    return node( t );
}

void Scripting::Project::addCommand( const QString &name )
{
    if ( m_command->isEmpty() ) {
        return;
    }
    if ( ! name.isEmpty() ) {
        m_command->setText( name );
    }
    m_module->addCommand( m_command );
    m_command = new KPlato::MacroCommand( QString() );
}

void Scripting::Project::revertCommand()
{
    if ( m_command ) {
        m_command->undo();
    }
    delete m_command;
    m_command = new KPlato::MacroCommand( QString() );
}

int Scripting::Project::resourceGroupCount() const
{
    return project()->resourceGroupCount();
}

QObject *Scripting::Project::resourceGroupAt( int index )
{
    return resourceGroup( project()->resourceGroupAt( index ) );
}

QObject *Scripting::Project::findResourceGroup( const QString &id )
{
    KPlato::ResourceGroup *g = project()->findResourceGroup( id );
    return g == 0 ? 0 : resourceGroup( g );
}

QObject *Scripting::Project::createResourceGroup( QObject *group )
{
    //kDebug()<<this<<group;
    const ResourceGroup *gr = qobject_cast<ResourceGroup*>( group );
    if ( gr == 0 ) {
        kDebug()<<"No group specified";
        return 0;
    }
    KPlato::ResourceGroup *copyfrom = gr->kplatoResourceGroup();
    if ( copyfrom == 0 ) {
        kDebug()<<"Nothing to copy from";
        return 0;
    }
    KPlato::ResourceGroup *g = project()->findResourceGroup( copyfrom->id() );
    if ( g ) {
        kDebug()<<"Resource group already exists";
        return 0; // ???
    }
    g = new KPlato::ResourceGroup( copyfrom );
    AddResourceGroupCmd *cmd = new AddResourceGroupCmd( project(), g, i18nc( "(qtundo_format)", "Add resource group" ) );
    cmd->redo();
    m_command->addCommand( cmd );
    return resourceGroup( g );
}

QObject *Scripting::Project::resourceGroup( KPlato::ResourceGroup *group )
{
    if ( ! m_groups.contains( group ) ) {
        m_groups[ group ] = new ResourceGroup( this, group, parent() );
    }
    return m_groups[ group ];
}

bool Scripting::Project::setResourceGroupData( KPlato::ResourceGroup *resource, const QString &property, const QVariant &data, const QString &role )
{
    KPlato::ResourceItemModel m;
    connect(&m, SIGNAL(executeCommand(KUndo2Command*)), SLOT(slotAddCommand(KUndo2Command*)));
    QModelIndex idx = m.index( resource );
    idx = m.index( idx.row(), resourceColumnNumber( property ), idx.parent() );
    if ( ! idx.isValid() ) {
        return false;
    }
    return m.setData( idx, data, stringToRole( role ) );
}

QVariant Scripting::Project::resourceGroupData( const KPlato::ResourceGroup *group, const QString &property, const QString &role, long /*schedule*/ )
{
//    m.setManager( project()->scheduleManager( schedule ) );
    return m_resourceModel.data( group, resourceColumnNumber( property ), stringToRole( role ) );
}

QObject *Scripting::Project::createResource( QObject *group, QObject *res )
{
    ResourceGroup *gr = qobject_cast<ResourceGroup*>( group );
    if ( gr == 0 ) {
        kDebug()<<"No group specified";
        return 0;
    }
    KPlato::ResourceGroup *g = project()->findResourceGroup( gr->kplatoResourceGroup()->id() );
    if ( g == 0 ) {
        kDebug()<<"Could not find group";
        return 0;
    }
    const Resource *rs = qobject_cast<Resource*>( res );
    if ( rs == 0 ) {
        kDebug()<<"No resource to copy from";
        return 0; // or create empty?
    }
    KPlato::Resource *r = project()->findResource( rs->kplatoResource()->id() );
    if ( r ) {
        kDebug()<<"Resource already exists";
        return 0;
    }
    r = new KPlato::Resource( rs->kplatoResource() );
    KPlato::Calendar *c = rs->kplatoResource()->calendar( true );
    if ( c ) {
        c = project()->calendar( c->id() );
    }
    r->setCalendar( c );
    AddResourceCmd *cmd = new AddResourceCmd( g, r, i18nc( "(qtundo_format)", "Add resource" ) );
    cmd->redo();
    m_command->addCommand( cmd );
    return resource( r );
}


QObject *Scripting::Project::resource( KPlato::Resource *resource )
{
    if ( ! m_resources.contains( resource ) ) {
        m_resources[ resource ] = new Resource( this, resource, parent() );
    }
    return m_resources[ resource ];
}

bool Scripting::Project::setResourceData( KPlato::Resource *resource, const QString &property, const QVariant &data, const QString &role )
{
    KPlato::ResourceItemModel m;
    connect(&m, SIGNAL(executeCommand(KUndo2Command*)), SLOT(slotAddCommand(KUndo2Command*)));
    QModelIndex idx = m.index( resource );
    idx = m.index( idx.row(), resourceColumnNumber( property ), idx.parent() );
    if ( ! idx.isValid() ) {
        return false;
    }
    return m.setData( idx, data, stringToRole( role ) );
}

QVariant Scripting::Project::resourceData( const KPlato::Resource *resource, const QString &property, const QString &role, long /*schedule*/ )
{
    //m_resourceModel.setManager( project()->scheduleManager( schedule.toLong() ) );
    return m_resourceModel.data( resource, resourceColumnNumber( property ), stringToRole( role ) ).toString();
}

QVariant Scripting::Project::resourceHeaderData( const QString &property )
{
    int col = resourceColumnNumber( property );
    return m_resourceModel.headerData( col );
}

QObject *Scripting::Project::findResource( const QString &id )
{
    KPlato::Resource *r = project()->findResource( id );
    return r == 0 ? 0 : resource( r );
}

QVariantList Scripting::Project::externalProjects()
{
    QVariantList lst;
    QMap<QString, QString> map = project()->externalProjects();
    for ( QMapIterator<QString, QString> it( map ); it.hasNext(); ) {
        it.next();
        QVariantList m;
        m << it.key() << it.value();
        lst << QVariant(m);
    }
    return lst;
}

void Scripting::Project::addExternalAppointment( QObject *resource, const QVariant &id, const QString &name, const QVariantList &lst )
{
    Resource *r = qobject_cast<Resource*>( resource );
    if ( r == 0 ) {
        return;
    }
    //kDebug()<<id<<name<<lst;
    KPlato::DateTime st = KPlato::DateTime::fromString( lst[0].toString() );
    KPlato::DateTime et = KPlato::DateTime::fromString( lst[1].toString() );
    double load = lst[2].toDouble();
    if ( ! st.isValid() || ! et.isValid() ) {
        return;
    }
    AddExternalAppointmentCmd *cmd = new AddExternalAppointmentCmd( r->kplatoResource(), id.toString(), name, st, et, load, i18nc( "(qtundofrmat)", "Add external appointment" ) );
    cmd->redo();
    m_command->addCommand( cmd );
}

void Scripting::Project::clearExternalAppointments( QObject *resource, const QString &id )
{
    Resource *r = qobject_cast<Resource*>( resource );
    if ( r == 0 ) {
        return;
    }
    ClearExternalAppointmentCmd *cmd = new ClearExternalAppointmentCmd( r->kplatoResource(), id, i18nc( "(qtundofrmat)", "Clear external appointments" ) );
    cmd->redo();
    m_command->addCommand( cmd );
}

void Scripting::Project::clearExternalAppointments( const QString &id )
{
    foreach ( KPlato::Resource *r, project()->resourceList() ) {
        ClearExternalAppointmentCmd *cmd = new ClearExternalAppointmentCmd( r, id, i18nc( "(qtundo_format)", "Clear external appointments" ) );
        cmd->redo();
        m_command->addCommand( cmd );
    }
}

void Scripting::Project::clearExternalAppointments()
{
    ClearAllExternalAppointmentsCmd *cmd = new ClearAllExternalAppointmentsCmd( project(), i18nc( "(qtundo_format)", "Clear all external appointments" ) );
    cmd->redo();
    m_command->addCommand( cmd );
}

int Scripting::Project::calendarCount() const
{
    return project()->calendarCount();
}

QObject *Scripting::Project::calendarAt( int index )
{
    return calendar( project()->calendarAt( index ) );
}

QObject *Scripting::Project::findCalendar( const QString &id )
{
    KPlato::Calendar *c = project()->calendar( id );
    kDebug()<<id<<c;
    return c == 0 ? 0 : calendar( c );
}

QObject *Scripting::Project::createCalendar( QObject *calendar, QObject *parent )
{
    kDebug()<<this<<calendar<<parent;
    if ( calendar == 0 ) {
        kDebug()<<"No calendar specified";
        return 0;
    }
    const Calendar *cal = qobject_cast<Calendar*>( calendar );
    const KPlato::Calendar *copyfrom = cal->kplatoCalendar();
    if ( copyfrom == 0 ) {
        kDebug()<<"Nothing to copy from";
        return 0;
    }
    KPlato::Calendar *c = project()->calendar( copyfrom->id() );
    if ( c ) {
        kDebug()<<"Calendar already exists";
        return this->calendar( c );
    }
    Calendar *par = qobject_cast<Calendar*>( parent );
    KPlato::Calendar *p = 0;
    if ( par ) {
        p = project()->calendar( par->id() );
    }
    c = new KPlato::Calendar();
    *c = *copyfrom;
    c->setId( copyfrom->id() ); // NOTE: id is not copied
    CalendarAddCmd *cmd = new CalendarAddCmd( project(), c, -1, p, i18nc( "(qtundoformat)", "Add calendar" ) );
    cmd->redo();
    m_command->addCommand( cmd );
    Calendar *call = this->calendar( c );
    kDebug()<<call;
    return call;
}

Scripting::Calendar *Scripting::Project::calendar( KPlato::Calendar *calendar )
{
    if ( calendar == 0 ) {
        return 0;
    }
    if ( ! m_calendars.contains( calendar ) ) {
        m_calendars[ calendar ] = new Calendar( this, calendar, parent() );
    }
    return m_calendars[ calendar ];
}

QVariant Scripting::Project::calendarData(const KPlato::Calendar* calendar, const QString& property, const QString& role, long int )
{
    return QVariant();
}

bool Scripting::Project::setCalendarData( KPlato::Calendar *calendar, const QString &property, const QVariant &data, const QString &role )
{
    return false;
}

int Scripting::Project::calendarColumnNumber(const QString& property) const
{
    return m_calendarModel.columnMap().keyToValue( property.toUtf8() );
}

//-----------------------
int Scripting::Project::accountCount() const
{
    return project()->accounts().accountCount();
}

QObject *Scripting::Project::accountAt( int index )
{
    return account( project()->accounts().accountAt( index ) );
}

QObject *Scripting::Project::findAccount( const QString &id )
{
    KPlato::Account *a = project()->accounts().findAccount( id );
    kDebug()<<id<<a;
    return a == 0 ? 0 : account( a );
}

QObject *Scripting::Project::account( KPlato::Account *account )
{
    if ( account == 0 ) {
        return 0;
    }
    if ( ! m_accounts.contains( account ) ) {
        m_accounts[ account ] = new Account( this, account, parent() );
    }
    return m_accounts[ account ];
}

QVariant Scripting::Project::accountHeaderData( const QString &property )
{
    int col = accountColumnNumber( property );
    return m_accountModel.headerData( col );
}

int Scripting::Project::accountColumnNumber( const QString &property ) const
{
    return m_accountModel.columnMap().keyToValue( property.toUtf8() );
}

bool Scripting::Project::setAccountData( KPlato::Account *account, const QString &property, const QVariant &data, const QString &role )
{
    KPlato::AccountItemModel m;
    connect(&m, SIGNAL(executeCommand(KUndo2Command*)), SLOT(slotAddCommand(KUndo2Command*)));
    QModelIndex idx = m.index( account );
    idx = m.index( idx.row(), accountColumnNumber( property ), idx.parent() );
    if ( ! idx.isValid() ) {
        return false;
    }
    return m.setData( idx, data, stringToRole( role ) );
}

QVariant Scripting::Project::accountData( const KPlato::Account *account, const QString &property, const QString &role, long /*schedule*/ )
{
    return m_accountModel.data( account, accountColumnNumber( property ), stringToRole( role ) ).toString();
}

int Scripting::Project::stringToRole( const QString &role ) const
{
    // TODO: use metaobject if possible
    QString r = role;
    if ( r.contains( "::" ) ) {
        r = r.right( role.length() - role.lastIndexOf( "::" ) - 2 );
    }
    if ( r == "DisplayRole" ) {
        return Qt::DisplayRole;
    }
    if ( r == "EditRole" ) {
        return Qt::EditRole;
    }
    kDebug()<<"Role is not handled:"<<role;
    return -1;
}

void Scripting::Project::slotAddCommand( KUndo2Command *cmd )
{
    cmd->redo();
    m_command->addCommand( cmd );
}

#include "Project.moc"
