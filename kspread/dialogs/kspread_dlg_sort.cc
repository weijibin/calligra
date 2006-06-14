/* This file is part of the KDE project
   Copyright (C) 2006 Robert Knight <robertknight@gmail.com>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Werner Trobin <trobin@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2000 David Faure <faure@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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


#include <q3buttongroup.h>

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QRect>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kvbox.h>
#include <kinstance.h>

#include "kspread_dlg_sort.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_sheet.h"
#include "kspread_view.h"
#include "kspread_util.h"
#include "selection.h"

using namespace KSpread;

SortDialog::SortDialog( View * parent,  const char * name,
                                bool modal )
  : KDialog( parent ),
    m_pView( parent )
{
  setCaption( i18n("Sort") );
  setObjectName( name );
  setModal( modal );
  setButtons( Ok|Cancel );

  if ( !name )
    setObjectName( "SortDialog" );

  resize( 528, 316 );
  setWindowTitle( i18n( "Sorting" ) );
  //setSizeGripEnabled( true );

  KVBox *page = new KVBox();
  setMainWidget( page );

  m_tabWidget = new QTabWidget( page );

  m_page1 = new QWidget( m_tabWidget );
  QGridLayout * page1Layout = new QGridLayout( m_page1 );
  page1Layout->setMargin(KDialog::marginHint());
  page1Layout->setSpacing(KDialog::spacingHint());

//---------------- Sort Layout & Header Row/Column Toggle

  //Sort orientation selector (for selecting Left-To-Right or Top-To-Bottom sorting of the selection)
  QGroupBox* layoutGroup = new QGroupBox(/*2 , Qt::Vertical,*/  m_page1, "layoutGroup");
  layoutGroup->setTitle( i18n("Layout") );

  Q3HButtonGroup * orientationGroup = new Q3HButtonGroup( layoutGroup, "orientationGroup" );
  //orientationGroup->setLineWidth(0);
  //orientationGroup->setMargin(0);
  orientationGroup->layout()->setMargin(0);

  m_sortColumn = new QRadioButton( orientationGroup );
  m_sortColumn->setText( i18n( "Sort &Rows" ) );

  m_sortRow = new QRadioButton( orientationGroup );
  m_sortRow->setText( i18n( "Sort &Columns" ) );

  //First row / column contains header toggle
  m_firstRowOrColHeader = new QCheckBox( layoutGroup );
  //m_firstRowOrColHeader->setText( i18n( "&First row contains headers" ) );
  m_firstRowOrColHeader->setChecked(true);
  page1Layout->addWidget(layoutGroup,0,0);

//----------------

  page1Layout->addItem(new QSpacerItem( 0, 10 ), 2, 0 );


  QGroupBox * sort1Box = new QGroupBox( m_page1 );
  sort1Box->setTitle( i18n( "Sort By" ) );
  sort1Box->setFlat(true);
  sort1Box->layout()->setSpacing( KDialog::spacingHint() );
  sort1Box->layout()->setMargin( KDialog::marginHint() );
  QHBoxLayout * sort1BoxLayout = new QHBoxLayout( sort1Box );
  sort1BoxLayout->setAlignment( Qt::AlignTop );

  m_sortKey1 = new QComboBox( sort1Box );
  sort1BoxLayout->addWidget( m_sortKey1 );

  m_sortOrder1 = new QComboBox( sort1Box );
  m_sortOrder1->insertItem( 0,i18n( "Ascending" ) );
  m_sortOrder1->insertItem( 1,i18n( "Descending" ) );
  sort1BoxLayout->addWidget( m_sortOrder1 );

  page1Layout->addWidget( sort1Box, 3, 0 );

  QGroupBox * sort2Box = new QGroupBox( m_page1 );
  sort2Box->setTitle( i18n( "Then By" ) );
  sort2Box->setFlat(true);
  sort2Box->layout()->setSpacing( KDialog::spacingHint() );
  sort2Box->layout()->setMargin( KDialog::marginHint() );
  QHBoxLayout * sort2BoxLayout = new QHBoxLayout( sort2Box );
  sort2BoxLayout->setAlignment( Qt::AlignTop );

  m_sortKey2 = new QComboBox( sort2Box );
  m_sortKey2->insertItem( 0,i18n( "None" ) );
  sort2BoxLayout->addWidget( m_sortKey2 );

  m_sortOrder2 = new QComboBox( sort2Box );
  m_sortOrder2->insertItem( 0,i18n( "Ascending" ) );
  m_sortOrder2->insertItem( 1,i18n( "Descending" ) );
  sort2BoxLayout->addWidget( m_sortOrder2 );

  page1Layout->addWidget( sort2Box, 4, 0 );

  QGroupBox * sort3Box = new QGroupBox( m_page1 );
  sort3Box->setTitle( i18n( "Then By" ) );
  sort3Box->setFlat(true);
  sort3Box->layout()->setSpacing( KDialog::spacingHint() );
  sort3Box->layout()->setMargin( KDialog::marginHint() );
  QHBoxLayout * sort3BoxLayout = new QHBoxLayout( sort3Box );
  sort3BoxLayout->setAlignment( Qt::AlignTop );

  m_sortKey3 = new QComboBox( sort3Box );
  m_sortKey3->insertItem( 0,i18n( "None" ) );
  m_sortKey3->setEnabled( false );
  sort3BoxLayout->addWidget( m_sortKey3 );

  m_sortOrder3 = new QComboBox( sort3Box );
  m_sortOrder3->insertItem( 0,i18n( "Ascending" ) );
  m_sortOrder3->insertItem( 1,i18n( "Descending" ) );
  m_sortOrder3->setEnabled( false );
  sort3BoxLayout->addWidget( m_sortOrder3 );

  page1Layout->addWidget( sort3Box, 5, 0 );
  m_tabWidget->insertTab( 0, m_page1, i18n( "Sort Criteria" ) );


  //---------------- options page

  m_page2 = new QWidget( m_tabWidget );
  QGridLayout * page2Layout = new QGridLayout( m_page2 );
  page2Layout->setMargin(KDialog::marginHint());
  page2Layout->setSpacing(KDialog::spacingHint());
  page2Layout->setAlignment(Qt::AlignTop);

  QGroupBox * firstKeyBox = new QGroupBox( m_page2 );
  firstKeyBox->setTitle( i18n( "First Key" ) );
  firstKeyBox->layout()->setSpacing( KDialog::spacingHint() );
  firstKeyBox->layout()->setMargin( KDialog::marginHint() );
  QVBoxLayout * firstKeyBoxLayout = new QVBoxLayout( firstKeyBox );
  firstKeyBoxLayout->setAlignment( Qt::AlignTop );

  m_useCustomLists = new QCheckBox( firstKeyBox );
  m_useCustomLists->setText( i18n( "&Use custom list" ) );
  firstKeyBoxLayout->addWidget( m_useCustomLists );

  m_customList = new QComboBox( firstKeyBox );
  m_customList->setEnabled( false );
  m_customList->setMaximumSize( 230, 30 );
  firstKeyBoxLayout->addWidget( m_customList );

  page2Layout->addWidget( firstKeyBox, 0, 0 );


 /*
  This doesn't work properly, and as a bug report pointed out, it isn't that useful since it is easy
  to just copy and paste the data and then sort the newly pasted data in place.
  -- Robert Knight

  QGroupBox * resultToBox = new QGroupBox( m_page2 );
  resultToBox->setTitle( i18n( "Location to Store Sort Results" ) );
  resultToBox->layout()->setSpacing( KDialog::spacingHint() );
  resultToBox->layout()->setMargin( KDialog::marginHint() );


  QHBoxLayout * resultToBoxLayout = new QHBoxLayout( resultToBox );
  resultToBoxLayout->setAlignment( Qt::AlignTop );

  QLabel * destinationSheet=new QLabel(resultToBox,"destinationSheet");
  destinationSheet->setText("Destination Sheet:");
  resultToBoxLayout->addWidget(destinationSheet);

  m_outputSheet = new QComboBox( resultToBox );
  resultToBoxLayout->addWidget( m_outputSheet );
  QSpacerItem * spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  resultToBoxLayout->addItem( spacer );

  QLabel * startingCellLabel = new QLabel( resultToBox, "destinationCellLabel" );
  startingCellLabel->setText( i18n( "Destination Cell:" ) );
  resultToBoxLayout->addWidget( startingCellLabel );

  m_outputCell = new QLineEdit( resultToBox );
  m_outputCell->setMaximumSize( QSize( 60, 32767 ) );
  resultToBoxLayout->addWidget( m_outputCell );

  page2Layout->addWidget( resultToBox, 1,0 );*/


  m_tabWidget->insertTab( 1, m_page2, i18n( "Options" ) );

  m_copyLayout = new QCheckBox( m_page2 );
  m_copyLayout->setText( i18n( "Copy cell &formatting (Borders, Colours, Text Style)" ) );

  page2Layout->addWidget( m_copyLayout, 1, 0 );

  m_respectCase = new QCheckBox( m_page2 );
  m_respectCase->setText( i18n( "Case sensitive sort" ) );
  m_respectCase->setChecked( true );

  page2Layout->addWidget( m_respectCase, 2,0 );

  connect( m_sortKey2, SIGNAL( activated( int ) ), this,
           SLOT( sortKey2textChanged( int ) ) );
  connect( m_useCustomLists, SIGNAL( stateChanged(int) ), this,
           SLOT( useCustomListsStateChanged(int) ) );
  connect( m_firstRowOrColHeader, SIGNAL( stateChanged(int) ), this,
           SLOT( firstRowHeaderChanged(int) ) );
  connect( orientationGroup, SIGNAL( pressed(int) ), this,
           SLOT( slotOrientationChanged(int) ) );

  init();
}

QRect SortDialog::sourceArea()
{
    return m_pView->selectionInfo()->selection();
}

SortDialog::Orientation SortDialog::guessDataOrientation()
{
    const QRect selection=sourceArea();

    if (selection.width() >= selection.height())
        return SortColumns;
    else
        return SortRows;
}

SortDialog::~SortDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

void SortDialog::init()
{
    QStringList lst;
    lst<<i18n("January");
    lst<<i18n("February");
    lst<<i18n("March");
    lst<<i18n("April");
    lst<<i18n("May");
    lst<<i18n("June");
    lst<<i18n("July");
    lst<<i18n("August");
    lst<<i18n("September");
    lst<<i18n("October");
    lst<<i18n("November");
    lst<<i18n("December");

    lst<<i18n("Monday");
    lst<<i18n("Tuesday");
    lst<<i18n("Wednesday");
    lst<<i18n("Thursday");
    lst<<i18n("Friday");
    lst<<i18n("Saturday");
    lst<<i18n("Sunday");

  KConfig * config = Factory::global()->config();
  config->setGroup( "Parameters" );
  QStringList other = config->readListEntry("Other list");
  QString tmp;
  for ( QStringList::Iterator it = other.begin(); it != other.end(); ++it )
  {
    if((*it) != "\\")
      tmp += (*it) + ", ";
    else if( it != other.begin())
    {
      tmp = tmp.left(tmp.length() - 2);
      lst.append(tmp);
      tmp = "";
    }
  }
  m_customList->insertItems( 0,lst);

  /*QPtrList<Sheet> sheetList = m_pView->doc()->map()->sheetList();
  for (unsigned int c = 0; c < sheetList.count(); ++c)
  {
    Sheet * t = sheetList.at(c);
    if (!t)
      continue;
    m_outputSheet->insertItem( t->sheetName() );
  }
  m_outputSheet->setItemText( m_pView->activeSheet()->sheetName() );*/

  QRect r = sourceArea();
  /*QString cellArea;
  cellArea += Cell::columnName(r.left());
  cellArea += QString::number( r.top() );
  m_outputCell->setText( cellArea );*/

  //If the top-most row or left-most column all contain text items (as opposed to numbers, dates etc.)
  //then the dialog will guess that the top row (or left column) is a header.
  //The user can always change this if we get this wrong.
  bool selectionMayHaveHeader = true;

  // Entire columns selected ?
  if ( util_isColumnSelected(r) )
  {
    m_sortRow->setEnabled(false);
    m_sortColumn->setChecked(true);

    int right = r.right();
    for (int i = r.left(); i <= right; ++i)
    {
	    QString guessName=m_pView->activeSheet()->guessColumnTitle(r,i);
	    QString colName=i18n(" (Column %1)",Cell::columnName(i));

	    if (!guessName.isEmpty())
	    {
	    	m_listColumn += guessName + colName;
	    }
	    else
	    {
		m_listColumn += i18n("Column %1",Cell::columnName(i));

		if ( i == r.left() )
		  selectionMayHaveHeader=false;
            }
    }
     // m_listColumn += i18n("Column %1",Cell::columnName(i));
  }
  // Entire rows selected ?
  else if ( util_isRowSelected(r) )
  {
    m_sortColumn->setEnabled(false);
    m_sortRow->setChecked(true);

    int bottom = r.bottom();
    for (int i = r.top(); i <= bottom; ++i)
    {
	    QString guessName=m_pView->activeSheet()->guessRowTitle(r,i);
	    QString rowName=i18n(" (Row %1)",i);

	    if (!guessName.isEmpty())
	    {
	    	m_listRow += guessName + rowName;
	    }
	    else
	    {
		m_listRow += i18n("Row %1",i);

		if ( i == r.top() )
		  selectionMayHaveHeader=false;
            }
    }
  }
  else
  {
    // Selection is only one row
    if ( r.top() == r.bottom() )
    {
      m_sortColumn->setEnabled(false);
      m_sortRow->setChecked(true);
    }
    // only one column
    else if (r.left() == r.right())
    {
      m_sortRow->setEnabled(false);
      m_sortColumn->setChecked(true);
    }
    else
    {
        if (guessDataOrientation() == SortColumns)
            m_sortRow->setChecked(true);
        else
            m_sortColumn->setChecked(true);
    }

    int right  = r.right();
    int bottom = r.bottom();
    for (int i = r.left(); i <= right; ++i)
    {
	    QString guessName=m_pView->activeSheet()->guessColumnTitle(r,i);
	    QString colName=i18n(" (Column %1)",Cell::columnName(i));

	    if (!guessName.isEmpty())
		    m_listColumn += guessName + colName;
	    else
	    {
		    m_listColumn += i18n("Column %1",Cell::columnName(i));

		    if (i == r.left())
		      selectionMayHaveHeader=false;
            }
    }

    for (int i = r.top(); i <= bottom; ++i)
    {
	    QString guessName=m_pView->activeSheet()->guessRowTitle(r,i);
	    QString rowName=i18n(" (Row %1)",i);

	    if (!guessName.isEmpty())
		    m_listRow += guessName + rowName;
	    else
	    {
		    m_listRow += i18n("Row %1",i);

		    if (i == r.top())
		      selectionMayHaveHeader=false;
            }
    }
  }

  if ( selectionMayHaveHeader )
    m_firstRowOrColHeader->setChecked( true );
  else
    m_firstRowOrColHeader->setChecked( false );

  // Initialize the combo box
  if ( m_sortRow->isChecked() )
  {
    slotOrientationChanged( SortRows );
  }
  else
  {
    slotOrientationChanged( SortColumns );
  }
}

void SortDialog::slotOrientationChanged(int id)
{
  switch( id )
  {
   case SortColumns :
    m_sortKey1->clear();
    m_sortKey2->clear();
    m_sortKey3->clear();
    m_sortKey1->insertItems( 0,m_listColumn);
    m_sortKey2->insertItem( 0,i18n("None") );
    m_sortKey2->insertItems( 1,m_listColumn);
    m_sortKey3->insertItem( 0,i18n("None") );
    m_sortKey3->insertItems( 1,m_listColumn);
    m_firstRowOrColHeader->setText( i18n( "&First row contains headers" ) );
    break;

   case SortRows :
    m_sortKey1->clear();
    m_sortKey2->clear();
    m_sortKey3->clear();
    m_sortKey1->insertItems( 0,m_listRow);
    m_sortKey2->insertItem( 0,i18n("None") );
    m_sortKey2->insertItems( 1,m_listRow);
    m_sortKey3->insertItem( 0,i18n("None") );
    m_sortKey3->insertItems( 1,m_listRow);
    m_firstRowOrColHeader->setText( i18n( "&First column contains headers" ) );

    /*if (m_firstRowOrColHeader->isChecked())
    {
      int k1 = m_sortKey1->currentIndex();
      int k2 = m_sortKey2->currentIndex();
      int k3 = m_sortKey3->currentIndex();
      m_sortKey1->removeItem( 0 );
      m_sortKey2->removeItem( 1 ); // because there is "None" in there
      m_sortKey3->removeItem( 1 );
      if (k1 > 0)
        m_sortKey1->setCurrentItem(--k1);
      else
        m_sortKey1->setCurrentItem( 0 );
      if (k2 > 0)
        m_sortKey2->setCurrentItem(--k2);
      if (k3 > 0)
        m_sortKey3->setCurrentItem(--k3);
    }*/

    break;

   default :
    kDebug(36001) << "Error in signal : pressed(int id)" << endl;
    break;
  }
}

void SortDialog::slotOk()
{
  m_pView->doc()->emitBeginOperation( false );

  Orientation sortOrientation;
  if (m_sortRow->isChecked())
    sortOrientation=SortColumns;
  else
    sortOrientation=SortRows;

  Sheet * sheet = m_pView->activeSheet();
  /*m_pView->doc()->map()->findSheet( m_outputSheet->currentText() );
  if ( !sheet )
  {
    KMessageBox::error( this, i18n("The destination sheet does not exist.") );
    m_outputSheet->setFocus();
    m_tabWidget->setTabEnabled(m_page2, true);
    m_pView->slotUpdateView( m_pView->activeSheet() );
    return;
  }  */

  /*if ( !outputPoint.isValid() || outputPoint.isSheetKnown() )
  {
    KMessageBox::error( this, i18n("The destination cell does not exist.") );
    m_outputCell->setFocus();
    m_tabWidget->setTabEnabled(m_page2, true);
    m_pView->slotUpdateView( m_pView->activeSheet() );
    return;
  }*/
  //outputPoint.setSheet(sheet);

  QRect sortArea = sourceArea();
  Point outputPoint;
    outputPoint.setPos(sortArea.topLeft());
    outputPoint.setSheet(sheet);
  bool hasHeader=m_firstRowOrColHeader->isChecked();

  if ( hasHeader )
  {
    if (sortOrientation == SortColumns)
    {
        sortArea.setLeft( sortArea.left()+1 );
        outputPoint.setColumn( outputPoint.column()+1 );
    }
    else
    {
        sortArea.setTop( sortArea.top()+1 );
        outputPoint.setRow( outputPoint.row()+1 );
    }
  }

  /*if ( sortArea.topLeft() != outputPoint.pos() )
  {
    int h = outputPoint.pos().y() + sortArea.height();
    int w = outputPoint.pos().x() + sortArea.width();

    if ( sortArea.contains(outputPoint.pos())
         || ( w >= sortArea.left() && w <= sortArea.right() )
         || ( h >= sortArea.top()  && h <= sortArea.bottom() ) )
    {
      KMessageBox::error( this, i18n("If the destination and source regions are different, they must not overlap.") );
      m_outputCell->setFocus();
      m_pView->slotUpdateView( m_pView->activeSheet() );
      // TODO: set right tab
      return;
    }
  }*/

  int key1 = 1;
  int key2 = 0;
  int key3 = 0;
  QStringList * firstKey = 0;
  Sheet::SortingOrder order1;
  Sheet::SortingOrder order2;
  Sheet::SortingOrder order3;

  order1 = ( m_sortOrder1->currentIndex() == 0 ? Sheet::Increase
             : Sheet::Decrease );
  order2 = ( m_sortOrder2->currentIndex() == 0 ? Sheet::Increase
             : Sheet::Decrease );
  order3 = ( m_sortOrder3->currentIndex() == 0 ? Sheet::Increase
             : Sheet::Decrease );

  if ( m_sortRow->isChecked() )
  {
    key1 = m_sortKey1->currentIndex() + sortArea.top();
    if (m_sortKey2->currentIndex() > 0)
      key2 = m_sortKey2->currentIndex() + sortArea.top() - 1; // cause there is "None"
    if (m_sortKey3->currentIndex() > 0)
      key3 = m_sortKey3->currentIndex() + sortArea.top() - 1; // cause there is "None"
  }
  else
  {
    key1 = m_sortKey1->currentIndex() + sortArea.left();
    if (m_sortKey2->currentIndex() > 0)
      key2 = m_sortKey2->currentIndex() + sortArea.left() - 1; // cause there is "None"
    if (m_sortKey3->currentIndex() > 0)
      key3 = m_sortKey3->currentIndex() + sortArea.left() - 1; // cause there is "None"

  }
  /*
  if (m_firstRowOrColHeader->isChecked())
    {
      if (key1 >= 0)
        ++key1;
      if (key2 > 0)
        ++key2;
      if (key3 > 0)
        ++key3;
    }*/

  if ( m_useCustomLists->isChecked() )
  {
    firstKey = new QStringList();
    QString list = m_customList->currentText();
    QString tmp;
    int l = list.length();
    for ( int i = 0; i < l; ++i )
    {
      if ( list[i] == ',' )
      {
        firstKey->append( tmp.trimmed() );
        tmp = "";
      }
      else
        tmp += list[i];
    }
  }



  if (key1 == key2)
    key2 = 0;

  if (key1 == key3)
    key3 = 0;

  if (key2 == 0 && key3 > 0)
  {
    key2 = key3;
    key3 = 0;
  }



  if ( m_sortRow->isChecked() )
  {
    m_pView->activeSheet()->sortByRow( sortArea/*sourceArea*/, key1, key2, key3,
                                       order1, order2, order3,
                                       firstKey, m_copyLayout->isChecked(),
                                       false /*m_firstRowOrColHeader->isChecked()*/,
                                       outputPoint, m_respectCase->isChecked() );
  }
  else if (m_sortColumn->isChecked())
  {
    m_pView->activeSheet()->sortByColumn( sortArea /*sourceArea*/, key1, key2, key3,
                                          order1, order2, order3,
                                          firstKey, m_copyLayout->isChecked(),
                                          false/*m_firstRowOrColHeader->isChecked()*/,
                                          outputPoint, m_respectCase->isChecked() );
  }
  else
  {
    kDebug(36001) << "Err in radiobutton" << endl;
  }

  delete firstKey;
  firstKey = 0;

  m_pView->slotUpdateView( m_pView->activeSheet() );
  accept();
}

void SortDialog::sortKey2textChanged( int i )
{
  m_sortKey3->setEnabled( ( i!=0 ) );
  m_sortOrder3->setEnabled( ( i!=0 ) );
}

void SortDialog::useCustomListsStateChanged( int state )
{
  if (state == 0)
    m_customList->setEnabled(false);
  else if (state == 2)
    m_customList->setEnabled(true);
}

void SortDialog::firstRowHeaderChanged( int /*state*/ )
{
 /* if (m_sortColumn->isChecked())
    return;

  if (state == 0) // off
  {
    int k1 = m_sortKey1->currentIndex();
    int k2 = m_sortKey2->currentIndex();
    int k3 = m_sortKey3->currentIndex();
    m_sortKey1->clear();
    m_sortKey2->clear();
    m_sortKey3->clear();
    m_sortKey1->insertItems( 0, m_listRow );
    m_sortKey2->insertItem( i18n("None") );
    m_sortKey2->insertItems( 0, m_listRow );
    m_sortKey3->insertItem( i18n("None") );
    m_sortKey3->insertItems( 0, m_listRow );

    m_sortKey1->setCurrentItem(++k1);
    m_sortKey2->setCurrentItem(++k2);
    m_sortKey3->setCurrentItem(++k3);
  }
  else if (state == 2) // on
  {
    int k1 = m_sortKey1->currentIndex();
    int k2 = m_sortKey2->currentIndex();
    int k3 = m_sortKey3->currentIndex();
    m_sortKey1->removeItem( 0 );
    m_sortKey2->removeItem( 1 ); // because there is "None" in there
    m_sortKey3->removeItem( 1 );
    if (k1 > 0)
      m_sortKey1->setCurrentItem(--k1);
    if (k2 > 0)
      m_sortKey2->setCurrentItem(--k2);
    if (k3 > 0)
      m_sortKey3->setCurrentItem(--k3);
  }*/
}

#include "kspread_dlg_sort.moc"
