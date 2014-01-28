/*
The zlib/libpng License

Copyright (c) 2014 Ryan Griege (www.github.com/rgriege)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that 
		you wrote the original software. If you use this software in a product, 
		an acknowledgment in the product documentation would be appreciated but is 
		not required.

    2. Altered source versions must be plainly marked as such, and must not be 
		misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/
#ifndef _REMOTE_INPUTSYSTEM_PREREQS_H
#define _REMOTE_INPUTSYSTEM_PREREQS_H

#include "OISPrereqs.h"
#include <inttypes.h>

#define _RemoteOISExport _OISExport

#define REMOTE_OIS_VERSION_MAJOR 0
#define REMOTE_OIS_VERSION_MINOR 1
#define REMOTE_OIS_VERSION_PATCH 0
#define REMOTE_OIS_VERSION_NAME "0.1.0"

#define REMOTE_OIS_VERSION ((REMOTE_OIS_VERSION_MAJOR << 16) | (REMOTE_OIS_VERSION_MINOR << 8) | REMOTE_OIS_VERSION_PATCH)

namespace RemoteOIS
{
	// Forward declarations
	class InputManager;
	class Keyboard;
	class Mouse;
	
	struct _RemoteOISExport WindowDataRequest
	{
		void* data;
		size_t len;
	};

	struct _RemoteOISExport WindowDataResponse
	{
		const void* data;
		size_t len;
	};

	//Interface Declarations
	class _RemoteOISExport MessageListener
	{
	public:
		virtual bool understands(WindowDataResponse) = 0;
		virtual void interpret(WindowDataResponse) = 0;
	};

	class _RemoteOISExport Connection
	{
	public:
		virtual void open() = 0;
		/*virtual void addOpenListener() = 0;
		virtual void removeOpenListener() = 0;*/
		virtual void send(WindowDataRequest) const = 0;
		virtual void addMessageListener(MessageListener*) = 0;
		virtual void removeMessageListener(MessageListener*) = 0;
		/*virtual void addCloseListener() = 0;
		virtual void removeCloseListener() = 0;*/
		virtual void close() = 0;
		virtual std::string name() const = 0;
	};

	class _RemoteOISExport DeviceProtocol
	{
	public:
		virtual WindowDataRequest createCaptureRequest() const = 0;
		virtual bool canParseResponse(WindowDataResponse response) const = 0;
		virtual OIS::Type deviceType() const = 0;
	};

	class _RemoteOISExport FactoryCreatorProtocol
	{
	public:
		virtual WindowDataRequest createFreeDeviceListRequest() const = 0;
		virtual WindowDataRequest createTotalDeviceCountRequest(OIS::Type iType) const = 0;
		virtual WindowDataRequest createFreeDeviceCountRequest(OIS::Type iType) const = 0;

		virtual OIS::DeviceList parseFreeDeviceListResponse(WindowDataResponse response) const = 0;
		virtual int parseTotalDeviceCountResponse(WindowDataResponse response) const = 0;
		virtual int parseFreeDeviceCountResponse(WindowDataResponse response) const = 0;
	};
}

#endif //_REMOTE_INPUTSYSTEM_PREREQS_H
