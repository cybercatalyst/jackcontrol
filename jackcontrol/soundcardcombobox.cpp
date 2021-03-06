/****************************************************************************
   Copyright (C) 2013, Arnout Engelen. All rights reserved.
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

// Own includes
#include "soundcardcombobox.h"

// Qt includes
#include <QTreeView>
#include <QHeaderView>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#ifdef SUPPORTS_COREAUDIO
#include <iostream>
#include <cstring>
#include <map>
#include <CoreAudio/CoreAudio.h>
#include <CoreFoundation/CFString.h>
#endif

#ifdef SUPPORTS_PORTAUDIO
#include <iostream>
#include <cstring>
#include <portaudio.h>
#endif

#ifdef SUPPORTS_ALSA
#include <alsa/asoundlib.h>
#endif

SoundcardComboBox::SoundcardComboBox(QWidget *parent ) :
    QComboBox(parent) {
    _driverName = "";
    _operationModeFilter = Settings::OperationModeDuplex;
}

void SoundcardComboBox::showPopup() {
	populateModel();
	QComboBox::showPopup();
}

void SoundcardComboBox::setDriverName(QString driverName) {
    _driverName = driverName.toLower();
    populateModel();
}

void SoundcardComboBox::setOperationModeFilter(Settings::OperationMode operationMode) {
    _operationModeFilter = operationMode;
    populateModel();
}

void SoundcardComboBox::update() {
    populateModel();
}

QString SoundcardComboBox::alsaDeviceIdentifier(int soundcardIndex, int pcmDeviceIndex) {
    return QString("hw:%1%2")
        .arg(soundcardIndex)
        .arg(pcmDeviceIndex >= 0 ? QString(",%1").arg(pcmDeviceIndex) : "");
}

void SoundcardComboBox::populateModel () {
    clear();

    if (_driverName == "alsa") {
#ifdef SUPPORTS_ALSA
        // Enumerate the ALSA cards and PCM hardware devices.
        snd_ctl_t           *soundcardHandle;
        snd_ctl_card_info_t *soundcardInfo;
        snd_pcm_info_t      *pcmDeviceInfo;
        snd_ctl_card_info_alloca(&soundcardInfo);
        snd_pcm_info_alloca(&pcmDeviceInfo);

        int soundcardIndex = -1;

        // Query the next soundcard if available.
        while(snd_card_next(&soundcardIndex) >= 0
           && soundcardIndex >= 0) {

            // Access the soundcard in order to grab informations about it.
            if(snd_ctl_open(&soundcardHandle,
                            alsaDeviceIdentifier(soundcardIndex).toUtf8().constData(),
                            0) >= 0
            && snd_ctl_card_info(soundcardHandle, soundcardInfo) >= 0) {
                QString soundCardId = snd_ctl_card_info_get_id(soundcardInfo);
                QString soundCardName = snd_ctl_card_info_get_longname(soundcardInfo);

                addItem(QIcon(":/images/jockey.svg"),
                        QString("%1: %2 [%3]")
                            .arg(soundCardId)
                            .arg(soundCardName)
                            .arg(alsaDeviceIdentifier(soundcardIndex)),
                        alsaDeviceIdentifier(soundcardIndex));

                int pcmDeviceIndex = -1;
                while (snd_ctl_pcm_next_device(soundcardHandle, &pcmDeviceIndex) >= 0 && pcmDeviceIndex >= 0) {

                    bool probeIsCapturingDevice = false;
                    if (_operationModeFilter == Settings::OperationModeCapture ||
                        _operationModeFilter == Settings::OperationModeDuplex) {
                        snd_pcm_info_set_device(pcmDeviceInfo, pcmDeviceIndex);
                        snd_pcm_info_set_subdevice(pcmDeviceInfo, 0);
                        snd_pcm_info_set_stream(pcmDeviceInfo, SND_PCM_STREAM_CAPTURE);
                        probeIsCapturingDevice = (snd_ctl_pcm_info(soundcardHandle, pcmDeviceInfo) >= 0);
					}

                    bool probeIsPlaybackDevice = false;
                    if (_operationModeFilter == Settings::OperationModePlayback ||
                        _operationModeFilter == Settings::OperationModeDuplex) {
                        snd_pcm_info_set_device(pcmDeviceInfo, pcmDeviceIndex);
                        snd_pcm_info_set_subdevice(pcmDeviceInfo, 0);
                        snd_pcm_info_set_stream(pcmDeviceInfo, SND_PCM_STREAM_PLAYBACK);
                        probeIsPlaybackDevice = (snd_ctl_pcm_info(soundcardHandle, pcmDeviceInfo) >= 0);
					}

                    if ((_operationModeFilter == Settings::OperationModeCapture && probeIsCapturingDevice && !probeIsPlaybackDevice) ||
                        (_operationModeFilter == Settings::OperationModePlayback && !probeIsCapturingDevice && probeIsPlaybackDevice) ||
                        (_operationModeFilter == Settings::OperationModeDuplex && probeIsCapturingDevice && probeIsPlaybackDevice)) {
                        QString pcmDeviceName = snd_pcm_info_get_name(pcmDeviceInfo);
                        addItem(QIcon(":/images/audio-x-wav.svg"),
                                QString("%1: %2 [%3]")
                                    .arg(soundCardId)
                                    .arg(pcmDeviceName)
                                    .arg(alsaDeviceIdentifier(soundcardIndex, pcmDeviceIndex)),
                                alsaDeviceIdentifier(soundcardIndex, pcmDeviceIndex));
                    }
				}
                snd_ctl_close(soundcardHandle);
			}
		}
#else
        qDebug() << "This application has been compiled without ALSA support.";
#endif 	// SUPPORTS_ALSA
    } else if(_driverName == "sun") {
#ifdef SUPPORTS_SUN
		QFile file("/var/run/dmesg.boot");
		if (file.open(QIODevice::ReadOnly)) {
			QTextStream stream(&file);
			QString sLine;
			QRegExp rxDevice("audio([0-9]) at (.*)");
			while (!stream.atEnd()) {
				sLine = stream.readLine();
				if (rxDevice.exactMatch(sLine)) {
                    addItem(QIcon(":/images/jockey.svg"), "/dev/audio" + rxDevice.cap(1) + " (" + rxDevice.cap(2) + ")");
				}
			}
			file.close();
		}
#else
        qDebug() << "This application has been compiled without SUN support.";
#endif // SUPPORTS_SUN
    } else if(_driverName == "oss") {
#ifdef SUPPORTS_OSS
		// Enumerate the OSS Audio devices...
		QFile file("/dev/sndstat");
		if (file.open(QIODevice::ReadOnly)) {
			QTextStream stream(&file);
			QString sLine;
			bool bAudioDevices = false;
			QRegExp rxHeader("Audio devices.*", Qt::CaseInsensitive);
			QRegExp rxDevice("([0-9]+):[ ]+(.*)");
			while (!stream.atEnd()) {
				sLine = stream.readLine();
				if (bAudioDevices) {
					if (rxDevice.exactMatch(sLine)) {
                        addItem(QIcon(":/images/jockey.svg"), "/dev/dsp" + rxDevice.cap(1) + " (" + rxDevice.cap(2) + ")");
					}
					else break;
				}
				else if (rxHeader.exactMatch(sLine))
					bAudioDevices = true;
			}
			file.close();
		}
#else
        qDebug() << "This application has been compiled without OSS support.";
#endif
    } else if(_driverName == "coreaudio") {
#ifdef SUPPORTS_COREAUDIO
        std::map<QString, AudioDeviceID> coreaudioIdMap;

		// Find out how many Core Audio devices are there, if any...
		// (code snippet gently "borrowed" from Stephane Letz jackdmp;)
		OSStatus err;
		Boolean isWritable;
		UInt32 outSize = sizeof(isWritable);
		err = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices,
				&outSize, &isWritable);
		if (err == noErr) {
			// Calculate the number of device available...
			int numCoreDevices = outSize / sizeof(AudioDeviceID);
			// Make space for the devices we are about to get...
			AudioDeviceID *coreDeviceIDs = new AudioDeviceID [numCoreDevices];
			err = AudioHardwareGetProperty(kAudioHardwarePropertyDevices,
					&outSize, (void *) coreDeviceIDs);
			if (err == noErr) {
				// Look for the CoreAudio device name...
				char coreDeviceName[256];
				UInt32 nameSize = 256;
				for (int i = 0; i < numCoreDevices; i++) {
					err = AudioDeviceGetPropertyInfo(coreDeviceIDs[i],
							0, true, kAudioDevicePropertyDeviceName,
							&outSize, &isWritable);
					if (err == noErr) {
						err = AudioDeviceGetProperty(coreDeviceIDs[i],
								0, true, kAudioDevicePropertyDeviceName,
								&nameSize, (void *) coreDeviceName);
						if (err == noErr) {
							char drivername[128];
							UInt32 dnsize = 128;
							// this returns the unique id for the device
							// that must be used on the commandline for jack
							if (getDeviceUIDFromID(coreDeviceIDs[i],
								drivername, dnsize) == noErr) {
								sName = drivername;
							} else {
								sName = "Error";
							}
							coreaudioIdMap[sName] = coreDeviceIDs[i];
							// TODO: hide this ugly ID from the user,
							// only show human readable name
							// humanreadable \t UID
							sSubName = QString(coreDeviceName);
							addCard(sSubName, sName);
							if (sCurName == sName || sCurName == sSubName)
								iCurCard = iCards;
							++iCards;
						}
					}
				}
			}
			delete [] coreDeviceIDs;
        }
#else
        qDebug() << "This application has been compiled without core audio support.";
#endif 	// SUPPORTS_COREAUDIO
    } else if(_driverName == "portaudio") {
#ifdef SUPPORTS_PORTAUDIO
//		if (Pa_Initialize() == paNoError) {
//			// Fill hostapi info...
//			PaHostApiIndex iNumHostApi = Pa_GetHostApiCount();
//			QString *pHostName = new QString[iNumHostApi];
//			for (PaHostApiIndex i = 0; i < iNumHostApi; ++i)
//				pHostName[i] = QString(Pa_GetHostApiInfo(i)->name);
//			// Fill device info...
//			PaDeviceIndex iNumDevice = Pa_GetDeviceCount();
//			PaDeviceInfo **ppDeviceInfo = new PaDeviceInfo * [iNumDevice];
//			for (PaDeviceIndex i = 0; i < iNumDevice; ++i) {
//				ppDeviceInfo[i] = const_cast<PaDeviceInfo *> (Pa_GetDeviceInfo(i));
//				sName = pHostName[ppDeviceInfo[i]->hostApi] + "::" + QString(ppDeviceInfo[i]->name);
//				addCard(sName, QString());
//				if (sCurName == sName)
//					iCurCard = iCards;
//				++iCards;
//			}
//			Pa_Terminate();
//		}
#else
        qDebug() << "This application has been compiled without port audio support.";
#endif  // SUPPORTS_PORTAUDIO
    }

    setCurrentIndex(0);
}
