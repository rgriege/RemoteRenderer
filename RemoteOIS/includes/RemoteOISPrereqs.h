/*
The zlib/libpng License

Copyright (c) 2005-2007 Phillip Castaneda (pjcast -- www.wreckedgames.com)
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
#define REMOTE_OIS_VERSION_MINOR 2
#define REMOTE_OIS_VERSION_PATCH 0
#define REMOTE_OIS_VERSION_NAME "0.2.0"

#define REMOTE_OIS_VERSION ((REMOTE_OIS_VERSION_MAJOR << 16) | (REMOTE_OIS_VERSION_MINOR << 8) | REMOTE_OIS_VERSION_PATCH)

namespace RemoteOIS
{
    // Forward declarations
    class InputManager;
    class Keyboard;
    class Mouse;
    class ConnectionManager;

    //! A simple data request struct
    struct _RemoteOISExport WindowDataRequest
    {
        void* data;
        size_t len;
    };

    //! A simple data response struct
    struct _RemoteOISExport WindowDataResponse
    {
        const void* data;
        size_t len;
    };

    //Interface Declarations
    /**
        Defines a listener interface for receiving a WindowDataResponse over
        an established Connection.
    */
    class _RemoteOISExport ConnectionListener
    {
    public:
        /** @remarks Fired when a Connection is opened */
        //virtual void onOpen() = 0;

        /** @remarks Determines whether or not the listener can interpret a response */
        virtual bool understands(WindowDataResponse response) = 0;

        /** @remarks Interprets a response.  Should be called only if understands returned true */
        virtual void interpret(WindowDataResponse response) = 0;

        /** @remarks Fired when a Connection is closed */
        //virtual void onClose() = 0;
    };

    /**
        Defines an interface for connections over which a WindowDataRequest or
        WindowDataResponse can be sent.
    */
    class _RemoteOISExport Connection
    {
    public:
        /** @remarks Opens the Connection */
        virtual void open() = 0;

        /** @remarks Sends a request */
        virtual void send(WindowDataRequest request) const = 0;

        /** @remarks Adds a ConnectionListener to the list of listeners */
        virtual void addConnectionListener(ConnectionListener* listener) = 0;

        /** @remarks Removes a ConnectionListener from the list of listeners */
        virtual void removeConnectionListener(ConnectionListener* listener) = 0;

        /** @remarks Closes the Connection */
        virtual void close() = 0;
    };

    /**
        Defines a protocol interface for sending and receiving OIS::Object requests
        and responses.
    */
    class _RemoteOISExport DeviceProtocol
    {
    public:
        /** @remarks Creates a request to capture remote device input */
        virtual WindowDataRequest createCaptureRequest() const = 0;
        
        /** @remarks Determines whether or not a response follows the protocol */
        virtual bool canParseResponse(WindowDataResponse response) const = 0;
        
        /** @remarks Returns the type of device this protocol supports */
        virtual OIS::Type deviceType() const = 0;
    };

    /**
        Defines a protocol interface for sending and receiving OIS::FactoryCreator requests
        and responses.
    */
    class _RemoteOISExport FactoryCreatorProtocol
    {
    public:
        /** @remarks
            Creates a request for OIS::FactoryCreator::freeDeviceList
        @return
            A WindowDataRequest with the corresponding request
        */
        virtual WindowDataRequest createFreeDeviceListRequest() const = 0;

        /**
        @remarks
            Creates a request for OIS::FactoryCreator::totalDeviceCount
        @param iType
            Type of devices to check
        @return
            A WindowDataRequest with the corresponding request
        */
        virtual WindowDataRequest createTotalDeviceCountRequest(OIS::Type iType) const = 0;

        /**
        @remarks
            Creates a request for OIS::FactoryCreator::freeDeviceCount
        @param iType
            Type of devices to check
        @return
            A WindowDataRequest with the corresponding request
        */
        virtual WindowDataRequest createFreeDeviceCountRequest(OIS::Type iType) const = 0;

        /**
        @remarks
            Creates a request for OIS::FactoryCreator::vendorExist
        @param iType
            Type to check
        @param vendor
            Vendor name to test
        @return
            A WindowDataRequest with the corresponding request
        */
        virtual WindowDataRequest createVendorExistsRequest(OIS::Type iType, const std::string & vendor) const = 0;

        /**
        @remarks
            Parses a response for OIS::FactoryCreator::freeDeviceList
        @param response
            The response to a request from createFreeDeviceListRequest
        @return
            A list of all unused devices the factory maintains
        */
        virtual OIS::DeviceList parseFreeDeviceListResponse(WindowDataResponse response) const = 0;

        /**
        @remarks
            Parses a response for OIS::FactoryCreator::totalDeviceCount
        @param response
            The response to a request from createTotalDeviceCountRequest
        @return
            Number of total devices of requested type
        */
        virtual int parseTotalDeviceCountResponse(WindowDataResponse response) const = 0;

        /**
        @remarks
            Parses a response for OIS::FactoryCreator::freeDeviceCount
        @param response
            The response to a request from createFreeDeviceCountRequest
        @return
            Number of free devices of requested type
        */
        virtual int parseFreeDeviceCountResponse(WindowDataResponse response) const = 0;

        /**
        @remarks
            Parses a response for OIS::FactoryCreator::vendorExist
        @param response
            The response to a request from createVendorExistsRequest
        @return
            Does a Type exist with the given vendor name
        */
        virtual bool parseVendorExistsResponse(WindowDataResponse response) const = 0;
    };
}

#endif //_REMOTE_INPUTSYSTEM_PREREQS_H
