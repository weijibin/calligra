/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTWBSDEFINITIONDIALOG_H
#define KPTWBSDEFINITIONDIALOG_H

#include "kplatoui_export.h"

#include <KoDialog.h>

class KUndo2Command;

namespace KPlato
{

class WBSDefinitionPanel;
class WBSDefinition;
class Project;

class KPLATOUI_EXPORT WBSDefinitionDialog : public KoDialog {
    Q_OBJECT
public:
    explicit WBSDefinitionDialog(Project &project, WBSDefinition &def, QWidget *parent=0);

    KUndo2Command *buildCommand();

protected Q_SLOTS:
    void slotOk();

private:
    WBSDefinitionPanel *m_panel;
};

} //KPlato namespace

#endif // WBSDEFINITIONDIALOG_H
