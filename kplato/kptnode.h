/* This file is part of the KDE project
   Copyright (C) 2001 Thomas Zander zander@kde.org
   Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTNODE_H
#define KPTNODE_H

#include "kptrelation.h"
#include "kptduration.h"
#include "kptdatetime.h"
#include "kptschedule.h"

#include <QObject>
#include <QHash>
#include <QRect>
#include <QString>

#include <KoXmlReader.h>

class QDomElement;

/// The main namespace.
namespace KPlato
{

class Account;
class Project;
class Appointment;
class ResourceGroup;
class Resource;
class ResourceGroupRequest;
class Estimate;
class WBSDefinition;
class EffortCostMap;

/**
 * This class represents any node in the project, a node can be a project or
 * a subproject or any task.
 * This class is basically an abstract interface to make the design more OO.
 */
class Node : public QObject
{
    Q_OBJECT
public:
    enum ConstraintType { ASAP, ALAP, MustStartOn, MustFinishOn, StartNotEarlier, FinishNotLater, FixedInterval };

    enum State { 
        State_None = 0,
        State_Started = 1,
        State_StartedLate = 2,
        State_StartedEarly = 4,
        State_Finished = 8,
        State_FinishedLate = 16,
        State_FinishedEarly = 32,
        State_Running = 64,
        State_RunningLate = 128,
        State_RuningEarly = 256
    };

    Node(Node *parent = 0);
    Node(Node &node, Node *parent = 0);


    // Declare the class abstract
    virtual ~Node() = 0;

    bool setId(const QString& id);
    QString id() const { return m_id; } // unique identity
    
    enum NodeTypes {
	  Type_Node = 0,
	  Type_Project = 1,
	  Type_Subproject = 2,
	  Type_Task = 3,
	  Type_Milestone = 4,
	  Type_Periodic = 5,
      Type_Summarytask = 6
    };

    virtual int type() const = 0;
    
    QString typeToString( bool trans = false ) const;
    /**
     * Returns a pointer to the project node (main- or sub-project)
     * Returns 0 if no project exists.
     */
    virtual Node *projectNode();
    
    // The load and save methods
    virtual bool load(KoXmlElement &) { return true; }
    virtual bool load(KoXmlElement &, Project &) { return true; }
    virtual void save(QDomElement &element) const  = 0;
    /// Save me and my childrens relations.
    virtual void saveRelations(QDomElement &element) const;

    // simple child node management
    // Child nodes are things like subtasks, basically a task can exists of
    // several sub-tasks. Creating a table has 4 subtasks, 1) measuring
    // 2) cutting 3) building 4) painting.
    Node *parentNode() const { return m_parent; }
    void setParentNode( Node* newParent ) { m_parent = newParent;}
    const QList<Node*> &childNodeIterator() const { return m_nodes; }
    int numChildren() const { return m_nodes.count(); }
    virtual void addChildNode(Node *node, Node *after=0);
    virtual void insertChildNode(int index, Node *node);
    void takeChildNode(Node *node );
    void takeChildNode(int number );
    Node* childNode(int number);
    const Node* childNode(int number) const;
    int findChildNode( Node* node );
    bool isChildOf( Node *node );
    int indexOf( const Node *node ) const;

    // Time-dependent child-node-management.
    // list all nodes that are dependent upon this one.
    // Building a house requires the table to be finished, therefore the
    // house-building is time dependent on the table-building. So a child
    // of the table-building node is the house-building node.

    int numDependChildNodes() const { return m_dependChildNodes.count(); }
    /// Adds relation of type @p p to both this node and @p node
    virtual void addDependChildNode( Node *node, Relation::Type p=Relation::FinishStart);
    /// Adds relation of type @p p with @p lag to both this node and @p node
    virtual void addDependChildNode( Node *node, Relation::Type p, Duration lag);
    /// Adds @p relation only to this node
    virtual bool addDependChildNode( Relation *relation);
    /// Inserts relation to this node at index @p index and appends relation to @p node
    virtual void insertDependChildNode( unsigned int index, Node *node, Relation::Type p=Relation::FinishStart);
    /**
     * Takes the relation rel from this node only.
     */
    void takeDependChildNode( Relation *rel );
    Relation *getDependChildNode( int number ) {
        return m_dependChildNodes.at(number);
    }
    QList<Relation*> &dependChildNodes() { return m_dependChildNodes; }

    int numDependParentNodes() const { return m_dependParentNodes.count(); }
    /// Adds relation if type @p to both this node and @p node
    virtual void addDependParentNode(Node *node, Relation::Type p=Relation::FinishStart);
    /// Adds relation to both this node and @p node
    virtual void addDependParentNode( Node *node, Relation::Type p, Duration lag);
    /// Adds relation only to this node
    virtual bool addDependParentNode( Relation *relation);
    /// Inserts relation to this node at index and appends relation to @p node
    virtual void insertDependParentNode( unsigned int index, Node *node, Relation::Type p=Relation::FinishStart);
    /**
     * Takes the relation @p rel from this node only.
     */
    void takeDependParentNode( Relation *rel );
    Relation *getDependParentNode( int number ) {
        return m_dependParentNodes.at(number);
    }
    QList<Relation*> &dependParentNodes() { return m_dependParentNodes; }
    QList<Node*> getParentNodes();
    bool isParentOf(Node *node);
    bool isDependChildOf(Node *node);
    virtual bool canMoveTo( Node *newParent );
    
    Relation *findParentRelation(Node *node);
    Relation *findChildRelation(Node *node);
    Relation *findRelation(Node *node);

    /// Set the scheduled start time
    void setStartTime(DateTime startTime, long id = -1 );
    /// Return the scheduled start time
    virtual DateTime startTime( long id = -1 ) const;
    /// Set the scheduled end time
    void setEndTime(DateTime endTime, long id = -1 );
    /// Return the scheduled end time
    virtual DateTime endTime( long id = -1 ) const;

    /// Return the estimate for this node
    Estimate *estimate() const { return m_estimate; }

    /**
     * Instead of using the expected duration, generate a random value using
     * the Distribution of each Task. This can be used for Monte-Carlo
     * estimation of Project duration.
     */
    virtual Duration *getRandomDuration() = 0;
    /**
     * Calculate the delay of this node. 
     * It is the difference between the actual startTime and scheduled startTime.
     */
    Duration *getDelay(); // TODO


    QString &name() { return m_name; }
    QString &leader() { return m_leader; }
    QString &description() { return m_description; }
    const QString &name() const { return m_name; }
    const QString &leader() const { return m_leader; }
    const QString &description() const { return m_description; }
    void setName(const QString &n);
    void setLeader(const QString &l);
    void setDescription(const QString &d);

    void setConstraint(Node::ConstraintType type);
    void setConstraint(QString &type);
    int constraint() const { return m_constraint; }
    QString constraintToString( bool trans=false ) const;
    static QStringList constraintList( bool trans );
    
    virtual void setConstraintStartTime(DateTime time) 
        { m_constraintStartTime = time; changed( this ); }
    virtual void setConstraintEndTime(DateTime time) 
        { m_constraintEndTime = time; changed( this ); }

    virtual DateTime constraintStartTime() const { return m_constraintStartTime; }
    virtual DateTime constraintEndTime() const { return m_constraintEndTime; }
    virtual DateTime startNotEarlier() const { return m_constraintStartTime; }
    virtual DateTime finishNotLater() const { return m_constraintEndTime; }
    virtual DateTime mustStartOn() const { return m_constraintStartTime; }
    virtual DateTime mustFinishOn() const { return m_constraintEndTime; }

    virtual ResourceGroupRequest *resourceGroupRequest(ResourceGroup */*group*/) const { return 0; }
    virtual QStringList requestNameList() const { return QStringList(); }
    virtual bool containsRequest( const QString &/*identity*/ ) const { return false; }
    virtual ResourceRequest *resourceRequest( const QString &/*name*/ ) const { return 0; }
    
    virtual void makeAppointments();
    /// Calculates if the assigned resource is overbooked 
    /// within the duration of this node
    virtual void calcResourceOverbooked();

    /// EstimateType == Estimate, but no resource is requested
    bool resourceError( long id = -1 ) const;
    /// The assigned resource is overbooked
    virtual bool resourceOverbooked( long id = -1 ) const;
    /// The requested resource is not available
    bool resourceNotAvailable( long id = -1 ) const;
    /// The task cannot be scheduled to fullfill all the constraints
    virtual bool schedulingError( long id = -1 ) const;
    /// The node has not been scheduled
    bool notScheduled( long id = -1 ) const;
    /// Return a list of overbooked resources
    virtual QStringList overbookedResources( long id = -1 ) const;
    
    virtual EffortCostMap plannedEffortCostPrDay(const QDate &start, const QDate &end, long id = -1 ) const=0;
        
    /// Returns the total planned effort for this task (or subtasks) 
    virtual Duration plannedEffort( long id = -1 ) const { Q_UNUSED(id); return Duration::zeroDuration; }
    /// Returns the total planned effort for this task (or subtasks) on date
    virtual Duration plannedEffort(const QDate &, long id = -1 ) const { Q_UNUSED(id); return Duration::zeroDuration; }
    /// Returns the planned effort up to and including date
    virtual Duration plannedEffortTo(const QDate &, long id = -1 ) const { Q_UNUSED(id); return Duration::zeroDuration; }
    
    /// Returns the total actual effort for this task (or subtasks) 
    virtual Duration actualEffort( long id = -1 ) const { Q_UNUSED(id); return Duration::zeroDuration; }
    /// Returns the total actual effort for this task (or subtasks) on date
    virtual Duration actualEffort(const QDate &/*date*/, long id = -1 ) const { Q_UNUSED(id); return Duration::zeroDuration; }
    /// Returns the total actual effort for this task (or subtasks) up to and including date
    virtual Duration actualEffortTo(const QDate &/*date*/, long id = -1 ) const { Q_UNUSED(id); return Duration::zeroDuration; }
    
    /**
     * Planned cost is the sum total of all resources and other costs
     * planned for this node.
     */
    virtual double plannedCost( long id = -1 ) const { Q_UNUSED(id); return 0; }
    
    /// Planned cost on date
    virtual double plannedCost(const QDate &/*date*/, long id = -1 ) const { Q_UNUSED(id); return 0; }
    /**
     * Planned cost from start of activity up to and including date
     * is the sum of all resource costs and other costs planned for this node.
     */
    virtual double plannedCostTo(const QDate &/*date*/, long id = -1 ) const { Q_UNUSED(id); return 0; }
    /**
     * Actual cost is the sum total of the reported costs actually used
     * for this node.
     */
    virtual double actualCost( long id = -1 ) const { Q_UNUSED(id); return 0; }
    /// Actual cost on date
    virtual double actualCost(const QDate &/*date*/, long id = -1 ) const { Q_UNUSED(id); return 0; }
    /// Actual cost up to and including date
    virtual double actualCostTo(const QDate &/*date*/, long id = -1 ) const { Q_UNUSED(id); return 0; }
    
    /// Budgeted Cost of Work Performed
    virtual double bcwp( long id ) const { Q_UNUSED(id); return 0.0; }
    /// Budgeted Cost of Work Performed ( up to @p date )
    virtual double bcwp( const QDate &/*date*/, long id = -1 ) const { Q_UNUSED(id); return 0.0; }
    
            /// Effort based performance index
    double effortPerformanceIndex(const QDate &/*date*/, bool */*error=0*/) const { return 0.0; }
    /// Cost performance index
    double costPerformanceIndex(const QDate &/*date*/, bool */*error=0*/) const { return 0.0; }
    
    virtual void initiateCalculationLists(MainSchedule &sch) = 0;
    virtual DateTime calculateForward(int /*use*/) = 0;
    virtual DateTime calculateEarlyFinish(int /*use*/) { return DateTime(); }
    virtual DateTime calculateBackward(int /*use*/) = 0;
    virtual DateTime calculateLateStart(int /*use*/) { return DateTime(); }
    virtual DateTime scheduleForward(const DateTime &, int /*use*/) = 0;
    virtual DateTime scheduleFromStartTime(int /*use*/) { return DateTime(); }
    virtual DateTime scheduleBackward(const DateTime &, int /*use*/) = 0;
    virtual DateTime scheduleFromEndTime(int /*use*/) { return DateTime(); }
    virtual void adjustSummarytask() = 0;

    /// Returns the (previously) calculated duration
    const Duration &duration( long id = -1 );
    
    Node *siblingBefore();
    Node *childBefore(Node *node);
    Node *siblingAfter();
    Node *childAfter(Node *node);
    bool moveChildUp(Node *node);
    bool moveChildDown(Node *node);
    
    /// Check if this node can be linked to node
    bool legalToLink(Node *node);
    /// Check if node par can be linked to node child. (Reimplement)
    virtual bool legalToLink(Node *, Node *) { return false; }

    /// Save appointments for schedule with id
    virtual void saveAppointments(QDomElement &element, long id) const;
    ///Return the list of appointments for schedule with id.
    QList<Appointment*> appointments( long id = -1 );
    /// Adds appointment to this node only (not to resource)
    virtual bool addAppointment(Appointment *appointment, Schedule &main);
    
    /// Find the node with my id
    virtual Node *findNode() const { return findNode(m_id); }
    /// Find the node with identity id
    virtual Node *findNode(const QString &id) const
        { return (m_parent ? m_parent->findNode(id) : 0); }
    /// Remove myself from the id register
    virtual bool removeId()  { return removeId(m_id); }
    /// Remove the registered identity id
    virtual bool removeId(const QString &id)
        { return (m_parent ? m_parent->removeId(id) : false); }
    /// Insert myself into the id register
    virtual void insertId(const QString &id) { insertId(id, this); }
    /// Insert node with identity id into the register
    virtual void insertId(const QString &id, Node *node)
        { if (m_parent) m_parent->insertId(id, node); }
    
    /**
     * This is when work can start on this node in accordance with 
     * the calendar of allocated resources. Normally this is the same
     * as @ref startTime(), but may differ if timing constraints are set.
     */
    virtual DateTime workStartTime( long id = -1 ) const;
    void setWorkStartTime(const DateTime &dt,  long id = -1 );
    
    /**
     * This is when work can finish on this node in accordance with 
     * the calendar of allocated resources. Normally this is the same
     * as @ref endTime(), but may differ if timing constraints are set.
     */
    virtual DateTime workEndTime( long id = -1 ) const;
    void setWorkEndTime(const DateTime &dt,  long id = -1 );
    
    virtual bool isCritical( long id = -1 ) const { Q_UNUSED(id); return false; }
    virtual bool inCriticalPath( long id = -1 ) const;
    /// Returns the level this node is in the hierarchy. Top node is level 0.
    virtual int level();
    /// Generate WBS
    virtual void generateWBS(int count, WBSDefinition &def, const QString& wbs=QString());
    QString wbs() const { return m_wbs; }
    
    double startupCost() const { return m_startupCost; }
    void setStartupCost(double cost);
    
    Account *startupAccount() const { return m_startupAccount; }
    void setStartupAccount(Account *acc);
    
    double shutdownCost() const { return m_shutdownCost; }
    void setShutdownCost(double cost);
    
    Account *shutdownAccount() const { return m_shutdownAccount; }
    void setShutdownAccount(Account *acc);
    
    Account *runningAccount() const { return m_runningAccount; }
    void setRunningAccount(Account *acc);

    /// Return current schedule
    Schedule *currentSchedule() const { return m_currentSchedule; }
    /// Set current schedule to schedule with identity id, for me and my children
    virtual void setCurrentSchedule(long id);
    
    QHash<long, Schedule*> &schedules() { return m_schedules; }
    /// Find schedule matching name and type. Does not return deleted schedule.
    Schedule *findSchedule(const QString name, const Schedule::Type type);
    /// Find schedule matching name. Does not return deleted schedule.
    Schedule *findSchedule(const QString name);
    /// Find schedule matching type.  Does not return deleted schedule.
    Schedule *findSchedule(const Schedule::Type type);
    /// Find schedule matching id.  Also returns deleted schedule.
    Schedule *findSchedule(long id) const;
    
    /// Take, don't delete (as in destruct).
    void takeSchedule(const Schedule *schedule);
    /// Add schedule to list, replace if schedule with same id already exists.
    void addSchedule(Schedule *schedule);
    /// Create a new schedule.
    Schedule *createSchedule(const QString& name, Schedule::Type type, long id);
    /// Create a new schedule.
    Schedule *createSchedule(Schedule *parent);
    
    /// Set deleted = onoff for schedule with id
    void setScheduleDeleted(long id, bool onoff);
    /// Set parent schedule recursivly
    virtual void setParentSchedule(Schedule *sch);
    
    virtual ResourceRequestCollection *requests() const { return 0; }
    
    virtual uint state( long ) const { return State_None; }
    
public:
    // These shouldn't be available to other than those who inherits
    /// Calculate the critical path
    virtual bool calcCriticalPath(bool fromEnd);
    virtual void calcFreeFloat();
    
    /// Check if this node has any dependent child nodes
    virtual bool isEndNode() const;
    /// Check if this node has any dependent parent nodes
    virtual bool isStartNode() const;
    
    virtual void initiateCalculation(MainSchedule &sch);
    virtual void resetVisited();
    void propagateEarliestStart(DateTime &time);
    void propagateLatestFinish(DateTime &time);
    void moveEarliestStart(DateTime &time);
    void moveLatestFinish(DateTime &time);
    // Reimplement this
    virtual Duration summarytaskDurationForward(const DateTime &/*time*/) 
    { return Duration::zeroDuration; }
    // Reimplement this
    virtual DateTime summarytaskEarliestStart() 
    { return DateTime(); }
    // Reimplement this
    virtual Duration summarytaskDurationBackward(const DateTime &/*time*/) 
    { return Duration::zeroDuration; }
    // Reimplement this
    virtual DateTime summarytaskLatestFinish() 
    { return DateTime(); }
    
    /**
     * earlyStart() returns earliest time this node can start
     * given the constraints of the network.
     */
    DateTime earlyStart( long id = -1 ) const;
    /**
     * setEarlyStart() sets earliest time this node can start
     */
    void setEarlyStart(const DateTime &dt, long id = -1 );
    /**
     * lateStart() returns latest time this node can start
     * given the constraints of the network.
     */
    DateTime lateStart( long id = -1 ) const;
    /**
     * setLateStart() sets the earliest time this node can start
     */
    void setLateStart(const DateTime &dt, long id = -1 );
    /**
     * earlyFinish() returns earliest time this node can finish
     * given the constraints of the network.
     */
    DateTime earlyFinish( long id = -1 ) const;
    /**
     * setEarlyFinish() sets earliest time this node can finish
     */
    void setEarlyFinish(const DateTime &dt, long id = -1 );
    /**
     * lateFinish() returns latest time this node can finish
     * given the constraints of the network.
     */
    DateTime lateFinish( long id = -1 ) const;
    /**
     * setLateFinish() sets latest time this node can finish
     */
    void setLateFinish(const DateTime &dt, long id = -1 );
    
    /// Adds appointment to both this node and resource
    virtual void addAppointment(ResourceSchedule *resource, DateTime &start, DateTime &end, double load=100);
    
    virtual void clearProxyRelations() {}
    virtual void addParentProxyRelations(QList<Relation*> &) {}
    virtual void addChildProxyRelations(QList<Relation*> &) {}
    virtual void addParentProxyRelation(Node *, const Relation *) {}
    virtual void addChildProxyRelation(Node *, const Relation *) {}
    
    virtual void changed() { changed( this ); }
    Duration getmDurationForward(){ return this->m_durationForward;}
    
protected:
    /**
     * Calculates and returns the duration of the node.
     * Reimplement.
     */
    virtual Duration duration(const DateTime &/*time*/, int /*use*/, bool /*backward*/)
        { return Duration::zeroDuration; }

    // NOTE: Cannot use setCurrentSchedule() due to overload/casting problems
    void setCurrentSchedulePtr(Schedule *schedule) { m_currentSchedule = schedule; }
    virtual void changed(Node *node);
    
    QList<Node*> m_nodes;
    QList<Relation*> m_dependChildNodes;
    QList<Relation*> m_dependParentNodes;
    QList<Node*>m_parentNodes;
    Node *m_parent;
    

    QString m_id; // unique id
    QString m_name;        // Name of this node
    QString m_leader;      // Person or group responsible for this node
    QString m_description; // Description of this node

    Estimate *m_estimate;
    

    ConstraintType m_constraint;

    /**
      * m_constraintTime is used if any of the constraints
      * FixedInterval, StartNotEarlier, MustStartOn or FixedInterval is selected
      */
    DateTime m_constraintStartTime;
    /**
      * m_constraintEndTime is used if any of the constraints
      * FixedInterval, FinishNotLater, MustFinishOn or FixedInterval is selected
      */
    DateTime m_constraintEndTime;

    bool m_visitedForward;
    bool m_visitedBackward;
    Duration m_durationForward;
    Duration m_durationBackward;
    DateTime m_earlyStart;
    DateTime m_lateStart;
    DateTime m_earlyFinish;
    DateTime m_lateFinish;
    
    QHash<long, Schedule*> m_schedules;
    Schedule *m_currentSchedule;

    QString m_wbs;
    
    double m_startupCost;
    Account *m_startupAccount;
    double m_shutdownCost;
    Account *m_shutdownAccount;
    Account *m_runningAccount;
    
private:
    void init();
        
#ifndef NDEBUG
public:
    virtual void printDebug(bool children, const QByteArray& indent);
#endif

};

////////////////////////////////   Estimate   ////////////////////////////////
/**
  * Any @ref Node will store how much time it takes to complete the node
  * (typically a @ref Task) in the traditional scheduling software the
  * estimate which is needed to complete the node is not simply a timespan but
  * is stored as an optimistic, a pessimistic and an expected timespan.
  */
class Estimate {
public:
    explicit Estimate ( Duration e = Duration::zeroDuration, Duration p = Duration::zeroDuration, Duration o = Duration::zeroDuration );

    explicit Estimate ( double e, double p = 0, double o = 0);
    
    Estimate (const Estimate &estimate);
    ~Estimate();

    enum Type {
        Type_Effort = 0, /// Changing amount of resources changes the task duration
        Type_FixedDuration = 1 /// Changing amount of resources will not change the tasks duration
    };
    Node *parentNode() const { return m_parent; }
    void setParentNode( Node* parent ) { m_parent = parent; }
    
    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; changed(); }
    void setType(const QString& type);
    QString typeToString( bool trans=false ) const;
    static QStringList typeToStringList( bool trans=false );
    
    enum Risktype { Risk_None, Risk_Low, Risk_High };
    Risktype risktype() const { return m_risktype; }
    void setRisktype(Risktype type) { m_risktype = type; changed(); }
    void setRisktype(const QString& type);
    QString risktypeToString( bool trans=false ) const;
    static QStringList risktypeToStringList( bool trans=false );

    enum Use { Use_Expected=0, Use_Optimistic=1, Use_Pessimistic=2 };
    Duration value(int valueType, bool pert) const;
    const Duration& optimistic() const {return m_optimisticEstimate;}
    const Duration& pessimistic() const {return m_pessimisticEstimate;}
    const Duration& expected() const {return m_expectedEstimate;}

    /// The unit in which this value was last entered.
    Duration::Unit displayUnit() const { return m_displayUnit; }
    /// Set display unit.
    void setDisplayUnit( Duration::Unit unit ) { m_displayUnit = unit; }
    
    void set( Duration e, Duration p = Duration::zeroDuration, Duration o = Duration::zeroDuration );
    void set( int e, int p = -1, int o = -1 );
    void set(unsigned days, unsigned hours, unsigned minutes);
    void expectedEstimate(unsigned *days, unsigned *hours, unsigned *minutes);

    bool load(KoXmlElement &element);
    void save(QDomElement &element) const;

    /**
     * Set the optimistic duration
     * @param percent should be a negative value.
     */
    void setOptimisticRatio(int percent);
    /**
     * Return the "optimistic" duration as deviation from "expected" in percent.
     * This should be a negative value.
     */
    int optimisticRatio() const;
    /**
     * Set the pessimistic duration
     * @param percent should be a positive value.
     */
    void setPessimisticRatio(int percent);
    /**
     * Return the "pessimistic" duration as the deviation from "expected" in percent.
     * This should be a positive value.
     */
    int pessimisticRatio() const;

    Duration variance() const;
    Duration pertExpected() const;
    Duration pertOptimistic() const;
    Duration pertPessimistic() const;

    static double scale( const Duration &value, Duration::Unit unit, QList<double> scales );
    static Duration scale( double value, Duration::Unit unit, QList<double> scales );

protected:
    void changed() { if ( m_parent ) m_parent->changed(); }
    
private:
    Node *m_parent;
    Duration m_optimisticEstimate;
    Duration m_pessimisticEstimate;
    Duration m_expectedEstimate;

    Type m_type;
    Risktype m_risktype;
    
    Duration::Unit m_displayUnit;
    
    
#ifndef NDEBUG
public:
    void printDebug(const QByteArray& indent);
#endif

};

}  //KPlato namespace

#endif
