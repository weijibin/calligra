/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PARAGRAPHSTYLESMODEL_H
#define PARAGRAPHSTYLESMODEL_H

#include <QModelIndex>

class ParagraphStylesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QObject* textEditor READ textEditor WRITE setTextEditor NOTIFY textEditorChanged)

public:
    enum ParagraphStyleRoles {
        Name = Qt::UserRole + 1,
        Current,
        Font,
        FontFamily,
        FontPointSize,
        FontWeight,
        FontItalic,
        FontUnderline
    };
    ParagraphStylesModel();
    ~ParagraphStylesModel();
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent) const;

    QObject* document() const;
    void setDocument(QObject* newDocument);

    QObject* textEditor() const;
    void setTextEditor(QObject* newEditor);

    Q_SLOT void cursorPositionChanged();
signals:
    void documentChanged();
    void textEditorChanged();

private:
    class Private;
    Private* d;
};

#endif // PARAGRAPHSTYLESMODEL_H
