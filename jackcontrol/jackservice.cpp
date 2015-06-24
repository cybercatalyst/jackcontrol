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

// Own includes
#include "jackservice.h"

#include <QDebug>

#if !defined(WIN32)
#include <unistd.h>
#endif

JackService::JackService(QObject *parent)
    : QObject(parent) {
    setupStdOutRedirect();

    //hw:PCH,0|hw:PCH,0|256|3|44100|0|0|nomon|swmeter|-|32bit

}

void JackService::start() {
    QtJack::DriverMap drivers = _jackServer.availableDrivers();
    QtJack::Driver alsaDriver = drivers["alsa"];

    QtJack::ParameterMap parameters = alsaDriver.parameters();
    parameters["rate"].setValue(44100);
    parameters["device"].setValue("hw:PCH,0");

    _jackServer.start(alsaDriver);
    qDebug() << _jackClient.connectToServer("JACK Control");
    qDebug() << _jackClient.activate();
}

void JackService::stop() {
    _jackClient.disconnectFromServer();
    //_jackServer.stop();
}

QtJack::Client& JackService::client() {
    return _jackClient;
}

QtJack::Server& JackService::server() {
    return _jackServer;
}

void JackService::stdOutActivated(int fileDescriptor) {
    char achBuffer[1024];
    int  cchBuffer = ::read(fileDescriptor, achBuffer, sizeof(achBuffer) - 1);
    if (cchBuffer > 0) {
        achBuffer[cchBuffer] = (char) 0;
        emit message(achBuffer, MessageTypeStdOut);
    }
}

void JackService::setupStdOutRedirect() {
#if !defined(WIN32)
    // Redirect our own stdout/stderr.
    int pipeDescriptors[2];
    if(::pipe(pipeDescriptors) == 0) {
        ::dup2(pipeDescriptors[1], STDOUT_FILENO);
        ::dup2(pipeDescriptors[1], STDERR_FILENO);
        _stdOutSocketNotifier = new QSocketNotifier(pipeDescriptors[0], QSocketNotifier::Read, this);
        connect(_stdOutSocketNotifier, SIGNAL(activated(int)), this, SLOT(stdOutActivated(int)));
    }
#endif
}
