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

#ifndef _REMOTE_KEYBOARD_H
#define _REMOTE_KEYBOARD_H

#include <mutex>
#include <condition_variable>

#include "OISKeyboard.h"
#include "RemoteOISPrereqs.h"

namespace RemoteOIS
{
    /**
        Defines a protocol interface for sending and receiving keyboard input
        requests and responses.
    */
    class _RemoteOISExport KeyboardProtocol : public DeviceProtocol
    {
    public:
        /**
        @remarks
            Parses a response and retrieves the keyboard state.
            Should only be called if canParseResponse returns true.
        @param response
            The response to parse
        @param buffer
            The keyboard buffer to fill (should be 256 bytes)
        @param modifiers
            A reference to the keyboard modifiers to set
        */
        virtual void parseResponse(WindowDataResponse response, char* buffer,
                                   unsigned int& modifiers) const = 0;
    };

    class _RemoteOISExport Keyboard : public OIS::Keyboard, public ConnectionListener
    {
    public:
        Keyboard(InputManager* creator, Connection* connection, bool buffered, DeviceProtocol* protocol, bool async);
        virtual ~Keyboard();

        /** @copydoc OIS::Keyboard::isKeyDown */
        virtual bool isKeyDown(OIS::KeyCode key) const;

        /** @copydoc OIS::Keyboard::getAsString */
        virtual const std::string& getAsString(OIS::KeyCode kc);

        /** @copydoc OIS::Keyboard::copyKeyStates */
        virtual void copyKeyStates(char keys[256]) const;

        /** @copydoc OIS::Object::setBuffered */
        virtual void setBuffered(bool buffered);

        /** @copydoc OIS::Object::capture */
        virtual void capture();

        /** @copydoc OIS::Object::queryInterface */
        virtual OIS::Interface* queryInterface(OIS::Interface::IType type) {return 0;}

        /** @copydoc OIS::Object::_initialize */
        virtual void _initialize();

        /** @copydoc ConnectionListener::understands */
        virtual bool understands(WindowDataResponse);

        /** @copydoc ConnectionListener::interpret */
        virtual void interpret(WindowDataResponse);

        /** @remarks Returns whether or not the mouse input is asynchronous */
        bool async() { return mAsync; }

        /** @remarks Sets whether or not the mouse input is asynchronous */
        void setAsync(bool async);

    protected:
        //! The connection used to send requests and receive responses
        Connection* mConnection;

        //! The protocol used to send requests and receive responses
        const DeviceProtocol* mProtocol;

        //! Internal method for translating KeyCodes to Text
        int _translateText( OIS::KeyCode kc );

        char KeyBuffer[256];

        char TempKeyBuffer[256];

        unsigned int mTempModifiers;

        bool mUpdated;

        std::mutex mUpdateLock;

        std::condition_variable mUpdateCv;

        //! used for getAsString
        std::string mGetString;

        //! Indicates if input is captured in lock step
        bool mAsync;
    };
}

#endif