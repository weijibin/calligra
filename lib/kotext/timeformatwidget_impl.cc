#include "timedateformatwidget.h"
#include "timeformatwidget_impl.h"
#include "timeformatwidget_impl.moc"
#include <qdatetime.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <kglobal.h>
#include <klocale.h>
#include <qlineedit.h>
#include <knuminput.h>
#include <koVariable.h>

/*
 *  Constructs a TimeFormatWidget which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
TimeFormatWidget::TimeFormatWidget( QWidget* parent,  const char* name, WFlags fl )
    : TimeDateFormatWidgetPrototype( parent, name, fl )
{
    setCaption( i18n( "TimeFormat", "This dialog allows you to set the format of the time variable" ) );

    QStringList listTimeFormat = KoVariableTimeFormat::staticTranslatedFormatPropsList();
    combo1->insertStringList(listTimeFormat);

    combo2->insertItem( i18n( "Hour" ) );
    combo2->insertItem( i18n( "Hour (2 digits)" ) );
    combo2->insertItem( i18n( "Minute" ) );
    combo2->insertItem( i18n( "Minute (2 digits)" ) );
    combo2->insertItem( i18n( "Second" ) );
    combo2->insertItem( i18n( "Second (2 digits)" ) );
    combo2->insertItem( i18n( "Millisecond (3 digits)" ) );
    combo2->insertItem( i18n( "am/pm" ) );
    combo2->insertItem( i18n( "AM/PM" ) );
    combo2->setCurrentItem( 0 );

    label_correction->setText(i18n("Correct in Minutes"));
    connect( CheckBox1, SIGNAL(toggled ( bool )),this,SLOT(slotPersonalizeChanged(bool)));
    connect( combo1, SIGNAL(activated ( const QString & )), this, SLOT(slotDefaultValueChanged(const QString &)));
    connect( KIntNumInput1, SIGNAL(valueChanged(int)), this, SLOT( slotOffsetChanged(int)));
    slotPersonalizeChanged(false);
}

/*
 *  Destroys the object and frees any allocated resources
 */
TimeFormatWidget::~TimeFormatWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 * public slot
 */
void TimeFormatWidget::slotDefaultValueChanged(const QString & )
{
    updateLabel();
}

void TimeFormatWidget::slotOffsetChanged(int)
{
    updateLabel();
}

void TimeFormatWidget::slotPersonalizeChanged(bool b)
{
    combo2->setEnabled(b);
    combo1->setEditable(b);
    TextLabel1->setEnabled(b);
    updateLabel();

}

void TimeFormatWidget::comboActivated()
{
    QString string=combo2->currentText();
    if(string==i18n("Hour"))
        combo1->lineEdit()->insert("h");
    else if(string==i18n("Hour (2 digits)"))
        combo1->lineEdit()->insert("hh");
    else if(string==i18n("Minute"))
        combo1->lineEdit()->insert("m");
    else if(string==i18n("Minute (2 digits)"))
        combo1->lineEdit()->insert("mm");
    else if(string==i18n("Second"))
        combo1->lineEdit()->insert("s");
    else if(string==i18n("Second (2 digits)"))
        combo1->lineEdit()->insert("ss");
    else if(string==i18n("Millisecond (3 digits)"))
        combo1->lineEdit()->insert("zzz");
    else if(string==i18n("AM/PM"))
        combo1->lineEdit()->insert("AP");
    else if(string==i18n("am/pm"))
        combo1->lineEdit()->insert("ap");
    updateLabel();
    combo1->setFocus();
}

/*
 * public slot
 */
void TimeFormatWidget::updateLabel()
{
    KoVariableTimeFormat format;
    format.setFormatProperties( resultString() );

    QTime ct = QTime::currentTime().addSecs(correctValue());
    label->setText( format.convert( ct ) );
}

QString TimeFormatWidget::resultString()
{
    // Lookup untranslated format
    QStringList listTimeFormat = KoVariableTimeFormat::staticFormatPropsList();

    QStringList::Iterator it = listTimeFormat.at(combo1->currentItem());
    Q_ASSERT( it != listTimeFormat.end() );
    if ( it != listTimeFormat.end() )
        return *it;
    return QString::null;
}

int TimeFormatWidget::correctValue()
{
    return KIntNumInput1->value()*60;
}
