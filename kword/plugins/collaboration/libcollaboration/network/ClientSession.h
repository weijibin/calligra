/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KCOLLABORATE_CLIENTSESSION_H
#define KCOLLABORATE_CLIENTSESSION_H

#include <libcollaboration/network/Session.h>

namespace kcollaborate
{
class Url;

class KCOLLABORATE_EXPORT ClientSession : public Session
{
        Q_OBJECT

    public:
        ClientSession( const Url &url, QObject *parent = 0 );
        virtual ~ClientSession();

        void reconnect();
        virtual void disconnect();

        bool readOnly() const { return readOnly_; };

    private:
        bool readOnly_;
};

};

#endif
