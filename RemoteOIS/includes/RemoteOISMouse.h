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

#ifndef _REMOTE_MOUSE_H
#define _REMOTE_MOUSE_H

#include <condition_variable>
#include <mutex>

#include "OISMouse.h"
#include "RemoteOISPrereqs.h"

namespace RemoteOIS
{
    /**
        Defines a protocol interface for sending and receiving mouse input
        requests and responses.
    */
    class _RemoteOISExport MouseProtocol : public DeviceProtocol
    {
    public:
        /**
        @remarks
            Parses a response and retrieves the mouse state.
            Should only be called if canParseResponse returns true.
        */
        virtual OIS::MouseState parseResponse(WindowDataResponse response) const = 0;
    };

    class _RemoteOISExport Mouse : public OIS::Mouse, public ConnectionListener
    {
    public:
        Mouse( InputManager* creator, Connection* connection, bool buffered, DeviceProtocol* protocol);
        virtual ~Mouse();

        /** @copydoc OIS::Object::setBuffered */
        virtual void setBuffered(bool buffered);

        /** @copydoc OIS::Object::capture */
        virtual void capture();

        /** @copydoc OIS::Object::queryInterface */
        virtual OIS::Interface* queryInterface(OIS::Interface::IType type) {return 0;}

        /** @copydoc OIS::Object::_initialize */
        virtual void _initialize();

        /** @copydoc ConnectionListener::understands */
        virtual bool understands(WindowDataResponse response);

        /** @copydoc ConnectionListener::interpret */
        virtual void interpret(WindowDataResponse response);

        /** @remarks Returns whether or not mouse input is being scaled */
        bool scaling() { return mScalingInput; }

        /** @remarks Sets whether or not mouse input is being scaled */
        virtual void setScaling(bool scaling);

        /** @remarks Returns the raw (unscaled) state of the mouse */
        OIS::MouseState getRawMouseState() const;

    protected:
        //! The connection used to send requests and receive responses
        Connection* mConnection;

        //! The protocol used to send requests and receive responses
        DeviceProtocol* mProtocol;

        OIS::MouseState mTempState;

        bool mUpdated;

        std::mutex mUpdateLock;

        std::condition_variable mUpdateCv;

        //! The current width/height scale factors
        float mWidthScale, mHeightScale;

        //! Indicates if the mouse state is being scaled
        bool mScalingInput;
    };
}

#endif //_REMOTE_MOUSE_H