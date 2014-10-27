/*
 *  Copyright (c) 2012 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "KisFlipbookSelector.h"

#include <kis_doc2.h>
#include <kis_part2.h>
#include <kis_flipbook.h>
#include <kis_flipbook_item.h>
#include <kis_image.h>

#include <KoApplication.h>
#include <KoIcon.h>
#include <KoFilterManager.h>
#include <KoFileDialog.h>

#include <QDesktopServices>
#include <QListWidget>
#include <QListWidgetItem>

KisFlipbookSelector::KisFlipbookSelector(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(bnCreateNewFlipbook, SIGNAL(clicked()), SLOT(createFlipbook()));
}

void KisFlipbookSelector::createFlipbook()
{
    KoFileDialog dialog(this, KoFileDialog::OpenFiles, "OpenDocument");
    dialog.setCaption(i18n("Select files to add to flipbook"));
    dialog.setDefaultDir(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation));
    dialog.setMimeTypeFilters(koApp->mimeFilter(KoFilterManager::Import));
    QStringList urls = dialog.urls();

    if (urls.size() < 1) return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    KisFlipbook *flipbook = KisPart2::instance()->createFlipbook();
    foreach(QString url, urls) {
        if (QFile::exists(url)) {
            flipbook->addItem(url);
        }
    }

    flipbook->setName(txtFlipbookName->text());

    QApplication::restoreOverrideCursor();
    emit documentSelected(flipbook);
}
