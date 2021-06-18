// Meatloaf - A Commodore 64/128 multi-device emulator
// https://github.com/idolpx/meatloaf
// Copyright(C) 2020 James Johnston
//
// Meatloaf is free software : you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Meatloaf is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Meatloaf. If not, see <http://www.gnu.org/licenses/>.

#ifndef IECDEVICE_H
#define IECDEVICE_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#if defined(USE_SPIFFS)
#include <SPIFFS.h>
#elif defined(USE_LITTLEFS)
#include <LittleFS.h>
#endif

#include "../../include/global_defines.h"
#include "../../include/cbmdefines.h"
#include "../../include/petscii.h"
//#include "../../include/make_unique.h"

#include "iec.h"
#include "device_db.h"
#include "meat_io.h"
#include "buffered_io.h"
#include "EdUrlParser.h"
#include "MemoryInfo.h"
#include "helpers.h"
#include "utils.h"
#include "string_utils.h"

enum OpenState
{
	O_NOTHING,		// Nothing to send / File not found error
	O_INFO,			// User issued a reload sd card
	O_FILE,			// A program file is opened
	O_DIR,			// A listing is requested
	O_FILE_ERR,		// Incorrect file format opened
	O_SAVE_REPLACE, // Save-with-replace is requested
	O_DEVICE_INFO,
	O_DEVICE_STATUS
};

class Interface
{
public:
	Interface(IEC &iec, FS *fileSystem);
	virtual ~Interface() {}

	bool begin();

	// The handler returns the current IEC state, see the iec.hpp for possible states.
	byte loop(void);


private:
	void reset(void);

	void sendStatus(void);
	void sendFileNotFound(void);
	void sendDeviceInfo(void);
	void sendDeviceStatus(void);
	void setDeviceStatus(int number, int track=0, int sector=0);


	void sendListing(void);
	void sendListingHTTP(void);
	uint16_t sendHeader(uint16_t &basicPtr, std::string header);
	uint16_t sendLine(uint16_t &basicPtr, uint16_t blocks, char *text);
	uint16_t sendLine(uint16_t &basicPtr, uint16_t blocks, const char *format, ...);
	uint16_t sendFooter(uint16_t &basicPtr, uint16_t blocks_free, uint16_t block_size);
	void sendFile(void);
	void sendFileHTTP(void);

	void saveFile(void);

	// handler helpers.
	void handleATNCmdCodeOpen(IEC::ATNCmd &cmd);
	void handleATNCmdCodeDataListen(void);
	void handleATNCmdCodeDataTalk(byte chan);
	void handleATNCmdClose(void);

	void handleDeviceCommand(IEC::ATNCmd &cmd);
	void handleMeatLoafCommand(IEC::ATNCmd &cmd);

	// our iec low level driver:
	IEC &m_iec;

	// This var is set after an open command and determines what to send next
	byte m_openState; // see OpenState
	byte m_queuedError;

	// atn command buffer struct
	IEC::ATNCmd &m_atn_cmd;

	FS *m_fileSystem;
	std::shared_ptr<MFile> m_mfile;
	StaticJsonDocument<256> m_jsonHTTP;
	String m_lineBuffer;
	//DynamicJsonDocument m_jsonHTTPBuffer;

	DeviceDB m_device;

	std::string m_device_status;
	bool m_show_date;
	bool m_show_load_address;
	bool m_show_hidden;
	bool m_hide_extension;

	MFile* guessIncomingPath(std::string commandLne);
};

#endif
