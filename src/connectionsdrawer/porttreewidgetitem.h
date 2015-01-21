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

// Own includes
class ClientTreeWidgetItem;

// Qt includes
#include <QTreeWidgetItem>

#define QJACKCTL_PORTITEM      1002

/**
 * A tree widget item that represents a port of a client in the tree.
 */
class PortTreeWidgetItem : public QTreeWidgetItem {
public:
    PortTreeWidgetItem(QString portName);
    virtual ~PortTreeWidgetItem();

    // Instance accessors.
    void setPortName(QString portName);
    const QString& portName() const;

    // Client port cleanup marker.
    void markPort(int mark);
    int portMark() const;

    // Connectiopn highlight methods.
    bool highlighted() const;
    void setHighlighted(bool highlighted);

    virtual bool isConnectedTo(PortTreeWidgetItem *other);

private:

    // Instance variables.
    ClientTreeWidgetItem *_clientTreeWidgetItem;

    int     _portMark;
    bool    _highlight;
};
