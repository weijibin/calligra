/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998	  */
/* Version: 0.0.1						  */
/* Author: Reginald Stadlbauer, Torben Weis			  */
/* E-Mail: reggie@kde.org, weis@kde.org				  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Format Collection					  */
/******************************************************************/

#include "formatcollection.h"
#include "kword_doc.h"
#include "font.h"

#include <qcolor.h>
#include <qfont.h>

/******************************************************************/
/* Class: KWFormatCollection					  */
/******************************************************************/

/*================================================================*/
KWFormatCollection::KWFormatCollection( KWordDocument *_doc )
    : formats( 1999, true )
{
    formats.setAutoDelete( true );
    doc = _doc;
}

/*================================================================*/
KWFormatCollection::~KWFormatCollection()
{
    formats.clear();
}

/*================================================================*/
KWFormat *KWFormatCollection::getFormat( const KWFormat &_format )
{
    QString key = generateKey( _format );

    KWFormat *format = findFormat( key );
    if ( format )
    {
	format->incRef();
	return format;
    }
    else
	return insertFormat( key, _format );
}

/*================================================================*/
void KWFormatCollection::removeFormat( KWFormat *_format )
{
    QString key = generateKey( *_format );

    formats.remove( key );
}

/*================================================================*/
QString KWFormatCollection::generateKey( const KWFormat &_format )
{
    QString key;

    // Key: BIU-Fontname-Fontsize-red-gree-blue
    // e.g. B**-Times-12-255-40-32
    key.sprintf( "%c%c%c-%s-%d-%d-%d-%d-%d",
		 ( _format.getWeight() == QFont::Bold ? 'B' : '*' ),
		 ( _format.getItalic() == 1 ? 'I' : '*' ),
		 ( _format.getUnderline() == 1 ? 'U' : '*' ),
		 _format.getUserFont()->getFontName().data(),
		 _format.getPTFontSize(), _format.getColor().red(),
		 _format.getColor().green(), _format.getColor().blue(),
		 _format.getVertAlign() );

    return key;
}

/*================================================================*/
KWFormat *KWFormatCollection::findFormat( QString _key )
{
    return formats.find( _key.data() );
}

/*================================================================*/
KWFormat *KWFormatCollection::insertFormat( QString _key, const KWFormat &_format )
{
    KWFormat *format = new KWFormat( doc, _format );

    formats.insert( _key.data(), format );
    format->incRef();

    return format;
}

/*================================================================*/
QDomElement KWFormatCollection::save( QDomDocument &doc )
{
    QDomElement formats_ = doc.createElement( "FORMATS" );
    indexMap.clear();

    QDictIterator<KWFormat> it( formats );
    for ( unsigned int i = 0; it.current(); ++it ) {
	indexMap[ it.current() ] = i++;
	QDomElement f = it.current()->save( doc, i );
	if ( f.isNull() )
	    return f;
	formats_.appendChild( f );
    }

    return formats_;
}

/*================================================================*/
// #### todo
// void KWFormatCollection::load( KOMLParser&, vector<KOMLAttrib>& )
// {
// }

