/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef __kofontdia_h__
#define __kofontdia_h__

#include <kfontdialog.h>
#include <qtabwidget.h>
#include <qrichtext_p.h>
#include <qradiobutton.h>
class QComboBox;

/**
 * The embeddable font chooser widget
 */
class KoFontChooser : public QTabWidget
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param fontListCriteria should contain all the restrictions for font selection as OR-ed values
     *        @see KFontChooser::FontListCriteria for the individual values
     */
    KoFontChooser( QWidget * parent, const char* name = 0L,
            bool _withSubSuperScript = true, uint fontListCriteria=0);
    virtual ~KoFontChooser();

    void setFont( const QFont &_font, bool _subscript, bool _superscript );
    void setColor( const QColor & col );
    void setBackGroundColor( const QColor & col );

    bool getSuperScript() const { return m_superScript->isChecked(); }
    bool getSubScript() const { return m_subScript->isChecked(); }


    QFont getNewFont() const { return m_newFont; }
    QColor color() const;
    QColor backGroundColor() const { return m_backGroundColor;}
    QColor underlineColor() const { return m_underlineColor ; }

    void setUnderlineColor( const QColor & col );


    KoTextFormat::UnderlineLineType getUnderlineLineType();
    KoTextFormat::UnderlineLineStyle getUnderlineLineStyle();
    KoTextFormat::StrikeOutLineType getStrikeOutLineType();
    KoTextFormat::StrikeOutLineStyle getStrikeOutLineStyle();

    void setUnderlineLineType(KoTextFormat::UnderlineLineType nb);
    void setStrikeOutlineType(KoTextFormat::StrikeOutLineType nb);
    void setUnderlineLineStyle(KoTextFormat::UnderlineLineStyle _t);
    void setStrikeOutLineStyle(KoTextFormat::StrikeOutLineStyle _t);

    void setShadowText( bool _b);
    bool getShadowText()const;

    bool getWordByWord()const;
    void setWordByWord( bool _b);

    KoTextFormat::AttributeStyle getFontAttribute()const;
    void setFontAttribute( KoTextFormat::AttributeStyle _att);


    double getRelativeTextSize()const;
    void setRelativeTextSize(double _size);

    int getOffsetFromBaseLine()const;
    void setOffsetFromBaseLine(int _offset);

    int changedFlags() const { return m_changedFlags; }
    void setupTab1(bool _withSubSuperScript, uint fontListCriteria );
    void setupTab2();
    void updatePositionButton();
protected slots:
    void slotSuperScriptClicked();
    void slotSubScriptClicked();
    void slotStrikeOutTypeChanged( int );
    void slotFontChanged(const QFont &);
    void slotChangeColor();
    void slotChangeBackGroundColor();
    void slotUnderlineColor();
    void slotChangeUnderlineType( int );
    void slotChangeUnderlining( int);

    void slotChangeStrikeOutType( int );
    void slotShadowClicked();
    void slotRelativeSizeChanged( int );
    void slotOffsetFromBaseLineChanged( int );
    void slotChangeAttributeFont( int );
    void slotWordByWordClicked();
private:
    KFontChooser *m_chooseFont;
    QRadioButton *m_superScript;
    QRadioButton *m_subScript;

    QComboBox *m_underlining;
    QComboBox *m_underlineType;

    QComboBox *m_strikeOutType;
    QPushButton *m_underlineColorButton;

    QPushButton *m_colorButton;
    QPushButton *m_backGroundColorButton;

    class KoFontChooserPrivate;
    KoFontChooserPrivate* d;

    QFont m_newFont;
    QColor m_backGroundColor;
    QColor m_underlineColor;

    int m_changedFlags;
};

class KoFontDia : public KDialogBase
{
    Q_OBJECT
public:
    KoFontDia( QWidget* parent, const char* name, const QFont &_font,
               bool _subscript, bool _superscript,
               bool _shadowText,
               bool _wordByWord,
               const QColor & color,
	       const QColor & backGroundColor,
               const QColor & underlineColor,
               KoTextFormat::UnderlineLineStyle _nbLine,
               KoTextFormat::UnderlineLineType _underlineType,
               KoTextFormat::StrikeOutLineType _strikeOutType,
               KoTextFormat::StrikeOutLineStyle _strikeOutLine,
               KoTextFormat::AttributeStyle _fontAttribute,
               double _relativeSize,
               int _offsetFromBaseLine,
               bool _withSubSuperScript=true );

    bool getSuperScript() const { return m_chooser->getSuperScript(); }
    bool getSubScript() const { return m_chooser->getSubScript(); }
    QFont getNewFont() const { return m_chooser->getNewFont(); }
    QColor color() const { return m_chooser->color(); }
    QColor backGroundColor() const {return m_chooser->backGroundColor();}
    QColor underlineColor() const { return m_chooser->underlineColor() ; }
    KoTextFormat::UnderlineLineType getUnderlineLineType() const { return m_chooser->getUnderlineLineType();}
    KoTextFormat::StrikeOutLineType getStrikeOutLineType() const { return m_chooser->getStrikeOutLineType();}

    KoTextFormat::UnderlineLineStyle getUnderlineLineStyle() const { return m_chooser->getUnderlineLineStyle();}
    KoTextFormat::StrikeOutLineStyle getStrikeOutLineStyle() const { return m_chooser->getStrikeOutLineStyle();}
    bool getShadowText()const{ return m_chooser->getShadowText();}
    double getRelativeTextSize()const{ return m_chooser->getRelativeTextSize();}

    int getOffsetFromBaseLine() const {return m_chooser->getOffsetFromBaseLine();}
    bool getWordByWord()const{ return m_chooser->getWordByWord();}

    int changedFlags() const { return m_chooser->changedFlags(); }

    KoTextFormat::AttributeStyle getFontAttribute()const { return m_chooser->getFontAttribute();}

protected slots:
    void slotReset();
    virtual void slotApply();
    virtual void slotOk();
signals:
     void apply();

private:
    KoFontChooser * m_chooser;
    QFont m_font;
    bool m_subscript;
    bool m_superscript;
    bool m_strikeOut;
    QColor m_color;
    QColor m_backGroundColor;
    QColor m_underlineColor;
    KoTextFormat::UnderlineLineType m_underlineType;
    KoTextFormat::UnderlineLineStyle m_underlineLineStyle;
    KoTextFormat::StrikeOutLineStyle m_strikeOutLineStyle;
    KoTextFormat::StrikeOutLineType m_strikeOutType;
    bool m_shadowText;
    double m_relativeSize;
    int m_offsetBaseLine;
    bool m_wordByWord;
    KoTextFormat::AttributeStyle m_fontAttribute;
};

#endif
