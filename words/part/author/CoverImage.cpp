/* This file is part of the KDE project
   Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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

#include "CoverImage.h"

#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoStoreDevice.h>

#include <kdebug.h>
#include <kmimetype.h>
#include <klocale.h>
#include <QFile>
#include <QFileDialog>
#include <QPair>

CoverImage::CoverImage()
{
}

bool CoverImage::saveCoverImage(KoStore *store, KoXmlWriter *manifestWriter, QPair<QString, QByteArray> coverData)
{
    // There is no cover to save.
    if (coverData.first.isEmpty())
        return true;

    if (!store->open("Author-Profile/cover." + coverData.first)) {
        kDebug(31000) << "Unable to open Author-Profile/cover."<<coverData.first;
        return false;
    }

    KoStoreDevice device(store);
    device.write(coverData.second, coverData.second.size());
    store->close();

    const QString mimetype(KMimeType::findByPath("Author-Profile/cover." + coverData.first, 0 , true)->name());
    manifestWriter->addManifestEntry("Author-Profile/cover." + coverData.first, mimetype);

    return true;
}

QPair<QString, QByteArray> CoverImage::getCoverData(QString path)
{
    QFile file (path);
    if (!file.open(QIODevice::ReadOnly)) {
        kDebug(31000) << "Unable to open" << path;
    }
    QByteArray data = file.readAll();

    QPair<QString, QByteArray> coverData;
    coverData.first = path.right(3);
    coverData.second = data;

    file.close();
    return coverData;
}
