/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2001       Sven Leiber         <s.leiber@web.de>

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

#ifndef koautoformat_h
#define koautoformat_h

#include <qstring.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qptrvector.h>
#include <qdom.h>
#include <qdict.h>
class KoDocument;
class KoTextParag;
class KoTextObject;
class KoVariableCollection;
class KoVariableFormatCollection;
class KCompletion;
class KoTextCursor;
class KCommand;
class KoSearchContext;
class KoTextFormat;

/******************************************************************/
/* Class: KWAutoFormatEntry					  */
/******************************************************************/
class KoAutoFormatEntry
{
public:
    // The text to find is actually the key in KWAutoFormat's map.
    // What we replace it with is replace().
    KoAutoFormatEntry(const QString& replace = QString::null);
    ~KoAutoFormatEntry();
    QString replace() const { return m_replace; }
    KoSearchContext *formatEntryContext()const;
    void createNewEntryContext();
    void setFormatEntryContext( KoSearchContext * );
    void clearFormatEntryContext( );

protected:
    QString m_replace;
    // For formatting in the replacement - not implemented yet
    KoSearchContext *m_formatOptions;
};

typedef QMap< QString, KoAutoFormatEntry > KoAutoFormatEntryMap;

/******************************************************************/
/* Class: KoAutoFormat						  */
/******************************************************************/
class KoAutoFormat
{
public:
    /**
     * There is a single instance of KoAutoFormat per document
     * (and a temporary one in the auto-format dialog).
     */
    KoAutoFormat( KoDocument *_doc, KoVariableCollection *_varCollection, KoVariableFormatCollection *_varFormatCollection );

    /** Copy constructor, used by KoAutoFormatDia */
    KoAutoFormat( const KoAutoFormat& format );

    ~KoAutoFormat();

    KCommand *applyAutoFormat( KoTextObject * obj );
    /**
     * Called by edit widget when a character (@p ch) has been inserted
     * into @p parag, at the given @p index.
     */
    void doAutoFormat( KoTextCursor* cursor, KoTextParag *parag, int index, QChar ch,KoTextObject *txtObj );

    /**
     * Called by edit widget when a call a competion
     */
    void doCompletion( KoTextCursor* textEditCursor, KoTextParag *parag, int index,KoTextObject *txtObj );


    bool doIgnoreDoubleSpace( KoTextParag *parag, int index,QChar ch );

    /**
     * Helper method, returns the last word before parag,index
     */
    static QString getLastWord( KoTextParag *parag, int index );

    /**
     * Helper method, returns the last word before parag,index
     * different from getLastWord, because we test just space character
     * and not punctualtion character
     */
    static QString getWordAfterSpace( KoTextParag * parag, int index);

    // Config for the typographic quotes. Used by the dialog.
    struct TypographicQuotes
    {
	QChar begin, end;
	bool replace; // aka enabled
    };

    // Configuration (on/off/settings). Called by the dialog.
    void configTypographicDoubleQuotes( TypographicQuotes _tq );
    void configTypographicSimpleQuotes( TypographicQuotes _tq );

    void configUpperCase( bool _uc );
    void configUpperUpper( bool _uu );
    void configAdvancedAutocorrect( bool _aa );
    void configAutoDetectUrl(bool _au);
    void configIgnoreDoubleSpace( bool _ids);
    void configRemoveSpaceBeginEndLine( bool _space);
    void configUseBulletStyle( bool _ubs);

    void configBulletStyle( QChar b );

    void configAutoChangeFormat( bool b);

    void configAutoReplaceNumber( bool b );

    void configAutoNumberStyle( bool b );

    void configCompletion( bool b );

    void configAppendSpace( bool b);

    void configMinWordLength( uint val );

    void configNbMaxCompletionWord( uint val );

    void configAddCompletionWord( bool b );

    void configIncludeTwoUpperUpperLetterException( bool b);

    void configIncludeAbbreviation( bool b );

    void configAutoSuperScript( bool b );

    void configCorrectionWithFormat( bool b);

    void configAutoFormatLanguage( const QString &_lang);

    TypographicQuotes getConfigTypographicSimpleQuotes() const
    { return m_typographicSimpleQuotes; }

    TypographicQuotes getConfigTypographicDoubleQuotes() const
    { return m_typographicDoubleQuotes; }

    TypographicQuotes getDefaultTypographicDoubleQuotes() const
        { return m_typographicDefaultDoubleQuotes; }

    TypographicQuotes getDefaultTypographicSimpleQuotes() const
        { return m_typographicDefaultSimpleQuotes; }

    bool getConfigUpperCase() const
    { return m_convertUpperCase; }
    bool getConfigUpperUpper() const
    { return m_convertUpperUpper; }
    bool getConfigAdvancedAutoCorrect() const
    { return m_advancedAutoCorrect;}
    bool getConfigAutoDetectUrl() const
    { return m_autoDetectUrl;}

    bool getConfigIgnoreDoubleSpace() const
    { return m_ignoreDoubleSpace;}

    bool getConfigRemoveSpaceBeginEndLine() const
    { return m_removeSpaceBeginEndLine;}

    bool getConfigUseBulletSyle() const
    { return m_useBulletStyle;}

    QChar getConfigBulletStyle() const
    { return m_bulletStyle; }

    bool getConfigAutoChangeFormat() const
    { return m_autoChangeFormat;}

    bool getConfigAutoReplaceNumber() const
    { return m_autoReplaceNumber; }

    bool getConfigAutoNumberStyle() const
    { return m_useAutoNumberStyle; }

    bool getConfigCompletion() const
    { return m_completion; }

    bool getConfigAppendSpace() const
    { return m_completionAppendSpace; }

    uint getConfigMinWordLength() const
    { return m_minCompletionWordLength; }

    uint getConfigNbMaxCompletionWord() const
    { return m_nbMaxCompletionWord; }

    bool getConfigAddCompletionWord() const
    { return m_addCompletionWord; }

    bool getConfigIncludeTwoUpperUpperLetterException() const
    { return m_includeTwoUpperLetterException; }

    bool getConfigIncludeAbbreviation() const
    { return m_includeAbbreviation; }

    bool getConfigAutoSuperScript(  ) const
    { return m_bAutoSuperScript; }

    bool getConfigCorrectionWithFormat() const
    { return m_bAutoCorrectionWithFormat; }


    QString getConfigAutoFormatLanguage( )const
        { return m_autoFormatLanguage;}


    const QDict<KoAutoFormatEntry> & getAutoFormatEntries() const{
        return m_entries;
    }

    KoAutoFormatEntry * findFormatEntry(const QString & text) {
        return m_entries[text];
    }

    // Add/remove entries, called by the dialog
    void addAutoFormatEntry( const QString &key, KoAutoFormatEntry *entry ) {
	m_entries.insert( key, entry );
	buildMaxLen();
    }

    void removeAutoFormatEntry( const QString &key ) {
        m_entries.setAutoDelete(true);
        m_entries.remove( key );
        m_entries.setAutoDelete(false);
	buildMaxLen();
    }

    // Copy all autoformat entries from another KWAutoFormat. Called by the dialog
    void copyAutoFormatEntries( const KoAutoFormat & other )
    { m_entries = other.m_entries; }

    void copyListException( const QStringList & _list)
	{ m_upperCaseExceptions=_list;}

    void copyListTwoUpperCaseException( const QStringList &_list)
	{ m_twoUpperLetterException=_list; }

    QStringList listException() const {return m_upperCaseExceptions;}

    QStringList listTwoUpperLetterException() const {return m_twoUpperLetterException;}

    QStringList listCompletion() const;

    KCompletion *getCompletion() const { return m_listCompletion; }

    int nbSuperScriptEntry() const
        { return m_superScriptEntries.count(); }

    // Read/save config ( into kwordrc )
    void readConfig(bool force = false);
    void saveConfig();

    static bool isUpper( const QChar &c );
    static bool isLower( const QChar &c );
    static bool isMark( const QChar &c ); // End of sentence
    static bool isSeparator( const QChar &c );
protected:
    //return a ref to index otherwise when we uperCase, index is bad !
    KCommand *doAutoCorrect( KoTextCursor* textEditCursor, KoTextParag *parag, int & index, KoTextObject *txtObj );
    KCommand *doUpperCase( KoTextCursor* textEditCursor, KoTextParag *parag, int index, const QString & word , KoTextObject *txtObj );
    KCommand * doTypographicQuotes( KoTextCursor* textEditCursor, KoTextParag *parag, int index, KoTextObject *txtObj, bool doubleQuotes );
    void buildMaxLen();

    void doAutoDetectUrl( KoTextCursor *textEditCursor, KoTextParag *parag,int index, const QString & word, KoTextObject *txtObj );
    KCommand *doRemoveSpaceBeginEndLine( KoTextCursor *textEditCursor, KoTextParag *parag, KoTextObject *txtObj );
    KCommand *doAutoChangeFormat( KoTextCursor *textEditCursor, KoTextParag *parag,int index, const QString & word, KoTextObject *txtObj );
    KCommand *doUseBulletStyle(KoTextCursor *textEditCursor, KoTextParag *parag, KoTextObject *txtObj, int& index );

    KCommand *doAutoReplaceNumber( KoTextCursor* textEditCursor, KoTextParag *parag, int index, const QString & word , KoTextObject *txtObj );

    KCommand *doUseNumberStyle(KoTextCursor * /*textEditCursor*/, KoTextParag *parag, KoTextObject *txtObj, int& index );

    void doAutoIncludeUpperUpper(KoTextCursor *textEditCursor, KoTextParag *parag, KoTextObject *txtObj );
    void doAutoIncludeAbbreviation(KoTextCursor *textEditCursor, KoTextParag *parag, KoTextObject *txtObj );

    KCommand *doAutoSuperScript( KoTextCursor* textEditCursor, KoTextParag *parag, int index, const QString & word , KoTextObject *txtObj );
    KCommand *scanParag( KoTextParag * parag, KoTextObject * obj );

    static void changeTextFormat(KoSearchContext *formatOptions, KoTextFormat * format, int & flags );
    void loadEntry( const QDomElement &nl);
    QDomElement saveEntry( QDictIterator<KoAutoFormatEntry> _entry, QDomDocument doc);
private:
    void detectStartOfLink(const QString &word);
    void autoFormatIsActive();
    void loadListOfWordCompletion();

    KoDocument *m_doc;
    KoVariableCollection *m_varCollection;
    KoVariableFormatCollection *m_varFormatCollection;

    QString m_autoFormatLanguage;
    bool m_configRead;
    bool m_convertUpperCase, m_convertUpperUpper,m_advancedAutoCorrect;
    bool m_autoDetectUrl, m_ignoreDoubleSpace, m_removeSpaceBeginEndLine;
    bool m_useBulletStyle, m_autoChangeFormat, m_autoReplaceNumber;
    bool m_useAutoNumberStyle;
    bool m_completion;
    bool m_completionAppendSpace;
    bool m_addCompletionWord;
    bool m_includeTwoUpperLetterException;
    bool m_includeAbbreviation;
    bool m_ignoreUpperCase;
    bool m_bAutoFormatActive;

    bool m_bAutoSuperScript;
    bool m_bAutoCorrectionWithFormat;
    QChar m_bulletStyle;

    TypographicQuotes m_typographicSimpleQuotes;

    TypographicQuotes m_typographicDoubleQuotes;

    TypographicQuotes m_typographicDefaultDoubleQuotes;

    TypographicQuotes m_typographicDefaultSimpleQuotes;

    KCompletion *m_listCompletion;

    QDict<KoAutoFormatEntry> m_entries;

    KoAutoFormatEntryMap m_superScriptEntries;

    QStringList m_upperCaseExceptions;
    QStringList m_twoUpperLetterException;

    uint m_maxFindLength;
    uint m_minCompletionWordLength;
    uint m_nbMaxCompletionWord;
};

#endif
