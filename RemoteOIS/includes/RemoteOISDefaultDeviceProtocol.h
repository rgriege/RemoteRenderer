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

#ifndef _DEFAULT_REMOTE_DEVICE_PROTOCOL_H
#define _DEFAULT_REMOTE_DEVICE_PROTOCOL_H

#include <sstream>
#include <vector>
#include <cassert>

#include "RemoteOISPrereqs.h"
#include "RemoteOISMouse.h"
#include "RemoteOISKeyboard.h"

namespace RemoteOIS
{
    /**
        A default protocol for sending and receiving mouse input requests
        and responses.  Allows InputManager::createObject to be inherited
        from OIS::InputManager without a MouseProtocol parameter.
    */
    class _RemoteOISExport DefaultMouseProtocol : public MouseProtocol
    {
    public:
        DefaultMouseProtocol() : prevX(0), prevY(0) {}

        /** @copydoc DeviceProtocol::createCaptureRequest */
        virtual WindowDataRequest createCaptureRequest() const;

        /** @copydoc DeviceProtocol::canParseResponse */
        virtual bool canParseResponse(WindowDataResponse response) const;

        /** @copydoc MouseProtocol::parseResponse */
        virtual OIS::MouseState parseResponse(WindowDataResponse response) const;

        /** @copydoc DeviceProtocol::deviceType */
        virtual OIS::Type deviceType() const { return OIS::OISMouse; }

    private:
        //! Used for determining relative axis movement
        mutable int prevX, prevY;
    };

    /**
        A default protocol for sending and receiving keyboard input requests
        and responses.  Allows InputManager::createObject to be inherited
        from OIS::InputManager without a KeyboardProtocol parameter.
    */
    class _RemoteOISExport DefaultRemoteKeyboardProtocol : public KeyboardProtocol
    {
        /** @copydoc DeviceProtocol::createCaptureRequest */
        virtual WindowDataRequest createCaptureRequest() const;

        /** @copydoc DeviceProtocol::canParseResponse */
        virtual bool canParseResponse(WindowDataResponse response) const;

        /** @copydoc KeyboardProtocol::parseResponse */
        virtual void parseResponse(WindowDataResponse response, char* buffer, unsigned int& modifiers) const;

        /** @copydoc DeviceProtocol::deviceType */
        virtual OIS::Type deviceType() const { return OIS::OISKeyboard; }
    };
}

#endif