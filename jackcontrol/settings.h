/****************************************************************************
   Copyright (C) 2003-2013, rncbc aka Rui Nuno Capela. All rights reserved.
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
#include <QString>
#include <QSettings>
#include <QByteArray>

class Settings {
public:
    enum OperationMode {
        OperationModeCapture,
        OperationModePlayback,
        OperationModeDuplex
    };

    enum DitherMode {
        DitherModeNone,
        DitherModeRectangular,
        DitherModeShaped,
        DitherModeTriangular
    };

    struct JackControlSettings {
        int             _version;
        QByteArray      _mainWindowGeometry;
    };

    struct JackServerPreset {
        QString         _presetName;
        int             _version;

        // Device settings
        QString         _audioDriverName;
        QString         _midiDriverName;
        OperationMode   _operationMode;
        QString         _inputDeviceIdentifier;
        QString         _outputDeviceIdentifier;

        // Audio processing
        bool            _enableRealtimeProcessing;
        int             _samplesPerFrame;
        int             _samplesPerSecond;
        int             _bufferSizeMultiplier;
        int             _maximumNumberOfPorts;
        bool            _provideMonitorPorts;
        bool            _enableHardwareMonitoring;
        bool            _enableHardwareMetering;
        DitherMode      _ditherMode;
        int             _realtimePriority;

        // Advanced configuration
        int             _clientTimeout;
        int             _maximumNumberOfAudioChannels;
        int             _maximumNumberOfHardwareInputChannels;
        int             _maximumNumberOfHardwareOutputChannels;
        int             _externalInputLatency;
        int             _externalOutputLatency;
        int             _dummyDriverProcessingDelay;
        int             _wordLength;

        bool            _noMemoryLock;
        bool            _unlockMemory;
        bool            _force16BitWordLength;
        bool            _ignoreHardwareBufferSize;
        bool            _enableSoftMode;
        bool            _showVerboseMessages;
    };

    static JackControlSettings loadJackControlSettings(QString fileName, bool *ok = 0);
    static bool saveJackControlSettings(QString fileName, JackControlSettings jackControlSettings);

    static JackServerPreset loadPreset(QString fileName, bool *ok = 0);
    static bool savePreset(QString fileName, JackServerPreset preset);

private:
    static bool loadJackControlSettingsVersion1(QSettings& settings, JackControlSettings& jackControlSettings);
    static bool saveJackControlSettingsVersion1(QSettings& settings, JackControlSettings& JackControlSettings);

    static bool loadJackServerPresetVersion1(QSettings& settings, JackServerPreset& jackServerPreset);
    static bool saveJackServerPresetVersion1(QSettings& settings, JackServerPreset& jackServerPreset);

    Settings();
    ~Settings();
};

