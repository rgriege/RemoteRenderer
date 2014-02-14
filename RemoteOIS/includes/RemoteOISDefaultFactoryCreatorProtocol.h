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

#ifndef _DEFAULT_REMOTE_FACTORY_CREATOR_PROTOCOL_H
#define _DEFAULT_REMOTE_FACTORY_CREATOR_PROTOCOL_H

#include "OIS.h"
#include "RemoteOISPrereqs.h"

namespace RemoteOIS
{
    /**
        A default protocol for sending and receiving FactoryCreator requests
        and responses.  Allows InputManager to inherit FactoryCreator methods
        without requiring a FactoryCreatorProtocol at instantiation.
    @remarks
        Currently sends and receives dummy responses and requests - work in progress
    */
    class _RemoteOISExport DefaultFactoryCreatorProtocol : public FactoryCreatorProtocol
    {
    public:
        /** @copydoc FactoryCreatorProtocol::createFreeDeviceListRequest */
        virtual WindowDataRequest createFreeDeviceListRequest() const { WindowDataRequest request = { 0, 0 }; return request; }

        /** @copydoc FactoryCreatorProtocol::createTotalDeviceCountRequest */
        virtual WindowDataRequest createTotalDeviceCountRequest(OIS::Type iType) const { WindowDataRequest request = { 0, 0 }; return request; }

        /** @copydoc FactoryCreatorProtocol::createFreeDeviceCountRequest */
        virtual WindowDataRequest createFreeDeviceCountRequest(OIS::Type iType) const { WindowDataRequest request = { 0, 0 }; return request; }

        /** @copydoc FactoryCreatorProtocol::createVendorExistsRequest */
        virtual WindowDataRequest createVendorExistsRequest(OIS::Type iType, const std::string& vendor) const { WindowDataRequest request = { 0, 0 }; return request; }

        /** @copydoc FactoryCreatorProtocol::parseFreeDeviceListResponse */
        virtual OIS::DeviceList parseFreeDeviceListResponse(WindowDataResponse response) const { return OIS::DeviceList(); }

        /** @copydoc FactoryCreatorProtocol::parseTotalDeviceCountResponse */
        virtual int parseTotalDeviceCountResponse(WindowDataResponse response) const { return 0; }

        /** @copydoc FactoryCreatorProtocol::parseFreeDeviceCountResponse */
        virtual int parseFreeDeviceCountResponse(WindowDataResponse response) const { return 0; }

        /** @copydoc FactoryCreatorProtocol::parseVendorExistsResponse */
        virtual bool parseVendorExistsResponse(WindowDataResponse response) const { return true; }
    };
}

#endif