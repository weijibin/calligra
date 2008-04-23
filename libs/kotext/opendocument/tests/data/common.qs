var document = new QTextDocument;
var cursor = new QTextCursor(document);

// See KoCharacterStyle.cpp
var defaultFont = new QFont;
defaultFont.setPointSizeF(12.0);
defaultFont.setWeight(QFont.Normal);

var defaultTextFormat = new QTextCharFormat;
defaultTextFormat.setFont(defaultFont);
defaultTextFormat.setVerticalAlignment(QTextCharFormat.AlignNormal);

QTextCharFormat.clone = function(fmt) {
    var newFormat = new QTextCharFormat;
    copyFormatProperties(newFormat, fmt);

    // CHECKME: When new formats are created, the color is explicitly set to Black
    if (newFormat.foreground().style() == 0) {
        newFormat.setForeground(new QBrush(new QColor(0, 0, 0)));
    }
    return newFormat;
};

QTextListFormat.clone = function(fmt) {
    var newFormat = new QTextListFormat;
    copyFormatProperties(newFormat, fmt);
    return newFormat;
};

// KOffice specific
var KoListStyle = {};
KoListStyle.ListItemSuffix = 0x1003E9;
KoListStyle.StartValue = 0x1003EA;
KoListStyle.Level = 0x1003EB;

var defaultListItemFormat = new QTextCharFormat;
defaultListItemFormat.setFont(defaultFont);
defaultListItemFormat.setVerticalAlignment(QTextCharFormat.AlignNormal);
defaultListItemFormat.setForeground(new QBrush(new QColor(0, 0, 0)));

var defaultListFormat = new QTextListFormat;
setFormatProperty(defaultListFormat, KoListStyle.StartValue, 1);
setFormatProperty(defaultListFormat, KoListStyle.Level, 1);

