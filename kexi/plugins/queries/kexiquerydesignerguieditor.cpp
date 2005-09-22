/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiquerydesignerguieditor.h"

#include <qlayout.h>
#include <qpainter.h>
#include <qdom.h>
#include <qregexp.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <kexidb/field.h>
#include <kexidb/queryschema.h>
#include <kexidb/connection.h>
#include <kexidb/parser/parser.h>
#include <kexidb/parser/sqlparser.h>
#include <kexidb/utils.h>
#include <kexiutils/identifier.h>
#include <kexiproject.h>
#include <keximainwindow.h>
#include <kexiinternalpart.h>
#include <kexitableview.h>
#include <kexitableitem.h>
#include <kexitableviewdata.h>
#include <kexidragobjects.h>
#include <kexidialogbase.h>
#include <kexidatatable.h>
#include <kexi.h>
#include <kexisectionheader.h>
#include <widget/tableview/kexidataawarepropertyset.h>
#include <widget/relations/kexirelationwidget.h>
#include <widget/relations/kexirelationviewtable.h>
#include <koproperty/property.h>
#include <koproperty/set.h>

#include "kexiquerypart.h"

//! indices for table columns
#define COLUMN_ID_COLUMN 0
#define COLUMN_ID_TABLE 1
#define COLUMN_ID_VISIBLE 2
#define COLUMN_ID_TOTALS 3
#define COLUMN_ID_CRITERIA 4

/*! @internal */
class KexiQueryDesignerGuiEditorPrivate
{
public:
	KexiQueryDesignerGuiEditorPrivate()
		: fieldColumnIdentifiers(101, false/*case insens.*/)
	{
		droppedNewItem = 0;
		slotTableAdded_enabled = true;
	}

	KexiTableViewData *data;
	KexiDataTable *dataTable;
	QGuardedPtr<KexiDB::Connection> conn;

	KexiRelationWidget *relations;
	KexiSectionHeader *head;
	QSplitter *spl;

	/*! Used to remember in slotDroppedAtRow() what data was dropped,
	 so we can create appropriate prop. set in slotRowInserted()
	 This information is cached and entirely refreshed on updateColumnsData(). */
	KexiTableViewData *fieldColumnData, *tablesColumnData;

	/*! Collects identifiers selected in 1st (field) column,
	 so we're able to distinguish between table identifiers selected from
	 the dropdown list, and strings (e.g. expressions) entered by hand.
	 This information is cached and entirely refreshed on updateColumnsData().
	 The dict is filled with (char*)1 values (doesn't matter what it is);
	*/
	QDict<char> fieldColumnIdentifiers;

	KexiDataAwarePropertySet* sets;
	KexiTableItem *droppedNewItem;

	QString droppedNewTable, droppedNewField;

	bool slotTableAdded_enabled : 1;
};

//=========================================================

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(
	KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : KexiViewBase(mainWin, parent, name)
 , d( new KexiQueryDesignerGuiEditorPrivate() )
{
	d->conn = mainWin->project()->dbConnection();

	d->spl = new QSplitter(Vertical, this);
	d->spl->setChildrenCollapsible(false);
	d->relations = new KexiRelationWidget(mainWin, d->spl, "relations");
	connect(d->relations, SIGNAL(tableAdded(KexiDB::TableSchema&)),
		this, SLOT(slotTableAdded(KexiDB::TableSchema&)));
	connect(d->relations, SIGNAL(tableHidden(KexiDB::TableSchema&)),
		this, SLOT(slotTableHidden(KexiDB::TableSchema&)));
	connect(d->relations, SIGNAL(tableFieldDoubleClicked(KexiDB::TableSchema*,const QString&)),
		this, SLOT(slotTableFieldDoubleClicked(KexiDB::TableSchema*,const QString&)));

	d->head = new KexiSectionHeader(i18n("Query columns"), Vertical, d->spl);
	d->dataTable = new KexiDataTable(mainWin, d->head, "guieditor_dataTable", false);
	d->dataTable->dataAwareObject()->setSpreadSheetMode();

	d->data = new KexiTableViewData(); //just empty data
	d->sets = new KexiDataAwarePropertySet( this, d->dataTable->dataAwareObject() );
	initTableColumns();
	initTableRows();

	QValueList<int> c;
	c << COLUMN_ID_COLUMN << COLUMN_ID_TABLE << COLUMN_ID_CRITERIA;
	if (d->dataTable->tableView()/*sanity*/) {
		d->dataTable->tableView()->maximizeColumnsWidth( c );
		d->dataTable->tableView()->adjustColumnWidthToContents(2);//'visible'
		d->dataTable->tableView()->setDropsAtRowEnabled(true);
		connect(d->dataTable->tableView(), SIGNAL(dragOverRow(KexiTableItem*,int,QDragMoveEvent*)),
			this, SLOT(slotDragOverTableRow(KexiTableItem*,int,QDragMoveEvent*)));
		connect(d->dataTable->tableView(), SIGNAL(droppedAtRow(KexiTableItem*,int,QDropEvent*,KexiTableItem*&)),
			this, SLOT(slotDroppedAtRow(KexiTableItem*,int,QDropEvent*,KexiTableItem*&)));
	}
	connect(d->data, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)),
		this, SLOT(slotBeforeCellChanged(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)));
	connect(d->data, SIGNAL(rowInserted(KexiTableItem*,uint,bool)),
		this, SLOT(slotRowInserted(KexiTableItem*,uint,bool)));
	connect(d->relations, SIGNAL(tablePositionChanged(KexiRelationViewTableContainer*)),
		this, SLOT(slotTablePositionChanged(KexiRelationViewTableContainer*)));
	connect(d->relations, SIGNAL(aboutConnectionRemove(KexiRelationViewConnection*)),
		this, SLOT(slotAboutConnectionRemove(KexiRelationViewConnection*)));

	QVBoxLayout *l = new QVBoxLayout(this);
	l->addWidget(d->spl);

	addChildView(d->relations);
	addChildView(d->dataTable);
	setViewWidget(d->dataTable, true);
	d->relations->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	d->head->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	updateGeometry();
	d->spl->setSizes(QValueList<int>()<< 800<<400);
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor()
{
}

void
KexiQueryDesignerGuiEditor::initTableColumns()
{
	KexiTableViewColumn *col1 = new KexiTableViewColumn("column", KexiDB::Field::Enum, i18n("Column"),
		i18n("Describes field name or expression for the designed query."));
	col1->setRelatedDataEditable(true);

	d->fieldColumnData = new KexiTableViewData(KexiDB::Field::Text, KexiDB::Field::Text);
	col1->setRelatedData( d->fieldColumnData );
	d->data->addColumn(col1);

	KexiTableViewColumn *col2 = new KexiTableViewColumn("table", KexiDB::Field::Enum, i18n("Table"),
		i18n("Describes table for a given field. Can be empty."));
	d->tablesColumnData = new KexiTableViewData(KexiDB::Field::Text, KexiDB::Field::Text);
	col2->setRelatedData( d->tablesColumnData );
	d->data->addColumn(col2);

	KexiTableViewColumn *col3 = new KexiTableViewColumn("visible", KexiDB::Field::Boolean, i18n("Visible"),
		i18n("Describes visibility for a given field or expression."));
	d->data->addColumn(col3);

	KexiTableViewColumn *col4 = new KexiTableViewColumn("totals", KexiDB::Field::Enum, i18n("Totals"),
		i18n("Describes a way of computing totals for a given field or expression."));
	QValueVector<QString> totalsTypes;
	totalsTypes.append( i18n("Group by") );
	totalsTypes.append( i18n("Sum") );
	totalsTypes.append( i18n("Average") );
	totalsTypes.append( i18n("Min") );
	totalsTypes.append( i18n("Max") );
	//todo: more like this
	col4->field()->setEnumHints(totalsTypes);
	d->data->addColumn(col4);

/*TODO
f= new KexiDB::Field(i18n("Sort"), KexiDB::Field::Enum);
	QValueVector<QString> sortTypes;
	sortTypes.append( i18n("Ascending") );
	sortTypes.append( i18n("Descending") );
	sortTypes.append( i18n("No sorting") );
	f->setEnumHints(sortTypes);
	KexiTableViewColumn *col5 = new KexiTableViewColumn(*f);
	d->data->addColumn(col5);*/

	KexiTableViewColumn *col6 = new KexiTableViewColumn("criteria", KexiDB::Field::Text, i18n("Criteria"),
		i18n("Describes criateria for a given field or expression."));
	d->data->addColumn(col6);

//	KexiTableViewColumn *col7 = new KexiTableViewColumn(i18n("Or"), KexiDB::Field::Text);
//	d->data->addColumn(col7);
}

void KexiQueryDesignerGuiEditor::initTableRows()
{
	d->data->deleteAllRows();
	const int columns = d->data->columnsCount();
	for (int i=0; i<(int)d->sets->size(); i++) {
		d->data->append(d->data->createItem());
	}
	d->dataTable->dataAwareObject()->setData(d->data);

	updateColumnsData();
}

void KexiQueryDesignerGuiEditor::updateColumnsData()
{
	d->dataTable->dataAwareObject()->acceptRowEdit();

	QStringList sortedTableNames;
	for (TablesDictIterator it(*d->relations->tables());it.current();++it)
		sortedTableNames += it.current()->schema()->name();
	qHeapSort( sortedTableNames );

	//several tables can be hidden now, so remove rows for these tables
	QValueList<int> rowsToDelete;
	for (int r = 0; r<(int)d->sets->size(); r++) {
		KoProperty::Set *set = d->sets->at(r);
		if (set) {
			QString tableName = (*set)["table"].value().toString();
			if (tableName!="*"
				&& !(*set)["isExpression"].value().toBool()
				&& sortedTableNames.end() == qFind( sortedTableNames.begin(), sortedTableNames.end(), tableName ))
			{
				//table not found: mark this line for later remove
				rowsToDelete += r;
			}
		}
	}
	d->data->deleteRows( rowsToDelete );

	//update 'table' and 'field' columns
	d->tablesColumnData->deleteAllRows();
	d->fieldColumnData->deleteAllRows();
	d->fieldColumnIdentifiers.clear();

	KexiTableItem *item = d->fieldColumnData->createItem(); //new KexiTableItem(2);
	(*item)[COLUMN_ID_COLUMN]="*";
	(*item)[COLUMN_ID_TABLE]="*";
	d->fieldColumnData->append( item );
	d->fieldColumnIdentifiers.insert((*item)[COLUMN_ID_COLUMN].toString(), (char*)1); //cache

//	tempData()->clearQuery();
	tempData()->unregisterForTablesSchemaChanges();
	for (QStringList::const_iterator it = sortedTableNames.constBegin();
		it!=sortedTableNames.constEnd(); ++it)
	{
		//table
/*! @todo what about query? */
		KexiDB::TableSchema *table = d->relations->tables()->find(*it)->schema()->table();
		d->conn->registerForTableSchemaChanges(*tempData(), *table); //this table will be used
		item = d->tablesColumnData->createItem(); //new KexiTableItem(2);
		(*item)[COLUMN_ID_COLUMN]=table->name();
		(*item)[COLUMN_ID_TABLE]=(*item)[COLUMN_ID_COLUMN];
		d->tablesColumnData->append( item );
		//fields
		item = d->fieldColumnData->createItem(); //new KexiTableItem(2);
		(*item)[COLUMN_ID_COLUMN]=table->name()+".*";
		(*item)[COLUMN_ID_TABLE]=(*item)[COLUMN_ID_COLUMN];
		d->fieldColumnData->append( item );
		d->fieldColumnIdentifiers.insert((*item)[COLUMN_ID_COLUMN].toString(), (char*)1); //cache
		for (KexiDB::Field::ListIterator t_it = table->fieldsIterator();t_it.current();++t_it) {
			item = d->fieldColumnData->createItem(); // new KexiTableItem(2);
			(*item)[COLUMN_ID_COLUMN]=table->name()+"."+t_it.current()->name();
			(*item)[COLUMN_ID_TABLE]=QString("  ") + t_it.current()->name();
			d->fieldColumnData->append( item );
			d->fieldColumnIdentifiers.insert((*item)[COLUMN_ID_COLUMN].toString(), (char*)1); //cache
		}
	}
//TODO
}

KexiRelationWidget *KexiQueryDesignerGuiEditor::relationView() const
{
	return d->relations;
}

/*
void
KexiQueryDesignerGuiEditor::addRow(const QString &tbl, const QString &field)
{
	kexipluginsdbg << "KexiQueryDesignerGuiEditor::addRow(" << tbl << ", " << field << ")" << endl;
	KexiTableItem *item = d->data->createItem(); //new KexiTableItem(0);
	(*item)[0] = tbl;
	(*item)[1] = field;
	(*item)[2] = QVariant(true, 1);
//null	(*item)[3] =
	d->data->append(item);

	//TODO: this should deffinitly not go here :)
//	m_table->updateContents();

	setDirty(true);
}*/

KexiQueryPart::TempData *
KexiQueryDesignerGuiEditor::tempData() const
{
	return static_cast<KexiQueryPart::TempData*>(parentDialog()->tempData());
}

static QString msgCannotSwitch_EmptyDesign() {
	return i18n("Cannot switch to data view, because query design is empty.\n"
		"First, please create your design.");
}

bool
KexiQueryDesignerGuiEditor::buildSchema(QString *errMsg)
{
	//build query schema
	KexiQueryPart::TempData * temp = tempData();
	if (temp->query) {
		temp->clearQuery();
	} else {
		temp->query = new KexiDB::QuerySchema();
	}

	//add tables
	for (TablesDictIterator it(*d->relations->tables()); it.current(); ++it) {
/*! @todo what about query? */
		temp->query->addTable( it.current()->schema()->table() );
	}

	//add fields
	KexiDB::BaseExpr *whereExpr = 0;
	KexiTableViewData::Iterator it(d->data->iterator());
	const uint count = QMIN(d->data->count(), d->sets->size());
	bool fieldsFound = false;
	for (uint i=0; i<count && it.current(); ++it, i++) {
		if (!it.current()->at(COLUMN_ID_TABLE).isNull() && it.current()->at(COLUMN_ID_COLUMN).isNull()) {
			//show message about missing field name, and set focus to that cell
			kexipluginsdbg << "no field provided!" << endl;
			d->dataTable->dataAwareObject()->setCursorPosition(i,0);
			if (errMsg)
				*errMsg = i18n("Select column for table \"%1\"")
					.arg(it.current()->at(COLUMN_ID_TABLE).toString());
			return false;
		}

		KoProperty::Set *set = d->sets->at(i);
		if (set) {
			QString tableName = (*set)["table"].value().toString().stripWhiteSpace();
			QString fieldName = (*set)["field"].value().toString();
			bool fieldVisible = (*set)["visible"].value().toBool();
			QString criteriaStr = (*set)["criteria"].value().toString();
			QCString alias = (*set)["alias"].value().toCString();
			if (!criteriaStr.isEmpty()) {
				int token;
				KexiDB::BaseExpr *criteriaExpr = parseExpressionString(criteriaStr, token, true/*allowRelationalOperator*/);
				if (!criteriaExpr) {//for sanity
					if (errMsg)
						*errMsg = i18n("Invalid criteria \"%1\"").arg(criteriaStr);
					delete whereExpr;
					return false;
				}
				//build relational expression for column variable
        KexiDB::VariableExpr *varExpr = new KexiDB::VariableExpr(fieldName);
				criteriaExpr = new KexiDB::BinaryExpr(KexiDBExpr_Relational, varExpr, token, criteriaExpr);
				//critera ok: add it to WHERE section
				if (whereExpr)
					whereExpr = new KexiDB::BinaryExpr(KexiDBExpr_Logical, whereExpr, AND, criteriaExpr);
				else //first expr.
					whereExpr =	criteriaExpr;
			}
			if (tableName.isEmpty()) {
				if ((*set)["isExpression"].value().toBool()==true) {
					//add expresion column
					int dummyToken;
					KexiDB::BaseExpr *columnExpr = parseExpressionString(fieldName, dummyToken, false/*!allowRelationalOperator*/);
					if (!columnExpr) {
						if (errMsg)
							*errMsg = i18n("Invalid expression \"%1\"").arg(fieldName);
						return false;
					}
					KexiDB::Field *f = new KexiDB::Field(temp->query, columnExpr);
					temp->query->addField(f, fieldVisible);
					if (fieldVisible)
						fieldsFound = true;
					if (!alias.isEmpty())
						temp->query->setColumnAlias( temp->query->fieldCount()-1, alias );
				}
				//TODO
			}
			else if (tableName=="*") {
				//all tables asterisk
				temp->query->addAsterisk( new KexiDB::QueryAsterisk( temp->query, 0 ), fieldVisible );
				if (fieldVisible)
					fieldsFound = true;
				continue;
			}
			else {
				KexiDB::TableSchema *t = d->conn->tableSchema(tableName);
//				if (fieldName.find(".*")!=-1) {
				if (fieldName=="*") {
					//single-table asterisk: <tablename> + ".*" + number
					temp->query->addAsterisk( new KexiDB::QueryAsterisk( temp->query, t ), fieldVisible );
					if (fieldVisible)
						fieldsFound = true;
				} else {
					if (!t) {
						kexipluginswarn << "query designer: NO TABLE '" << (*set)["table"].value().toString() << "'" << endl;
						continue;
					}
					KexiDB::Field *f = t->field( fieldName );
					if (!f) {
						kexipluginswarn << "query designer: NO FIELD '" << fieldName << "'" << endl;
						continue;
					}
					temp->query->addField(f, fieldVisible);
					if (fieldVisible)
						fieldsFound = true;
					if (!alias.isEmpty())
						temp->query->setColumnAlias( temp->query->fieldCount()-1, alias );
				}
			}
		}
		else {//!set
			kexipluginsdbg << it.current()->at(COLUMN_ID_TABLE).toString() << endl;
		}
	}
	if (!fieldsFound) {
		if (errMsg)
			*errMsg = msgCannotSwitch_EmptyDesign();
		return false;
	}
	if (whereExpr)
		kexipluginsdbg << "KexiQueryDesignerGuiEditor::buildSchema(): setting CRITERIA: " << whereExpr->debugString() << endl;

	//set always, because if whereExpr==NULL,
	//this will clear prev. expr
	temp->query->setWhereExpression( whereExpr );

	//add relations (looking for connections)
	for (ConnectionListIterator it(*d->relations->connections()); it.current(); ++it) {
		KexiRelationViewTableContainer *masterTable = it.current()->masterTable();
		KexiRelationViewTableContainer *detailsTable = it.current()->detailsTable();

/*! @todo what about query? */
		temp->query->addRelationship(
			masterTable->schema()->table()->field(it.current()->masterField()),
			detailsTable->schema()->table()->field(it.current()->detailsField()) );
	}

	temp->query->debug();
	temp->registerTableSchemaChanges(temp->query);
	//TODO?
	return true;
}

tristate
KexiQueryDesignerGuiEditor::beforeSwitchTo(int mode, bool &dontStore)
{
	kexipluginsdbg << "KexiQueryDesignerGuiEditor::beforeSwitch()" << mode << endl;

	if (!d->dataTable->dataAwareObject()->acceptRowEdit())
		return cancelled;

	if (mode==Kexi::DesignViewMode) {
		return true;
	}
	else if (mode==Kexi::DataViewMode) {
//		if (!d->dataTable->dataAwareObject()->acceptRowEdit())
	//		return cancelled;

		if (!dirty() && parentDialog()->neverSaved()) {
			KMessageBox::information(this, msgCannotSwitch_EmptyDesign());
			return cancelled;
		}
		if (dirty() || !tempData()->query) {
			//remember current design in a temporary structure
			dontStore=true;
			QString errMsg;
			//build schema; problems are not allowed
			if (!buildSchema(&errMsg)) {
				KMessageBox::information(this, errMsg);
				return cancelled;
			}
		}
		//TODO
		return true;
	}
	else if (mode==Kexi::TextViewMode) {
		dontStore=true;
		//build schema; ignore problems
		buildSchema();
/*		if (tempData()->query && tempData()->query->fieldCount()==0) {
			//no fields selected: let's add "*" (all-tables asterisk),
			// otherwise SQL statement will be invalid
			tempData()->query->addAsterisk( new KexiDB::QueryAsterisk( tempData()->query ) );
		}*/
		//todo
		return true;
	}

	return false;
}

tristate
KexiQueryDesignerGuiEditor::afterSwitchFrom(int mode)
{
	if (mode==Kexi::NoViewMode || (mode==Kexi::DataViewMode && !tempData()->query)) {
		//this is not a SWITCH but a fresh opening in this view mode
		if (!m_dialog->neverSaved()) {
			if (!loadLayout()) {
				//err msg
				parentDialog()->setStatus(parentDialog()->mainWin()->project()->dbConnection(), i18n("Query definition loading failed."), i18n("Query data may be corrupted."));
				return false;
			}
			showFieldsForQuery( static_cast<KexiDB::QuerySchema *>(parentDialog()->schemaData()) );
			//todo: load global query properties
		}
	}
	else if (mode==Kexi::TextViewMode) {
		if (tempData()->queryChangedInPreviousView) {
			//previous view changed query data
			//-clear and regenerate GUI items
			d->relations->clear();
			initTableRows();
			//todo
			if (tempData()->query) {
				//there is a query schema to show
				showTablesAndConnectionsForQuery( tempData()->query );
				//-show fields
				showFieldsForQuery( tempData()->query );
			}
		}
		//todo: load global query properties
	}
	else if (mode==Kexi::DataViewMode) {
		//this is just a SWITCH from data view
		//set cursor if needed:
		if (d->dataTable->dataAwareObject()->currentRow()<0
			|| d->dataTable->dataAwareObject()->currentColumn()<0)
		{
			d->dataTable->dataAwareObject()->ensureCellVisible(0,0);
			d->dataTable->dataAwareObject()->setCursorPosition(0,0);
		}
	}
	tempData()->queryChangedInPreviousView = false;
	setFocus(); //to allow shared actions proper update
	return true;
}


KexiDB::SchemaData*
KexiQueryDesignerGuiEditor::storeNewData(const KexiDB::SchemaData& sdata, bool &/*cancel*/)
{
	buildSchema();
	KexiQueryPart::TempData * temp = tempData();
	(KexiDB::SchemaData&)*temp->query = sdata; //copy main attributes

	bool ok = m_mainWin->project()->dbConnection()->storeObjectSchemaData( *temp->query, true /*newObject*/ );
	m_dialog->setId( temp->query->id() );

	if (ok)
		ok = storeLayout();

	KexiDB::QuerySchema *query = temp->query;
	temp->query = 0; //will be returned, so: don't keep it
	if (!ok) {
		delete query;
		return 0;
	}
	return query;
}

tristate KexiQueryDesignerGuiEditor::storeData()
{
	tristate res = KexiViewBase::storeData();
	if (~res)
		return res;
	if (res) {
		buildSchema();
		res = storeLayout();
	}
	return res;
}

void KexiQueryDesignerGuiEditor::showTablesAndConnectionsForQuery(KexiDB::QuerySchema *query)
{
	d->relations->clear();
	d->slotTableAdded_enabled = false; //speedup

	//-show tables:
	//(todo: instead of hiding all tables and showing some tables,
	// show only these new and hide these unncecessary; the same for connections)
	for (KexiDB::TableSchema::ListIterator it(*query->tables()); it.current(); ++it) {
		d->relations->addTable( it.current() );
	}
	//-show relationships:
	KexiDB::Relationship *rel;
	for (KexiDB::Relationship::ListIterator it(*query->relationships()); (rel=it.current()); ++it) {
		SourceConnection conn;
//@todo: now only sigle-field relationships are implemented!
		KexiDB::Field *masterField = rel->masterIndex()->fields()->first();
		KexiDB::Field *detailsField = rel->detailsIndex()->fields()->first();
		conn.masterTable = masterField->table()->name(); //<<<TODO
		conn.masterField = masterField->name();
		conn.detailsTable = detailsField->table()->name();
		conn.detailsField = detailsField->name();
		d->relations->addConnection( conn );
	}

	d->slotTableAdded_enabled = true;
	updateColumnsData();
}

void KexiQueryDesignerGuiEditor::showFieldsForQuery(KexiDB::QuerySchema *query)
{
	const bool was_dirty = dirty();

	QDict<KexiDB::BinaryExpr> criterias(101, false);
	//collect information about criterias:
	//--this must be top level chain of AND's
	KexiDB::BaseExpr* e = query->whereExpression();
	KexiDB::BaseExpr* eItem = 0;
	while (e) {
		if (e->toBinary() && e->toBinary()->token()==AND) {
			eItem = e->toBinary()->left();
			e = e->toBinary()->right();
		}
		else {
			eItem = e;
			e = 0;
		}
		kexidbg << eItem->toString() << endl;
		if (eItem->toBinary() && eItem->exprClass()==KexiDBExpr_Relational
			&& eItem->toBinary()->left()->toVariable())
		{
			//this is: variable , op , argument
			//store:
			criterias.insert(eItem->toBinary()->left()->toVariable()->name, eItem->toBinary());
		}
	}

	//add fields
	uint row_num = 0;
	KexiDB::Field *field;
	for (KexiDB::Field::ListIterator it(*query->fields()); (field = it.current()); ++it, row_num++) {
		//add row
		QString tableName, fieldName, columnAlias, criteriaString;
		KexiDB::BinaryExpr *criteriaExpr = 0;
		if (field->isQueryAsterisk()) {
			if (field->table()) {//single-table asterisk
				tableName = field->table()->name();
				fieldName = "*";
			}
			else {//all-tables asterisk
				tableName = "*";
				fieldName = "";
			}
		}
		else {
			columnAlias = query->columnAlias(row_num);
			if (field->isExpression()) {
//				if (columnAlias.isEmpty()) {
//					columnAlias = i18n("expression", "expr%1").arg(row_num); //TODO
//				}
//				if (columnAlias.isEmpty())
//TODO: ok? perhaps do not allow to omit aliases?
					fieldName = field->expression()->toString();
//				else
//					fieldName = columnAlias + ": " + field->expression()->toString();
			}
			else {
				tableName = field->table()->name();
				fieldName = field->name();
				criteriaExpr = criterias[fieldName];
			}
		}
		KexiTableItem *newItem = createNewRow(tableName, fieldName); //, columnAlias);
		if (criteriaExpr) {
//TODO: fix for !INFIX operators
			if (criteriaExpr->token()=='=')
				criteriaString = criteriaExpr->right()->toString();
			else
				criteriaString = criteriaExpr->tokenToString() + " " + criteriaExpr->right()->toString();
			(*newItem)[COLUMN_ID_CRITERIA] = criteriaString;
		}
		d->dataTable->dataAwareObject()->insertItem(newItem, row_num);
		//create a new set
		KoProperty::Set &set = *createPropertySet( row_num, tableName, fieldName, true/*new one*/ );
		if (!columnAlias.isEmpty())
			set["alias"].setValue(columnAlias, false);
		if (!criteriaString.isEmpty())
			set["criteria"].setValue( criteriaString, false );
		if (field->isExpression()) {
			(*newItem)[COLUMN_ID_COLUMN] = criteriaString;
			d->data->clearRowEditBuffer();
			d->data->updateRowEditBuffer(newItem, COLUMN_ID_COLUMN, 
				QVariant(columnAlias + ": " + field->expression()->toString()));
			d->data->saveRowChanges(*newItem, true);
		}
	}
	propertySetSwitched();

	if (!was_dirty)
		setDirty(false);
	d->dataTable->dataAwareObject()->ensureCellVisible(0,0);
//	tempData()->registerTableSchemaChanges(query);
}

bool KexiQueryDesignerGuiEditor::loadLayout()
{
	QString xml;
//	if (!loadDataBlock( xml, "query_layout" )) {
	loadDataBlock( xml, "query_layout" );
		//TODO errmsg
//		return false;
//	}
	if (xml.isEmpty()) {
		//in a case when query layout was not saved, build layout by hand
		showTablesAndConnectionsForQuery( 
			static_cast<KexiDB::QuerySchema *>(parentDialog()->schemaData()) );
		return true;
	}

	QDomDocument doc;
	doc.setContent(xml);
	QDomElement doc_el = doc.documentElement(), el;
	if (doc_el.tagName()!="query_layout") {
		//TODO errmsg
		return false;
	}

	const bool was_dirty = dirty();

	//add tables and relations to the relation view
	for (el = doc_el.firstChild().toElement(); !el.isNull(); el=el.nextSibling().toElement()) {
		if (el.tagName()=="table") {
			KexiDB::TableSchema *t = d->conn->tableSchema(el.attribute("name"));
			int x = el.attribute("x","-1").toInt();
			int y = el.attribute("y","-1").toInt();
			int width = el.attribute("width","-1").toInt();
			int height = el.attribute("height","-1").toInt();
			QRect rect;
			if (x!=-1 || y!=-1 || width!=-1 || height!=-1)
				rect = QRect(x,y,width,height);
			d->relations->addTable( t, rect );
		}
		else if (el.tagName()=="conn") {
			SourceConnection src_conn;
			src_conn.masterTable = el.attribute("mtable");
			src_conn.masterField = el.attribute("mfield");
			src_conn.detailsTable = el.attribute("dtable");
			src_conn.detailsField = el.attribute("dfield");
			d->relations->addConnection(src_conn);
		}
	}

	if (!was_dirty)
		setDirty(false);
	return true;
}

bool KexiQueryDesignerGuiEditor::storeLayout()
{
	KexiQueryPart::TempData * temp = tempData();

	// Save SQL without driver-escaped keywords.
	QString sqlText = mainWin()->project()->dbConnection()->selectStatement( 
		*temp->query, KexiDB::Driver::EscapeKexi|KexiDB::Driver::EscapeAsNecessary );
	if (!storeDataBlock( sqlText, "sql" )) {
		return false;
	}

	//serialize detailed XML query definition
	QString xml = "<query_layout>", tmp;
	for (TablesDictIterator it(*d->relations->tables()); it.current(); ++it) {
		KexiRelationViewTableContainer *table_cont = it.current();
/*! @todo what about query? */
		tmp = QString("<table name=\"")+QString(table_cont->schema()->name())+"\" x=\""
		 +QString::number(table_cont->x())
		 +"\" y=\""+QString::number(table_cont->y())
		 +"\" width=\""+QString::number(table_cont->width())
		 +"\" height=\""+QString::number(table_cont->height())
		 +"\"/>";
		xml += tmp;
	}

	KexiRelationViewConnection *con; 
	for (ConnectionListIterator it(*d->relations->connections()); (con = it.current()); ++it) {
		tmp = QString("<conn mtable=\"") + QString(con->masterTable()->schema()->name())
			+ "\" mfield=\"" + con->masterField() + "\" dtable=\"" 
			+ QString(con->detailsTable()->schema()->name()) 
			+ "\" dfield=\"" + con->detailsField() + "\"/>";
		xml += tmp;
	}
	xml += "</query_layout>";
	if (!storeDataBlock( xml, "query_layout" )) {
		return false;
	}
	return true;
}

QSize KexiQueryDesignerGuiEditor::sizeHint() const
{
	QSize s1 = d->relations->sizeHint();
	QSize s2 = d->head->sizeHint();
	return QSize(QMAX(s1.width(),s2.width()), s1.height()+s2.height());
}

KexiTableItem*
KexiQueryDesignerGuiEditor::createNewRow(const QString& tableName, const QString& fieldName) const
{
	KexiTableItem *newItem = d->data->createItem(); //new KexiTableItem(d->data->columnsCount());
	QString key;
//	if (!alias.isEmpty())
//		key=alias+": ";
	if (tableName=="*")
		key="*";
	else {
		if (!tableName.isEmpty())
			key = (tableName+".");
		key += fieldName;
	}
	(*newItem)[COLUMN_ID_COLUMN]=key;
	(*newItem)[COLUMN_ID_TABLE]=tableName;
	(*newItem)[COLUMN_ID_VISIBLE]=QVariant(true,1);//visible
	(*newItem)[COLUMN_ID_TOTALS]=QVariant(0);//totals
	return newItem;
}

void KexiQueryDesignerGuiEditor::slotDragOverTableRow(
	KexiTableItem * /*item*/, int /*row*/, QDragMoveEvent* e)
{
	if (e->provides("kexi/field")) {
		e->acceptAction(true);
	}
}

void
KexiQueryDesignerGuiEditor::slotDroppedAtRow(KexiTableItem * /*item*/, int /*row*/,
	QDropEvent *ev, KexiTableItem*& newItem)
{
	QString sourceMimeType;
	QString srcTable;
	QString srcField;

	if (!KexiFieldDrag::decodeSingle(ev,sourceMimeType,srcTable,srcField))
		return;
	//insert new row at specific place
	newItem = createNewRow(srcTable, srcField);
	d->droppedNewItem = newItem;
	d->droppedNewTable = srcTable;
	d->droppedNewField = srcField;
	//TODO
}

void KexiQueryDesignerGuiEditor::slotRowInserted(KexiTableItem* item, uint row, bool /*repaint*/)
{
	if (d->droppedNewItem && d->droppedNewItem==item) {
		createPropertySet( row, d->droppedNewTable, d->droppedNewField, true );
		propertySetSwitched();
		d->droppedNewItem=0;
	}
}

void KexiQueryDesignerGuiEditor::slotTableAdded(KexiDB::TableSchema & /*t*/)
{
	if (!d->slotTableAdded_enabled)
		return;
	updateColumnsData();
	setDirty();
	d->dataTable->setFocus();
}

void KexiQueryDesignerGuiEditor::slotTableHidden(KexiDB::TableSchema & /*t*/)
{
	updateColumnsData();
	setDirty();
}

/*! @internal generates smallest unique alias */
QCString KexiQueryDesignerGuiEditor::generateUniqueAlias() const
{
//TODO: add option for using non-i18n'd "expr" prefix?
	const QCString expStr 
		= i18n("short for 'expression' word (only latin letters, please)", "expr").latin1();
//TODO: optimization: cache it?
	QAsciiDict<char> aliases(101);
	for (int r = 0; r<(int)d->sets->size(); r++) {
		KoProperty::Set *set = d->sets->at(r);
		if (set) {
			const QCString a = (*set)["alias"].value().toCString().lower();
			if (!a.isEmpty())
				aliases.insert(a,(char*)1);
		}
	}
	int aliasNr=1;
	for (;;aliasNr++) {
		if (!aliases[expStr+QString::number(aliasNr).latin1()])
			break;
	}
	return expStr+QString::number(aliasNr).latin1();
}

//! @todo this is primitive, temporary: reuse SQL parser
KexiDB::BaseExpr*
KexiQueryDesignerGuiEditor::parseExpressionString(const QString& fullString, int& token,
 bool allowRelationalOperator)
{
	QString str = fullString.stripWhiteSpace();
	int len = 0;
	KexiDB::BaseExpr *expr = 0;
	//1. get token
	token = 0;
	//2-char-long tokens
	if (str.startsWith(">="))
		token = GREATER_OR_EQUAL;
	else if (str.startsWith("<="))
		token = LESS_OR_EQUAL;
	else if (str.startsWith("<>"))
		token = NOT_EQUAL;
	else if (str.startsWith("!="))
		token = NOT_EQUAL2;
	else if (str.startsWith("=="))
		token = '=';

	if (token!=0)
		len = 2;
	else if (str.startsWith("=") //1-char-long tokens
		|| str.startsWith("<")
		|| str.startsWith(">"))
	{
		token = str[0].latin1();
		len = 1;
	}
	else {
		if (allowRelationalOperator)
			token = '=';
	}

	if (!allowRelationalOperator && token!=0)
		return 0;

	//1. get expression after token
	if (len>0)
		str = str.mid(len).stripWhiteSpace();
	if (str.isEmpty())
		return 0;

	KexiDB::BaseExpr *valueExpr = 0;
	QRegExp re;
	if (str.length()>=2
		&& (
		(str.startsWith("\"") && str.endsWith("\""))
		|| (str.startsWith("'") && str.endsWith("'")))
		)
	{
		valueExpr = new KexiDB::ConstExpr(CHARACTER_STRING_LITERAL, str.mid(1,str.length()-2));
	}
	else if ((re = QRegExp("(\\d{1,4})-(\\d{1,2})-(\\d{1,2})")).exactMatch( str ))
	{
			valueExpr = new KexiDB::ConstExpr(DATE_CONST, QDate::fromString(
				re.cap(1).rightJustify(4, '0')+"-"+re.cap(2).rightJustify(2, '0')
				+"-"+re.cap(3).rightJustify(2, '0'), Qt::ISODate));
	}
	else if ((re = QRegExp("(\\d{1,2}):(\\d{1,2})")).exactMatch( str )
	      || (re = QRegExp("(\\d{1,2}):(\\d{1,2}):(\\d{1,2})")).exactMatch( str ))
	{
		QString res = re.cap(1).rightJustify(2, '0')+":"+re.cap(2).rightJustify(2, '0')
			+":"+re.cap(3).rightJustify(2, '0');
//		kexipluginsdbg << res << endl;
		valueExpr = new KexiDB::ConstExpr(TIME_CONST, QTime::fromString(res, Qt::ISODate));
	}
	else if ((re = QRegExp("(\\d{1,4})-(\\d{1,2})-(\\d{1,2})\\s+(\\d{1,2}):(\\d{1,2})")).exactMatch( str )
	      || (re = QRegExp("(\\d{1,4})-(\\d{1,2})-(\\d{1,2})\\s+(\\d{1,2}):(\\d{1,2}):(\\d{1,2})")).exactMatch( str ))
	{
		QString res = re.cap(1).rightJustify(4, '0')+"-"+re.cap(2).rightJustify(2, '0')
			+"-"+re.cap(3).rightJustify(2, '0')
			+"T"+re.cap(4).rightJustify(2, '0')+":"+re.cap(5).rightJustify(2, '0')
			+":"+re.cap(6).rightJustify(2, '0');
//		kexipluginsdbg << res << endl;
		valueExpr = new KexiDB::ConstExpr(DATETIME_CONST,
			QDateTime::fromString(res, Qt::ISODate));
	}
	else if (str[0]>='0' && str[0]<='9' || str[0]=='-' || str[0]=='+') {
		//number
		QString decimalSym = KGlobal::locale()->decimalSymbol();
		bool ok;
		int pos = str.find('.');
		if (pos==-1) {//second chance: local decimal symbol
			pos = str.find(decimalSym);
		}
		if (pos>=0) {//real const number
			const int left = str.left(pos).toInt(&ok);
			if (!ok)
				return 0;
			const int right = str.mid(pos+1).toInt(&ok);
			if (!ok)
				return 0;
			valueExpr = new KexiDB::ConstExpr(REAL_CONST, QPoint(left,right)); //decoded to QPoint
		}
		else {
			//integer const
			const Q_LLONG val = str.toLongLong(&ok);
			if (!ok)
				return 0;
			valueExpr = new KexiDB::ConstExpr(INTEGER_CONST, val);
		}
	}
	else if (str.lower()=="null") {
		valueExpr = new KexiDB::ConstExpr(SQL_NULL, QVariant());
	}
	else {//identfier
		if (!KexiUtils::isIdentifier(str))
			return 0;
		valueExpr = new KexiDB::VariableExpr(str);
		//find first matching field for name 'str':
		for (TablesDictIterator it(*d->relations->tables()); it.current(); ++it) {
/*! @todo what about query? */
			if (it.current()->schema()->table() && it.current()->schema()->table()->field(str)) {
				valueExpr->toVariable()->field = it.current()->schema()->table()->field(str);
				break;
			}
		}
	}
	return valueExpr;
}

void KexiQueryDesignerGuiEditor::slotBeforeCellChanged(KexiTableItem *item, int colnum,
	QVariant& newValue, KexiDB::ResultInfo* result)
{
	if (colnum == COLUMN_ID_COLUMN) {
		if (newValue.isNull()) {
			d->data->updateRowEditBuffer(item, COLUMN_ID_TABLE, QVariant(), false/*!allowSignals*/);
			d->data->updateRowEditBuffer(item, COLUMN_ID_VISIBLE, QVariant(false,1));//invisible
			d->data->updateRowEditBuffer(item, COLUMN_ID_TOTALS, QVariant());//remove totals
			d->data->updateRowEditBuffer(item, COLUMN_ID_CRITERIA, QVariant());//remove crit.
			d->sets->removeCurrentPropertySet();
		}
		else {
			//auto fill 'table' column
			QString fieldId = newValue.toString().stripWhiteSpace(); //tmp, can look like "table.field"
			QString fieldName; //"field" part of "table.field" or expression string
			QString tableName; //empty for expressions
			QCString alias;
			QString columnValueForExpr; //for setting pretty printed "alias: expr" in 1st column
			const bool isExpression = !d->fieldColumnIdentifiers[fieldId];
			if (isExpression) {
				//this value is entered by hand and doesn't match
				//any value in the combo box -- we're assuming this is an expression
				//-table remains null
				//-find "alias" in something like "alias : expr"
				const int id = fieldId.find(':');
				if (id>0) {
					alias = fieldId.left(id).stripWhiteSpace().latin1();
					if (!KexiUtils::isIdentifier(alias)) {
						result->success = false;
						result->column = 0;
						result->msg = i18n("Entered column alias \"%1\" is not a valid identifier.")
							.arg(alias);
						result->desc = i18n("Identifiers should start with a letter or '_' character");
						return;
					}
				}
				fieldName = fieldId.mid(id+1).stripWhiteSpace();
				//check expr.
				KexiDB::BaseExpr *e;
				int dummyToken;
				if ((e = parseExpressionString(fieldName, dummyToken, false/*allowRelationalOperator*/)))
				{
					fieldName = e->toString(); //print it prettier
					//this is just checking: destroy expr. object
					delete e;
				}
				else {
					result->success = false;
					result->column = 0;
					result->msg = i18n("Invalid expression \"%1\"").arg(fieldName);
					return;
				}
			}
			else {//not expr.
				//this value is properly selected from combo box list
				if (fieldId=="*") {
					tableName = "*";
				}
				else {
					const int id = fieldId.find('.');
					if (id>=0)
						tableName = fieldId.left(id);
					fieldName = fieldId.mid(id+1);
				}
			}
			bool saveOldValue = true;
			KoProperty::Set *set = d->sets->listForItem(*item); //*propertyBuffer();
			if (!set) {
				saveOldValue = false; // no old val.
				const int row = d->data->findRef(item);
				if (row<0) {
					result->success = false;
					return;
				}
				set = createPropertySet( row, tableName, fieldName, true );
				propertySetSwitched();
			}
			d->data->updateRowEditBuffer(item, COLUMN_ID_TABLE, QVariant(tableName), false/*!allowSignals*/);
			d->data->updateRowEditBuffer(item, COLUMN_ID_VISIBLE, QVariant(true,1));//visible
			d->data->updateRowEditBuffer(item, COLUMN_ID_TOTALS, QVariant(0));//totals
			//update properties
			(*set)["field"].setValue(fieldName, saveOldValue);
			if (isExpression) {
				//-no alias but it's needed:
				if (alias.isEmpty()) //-try oto get old alias
					alias = (*set)["alias"].value().toCString();
				if (alias.isEmpty()) //-generate smallest unique alias
					alias = generateUniqueAlias();
			}
			(*set)["isExpression"].setValue(QVariant(isExpression,1), saveOldValue);
			if (!alias.isEmpty()) {
				(*set)["alias"].setValue(alias, saveOldValue);
				//pretty printed "alias: expr"
				newValue = QString(alias) + ": " + fieldName;
			}
			(*set)["caption"].setValue(QString::null, saveOldValue);
			(*set)["table"].setValue(tableName, saveOldValue);
			updatePropertiesVisibility(*set);
		}
	}
	else if (colnum==COLUMN_ID_TABLE) {
		if (newValue.isNull()) {
			if (!item->at(COLUMN_ID_COLUMN).toString().isEmpty())
				d->data->updateRowEditBuffer(item, COLUMN_ID_COLUMN, QVariant(), false/*!allowSignals*/);
			d->data->updateRowEditBuffer(item, COLUMN_ID_VISIBLE, QVariant(false,1));//invisible
			d->data->updateRowEditBuffer(item, COLUMN_ID_TOTALS, QVariant());//remove totals
			d->data->updateRowEditBuffer(item, COLUMN_ID_CRITERIA, QVariant());//remove crit.
			d->sets->removeCurrentPropertySet();
		}
		//update property
		KoProperty::Set *set = d->sets->listForItem(*item);
		if (set) {
			if ((*set)["isExpression"].value().toBool()==false) {
				(*set)["table"] = newValue;
				(*set)["caption"] = QString::null;
			}
			else {
				//do not set table for expr. columns
				newValue = QVariant();
			}
//			KoProperty::Set &set = *propertyBuffer();
			updatePropertiesVisibility(*set);
		}
	}
	else if (colnum==COLUMN_ID_VISIBLE) {
		bool saveOldValue = true;
		if (!propertySet()) {
			saveOldValue = false;
			createPropertySet( d->dataTable->dataAwareObject()->currentRow(),
				item->at(COLUMN_ID_TABLE).toString(), item->at(COLUMN_ID_COLUMN).toString(), true );
			d->data->updateRowEditBuffer(item, COLUMN_ID_TOTALS, QVariant(0));//totals
			propertySetSwitched();
		}
		KoProperty::Set &set = *propertySet();
		set["visible"].setValue(newValue, saveOldValue);
	}
	else if (colnum==COLUMN_ID_TOTALS) {
		//TODO:
		//unused yet
		setDirty(true);
	}
	else if (colnum==COLUMN_ID_CRITERIA) {//'criteria'
//TODO: this is primitive, temporary: reuse SQL parser
		QString operatorStr, argStr;
		KexiDB::BaseExpr* e = 0;
		const QString str = newValue.toString().stripWhiteSpace();
//		KoProperty::Set &set = *propertySet();
		int token;
		QString field, table;
		KoProperty::Set *set = d->sets->listForItem(*item); //*propertySet();
		if (set) {
			field = (*set)["field"].value().toString();
			table = (*set)["table"].value().toString();
		}
		if (!str.isEmpty() && (!set || table=="*" || field.find("*")!=-1)) {
			//asterisk found! criteria not allowed
			result->success = false;
			result->column = 4;
			if (propertySet())
				result->msg = i18n("Could not set criteria for \"%1\"")
					.arg(table=="*" ? table : field);
			else
				result->msg = i18n("Could not set criteria for empty row");
			d->dataTable->dataAwareObject()->cancelEditor(); //prevents further editing of this cell
		}
		else if (str.isEmpty() || (e = parseExpressionString(str, token, true/*allowRelationalOperator*/)))
		{
			if (e) {
				QString tokenStr;
				if (token!='=') {
					KexiDB::BinaryExpr be(KexiDBExpr_Relational, 0, token, 0);
					tokenStr = be.tokenToString() + " ";
				}
				(*set)["criteria"] = tokenStr + e->toString(); //print it prettier
				//this is just checking: destroy expr. object
				delete e;
			}
			else if (str.isEmpty()) {
				(*set)["criteria"] = QVariant(); //clear it
			}
			setDirty(true);
		}
		else {
			result->success = false;
			result->column = 4;
			result->msg = i18n("Invalid criteria \"%1\"").arg(newValue.toString());
		}
	}
}

void KexiQueryDesignerGuiEditor::slotTablePositionChanged(KexiRelationViewTableContainer*)
{
	setDirty(true);
}

void KexiQueryDesignerGuiEditor::slotAboutConnectionRemove(KexiRelationViewConnection*)
{
	setDirty(true);
}

void KexiQueryDesignerGuiEditor::slotTableFieldDoubleClicked( 
	KexiDB::TableSchema* table, const QString& fieldName )
{
	if (!table || (!table->field(fieldName) && fieldName!="*"))
		return;
	int row_num;
	//find last filled row in the GUI table
	for (row_num=d->sets->size()-1; !d->sets->at(row_num) && row_num>=0; row_num--)
		;
	row_num++; //after
	//add row
	KexiTableItem *newItem = createNewRow(table->name(), fieldName);
	d->dataTable->dataAwareObject()->insertItem(newItem, row_num);
	d->dataTable->dataAwareObject()->setCursorPosition(row_num, 0);
	//create buffer
	createPropertySet( row_num, table->name(), fieldName, true/*new one*/ );
	propertySetSwitched();
	d->dataTable->setFocus();
}

KoProperty::Set *KexiQueryDesignerGuiEditor::propertySet()
{
	return d->sets->currentPropertySet();
}

static bool isAsterisk(const QString& tableName, const QString& fieldName)
{
	return tableName=="*" || fieldName.endsWith("*");
}

void KexiQueryDesignerGuiEditor::updatePropertiesVisibility(KoProperty::Set& set)
{
	const bool asterisk = isAsterisk(
		set["table"].value().toString(), set["field"].value().toString()
	);
#ifndef KEXI_NO_UNFINISHED
	set["caption"].setVisible( !asterisk );
#endif
	set["alias"].setVisible( !asterisk );
	set["sorting"].setVisible( !asterisk );
	propertySetReloaded(true);
}

KoProperty::Set*
KexiQueryDesignerGuiEditor::createPropertySet( int row, 
	const QString& tableName, const QString& fieldName, bool newOne )
{
	const bool asterisk = isAsterisk(tableName, fieldName);
	QString typeName = "KexiQueryDesignerGuiEditor::Column";
	KoProperty::Set *set = new KoProperty::Set(d->sets, typeName);
	KoProperty::Property *prop;

	//meta-info for property editor
	set->addProperty(prop = new KoProperty::Property("this:classString", i18n("Query column")) );
	prop->setVisible(false);
//! \todo add table_field icon (add	buff->addProperty(prop = new KexiProperty("this:iconName", "table_field") );
//	prop->setVisible(false);

	set->addProperty(prop = new KoProperty::Property("table", QVariant(tableName)) );
	prop->setVisible(false);//always hidden

	set->addProperty(prop = new KoProperty::Property("field", QVariant(fieldName)) );
	prop->setVisible(false);//always hidden

	set->addProperty(prop = new KoProperty::Property("caption", QVariant(QString::null), i18n("Caption") ) );
#ifdef KEXI_NO_UNFINISHED
		prop->setVisible(false);
#endif

	set->addProperty(prop = new KoProperty::Property("alias", QVariant(QString::null), i18n("Alias")) );

	set->addProperty(prop = new KoProperty::Property("visible", QVariant(true, 4)) );
	prop->setVisible(false);

/*TODO: 
	set->addProperty(prop = new KexiProperty("totals", QVariant(QString::null)) );
	prop->setVisible(false);*/

	//sorting
	QStringList slist, nlist;
	slist << "nosorting" << "ascending" << "descending";
	nlist << i18n("None") << i18n("Ascending") << i18n("Descending");
	set->addProperty(prop = new KoProperty::Property("sorting", 
		slist, nlist, slist[0], i18n("Sorting")));

	set->addProperty(prop = new KoProperty::Property("criteria", QVariant(QString::null)) );
	prop->setVisible(false);

	set->addProperty(prop = new KoProperty::Property("isExpression", QVariant(false, 1)) );
	prop->setVisible(false);

	connect(set, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
		this, SLOT(slotPropertyChanged(KoProperty::Set&, KoProperty::Property&)));

	d->sets->insert(row, set, newOne);

	updatePropertiesVisibility(*set);
	return set;
}

void KexiQueryDesignerGuiEditor::setFocus()
{
	d->dataTable->setFocus();
}

void KexiQueryDesignerGuiEditor::slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property)
{
	const QCString& pname = property.name();
/*
 * TODO (js) use KexiProperty::setValidator(QString) when implemented as described in TODO #60
 */
	if (pname=="alias" || pname=="name") {
		const QVariant& v = property.value();
		if (!v.toString().stripWhiteSpace().isEmpty() && !KexiUtils::isIdentifier( v.toString() )) {
			KMessageBox::sorry(this,
				KexiUtils::identifierExpectedMessage(property.caption(), v.toString()));
			property.resetValue();
		}
		if (pname=="alias") {
			if (set["isExpression"].value().toBool()==true) {
				//update value in column #1
				d->dataTable->dataAwareObject()->acceptEditor();
//				d->dataTable->dataAwareObject()->setCursorPosition(d->dataTable->dataAwareObject()->currentRow(),0);
				//d->dataTable->dataAwareObject()->startEditCurrentCell();
				d->data->updateRowEditBuffer(d->dataTable->dataAwareObject()->selectedItem(), 
					0, QVariant(set["alias"].value().toString() + ": " + set["field"].value().toString()));
				d->data->saveRowChanges(*d->dataTable->dataAwareObject()->selectedItem(), true);
//				d->dataTable->dataAwareObject()->acceptRowEdit();
			}
		}
	}
}

void KexiQueryDesignerGuiEditor::slotNewItemStored(KexiPart::Item& item)
{
	d->relations->objectCreated(item.mime(), item.name().latin1());
}

void KexiQueryDesignerGuiEditor::slotItemRemoved(const KexiPart::Item& item)
{
	d->relations->objectDeleted(item.mime(), item.name().latin1());
}

void KexiQueryDesignerGuiEditor::slotItemRenamed(const KexiPart::Item& item, const QCString& oldName)
{
	d->relations->objectRenamed(item.mime(), oldName, item.name().latin1());
}

#include "kexiquerydesignerguieditor.moc"

