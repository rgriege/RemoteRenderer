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

#ifndef _REMOTE_INPUT_MANAGER_H
#define _REMOTE_INPUT_MANAGER_H

#include <map>

#include "OISInputManager.h"
#include "OISFactoryCreator.h"
#include "RemoteOISPrereqs.h"
#include "RemoteOISDefaultFactoryCreatorProtocol.h"
#include "RemoteOISDefaultDeviceProtocol.h"

namespace RemoteOIS
{
    class _RemoteOISExport InputManager : public OIS::InputManager, public OIS::FactoryCreator
    {
    public:
        InputManager();

        /**
        @remarks
            Creates input system. Hides OIS::InputManager::createInputSystem
        @param paramList
            ParamList contains connection name as stored in ConnectionManager
        @returns
            A pointer to the created manager, or raises Exception
        */
        static InputManager* createInputSystem( OIS::ParamList &paramList );

        /**
        @remarks
            Creates input system. Hides OIS::InputManager::createInputSystem
        @param connection
            A connection to the remote window
        @param protocol
            The protocol to use when sending device information requests
        @returns
            A pointer to the created manager, or raises Exception
        */
        static InputManager* createInputSystem( Connection* connection, 
            FactoryCreatorProtocol* protocol = new DefaultFactoryCreatorProtocol() );

        /**
        @remarks
            Destroys the InputManager
        @param manager
            Manager to destroy
        */
        static void destroyRemoteInputSystem(InputManager* manager);

        //FactoryCreator Overrides
        /** @copydoc FactoryCreator::deviceList */
        virtual OIS::DeviceList freeDeviceList();

        /** @copydoc FactoryCreator::totalDevices */
        virtual int totalDevices(OIS::Type iType);

        /** @copydoc FactoryCreator::freeDevices */
        virtual int freeDevices(OIS::Type iType);

        /** @copydoc FactoryCreator::vendorExist */
        virtual bool vendorExist(OIS::Type iType, const std::string & vendor);

        /** @copydoc FactoryCreator::createObject */
        virtual OIS::Object* createObject(OIS::InputManager* creator, OIS::Type iType, bool bufferMode,
            const std::string & vendor = "");

        /**
        @remarks Creates the object
        @param creator
            The creator of the object
        @param protocol
            The communication protocol for the object
        @param bufferMode
            True to setup for buffered events
        @param vendor
            Create a device with the vendor name, "" means vendor name is unimportant
        */
        OIS::Object* createObject(InputManager* creator, DeviceProtocol* protocol, bool bufferMode,
            const std::string & vendor = "");

        /** @copydoc FactoryCreator::destroyObject */
        virtual void destroyObject(OIS::Object* obj);

    protected:
        /**
        @remarks
            Derived classes must provide input system name
        */
        InputManager(const std::string& name);

        /**
        @remarks
            Virtual Destructor - this base class will clean up all devices still opened in mFactoryObjects list
        */
        virtual ~InputManager();

        virtual void _initialize(OIS::ParamList& paramList);

        void _initialize(Connection* connection, FactoryCreatorProtocol* protocol);

        Connection* mConnection;

        FactoryCreatorProtocol* mProtocol;

        bool mKeyboardUsed;

        bool mMouseUsed;
    };
}
#endif