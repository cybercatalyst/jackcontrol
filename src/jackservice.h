/****************************************************************************
   Copyright (C) 2015, Jacob Dawid <jacob@omg-it.works>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#pragma once

// Qt includes
#include <QObject>
#include <QSocketNotifier>

// QJack includes
#include <System>
#include <Server>
#include <Client>

class JackService : public QObject {
    Q_OBJECT
public:
    static JackService& instance() {
        static JackService jackService;
        return jackService;
    }

    enum MessageType {
        MessageTypeNormal,
        MessageTypeError,
        MessageTypeStdOut
    };

    void start();
    void stop();

    QJack::Client& client();
    QJack::Server& server();

signals:
    void message(QString message, JackService::MessageType messageType);
    void connected();
    void disconnected();

private slots:
    void stdOutActivated(int fileDescriptor);

    void handleConnectedToServer();
    void handleDisconnectedFromServer();
    void handleError(QString errorMessage);

private:
    void setupStdOutRedirect();

private:
    JackService(QObject *parent = 0);

    QSocketNotifier *_stdOutSocketNotifier;
    QJack::Server _jackServer;
    QJack::Client _jackClient;
};
