/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIDIALOGBASE_H
#define KEXIDIALOGBASE_H

#include <kmdichildview.h>
#include <kxmlguiclient.h>

class KexiMainWindow;
class KActionCollection;

class KexiDialogBase : public KMdiChildView, public KXMLGUIClient
{
	Q_OBJECT

	public:
		KexiDialogBase(KexiMainWindow *parent, const QString &title);
		~KexiDialogBase();
		bool isRegistered();
		virtual KXMLGUIClient* guiClient();
	protected:
		void registerDialog();
	private:
		KexiMainWindow *m_parentWindow;
		bool m_isRegistered;
};

#endif

