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
#ifndef OIS_RemoteInputManager_H
#define OIS_RemoteInputManager_H

#include <map>

#include "OISInputManager.h"
#include "OISFactoryCreator.h"
#include "remote/RemotePrereqs.h"
#include "remote/DefaultRemoteFactoryCreatorProtocol.h"
#include "remote/DefaultRemoteDeviceProtocol.h"

namespace OIS
{
	class RemoteInputManager : public InputManager, public FactoryCreator
	{
	public:
		RemoteInputManager();

		/**
		@remarks
			Creates appropriate input system dependent on platform. 
		@param paramList
			ParamList contains OS specific info (such as HWND and HINSTANCE for window apps),
			and access mode.
		@returns
			A pointer to the created manager, or raises Exception
		*/
		static RemoteInputManager* createInputSystem( ParamList &paramList );

		/**
		@remarks
			Creates appropriate input system dependent on platform. 
		@param paramList
			ParamList contains OS specific info (such as HWND and HINSTANCE for window apps),
			and access mode.
		@returns
			A pointer to the created manager, or raises Exception
		*/
		static RemoteInputManager* createInputSystem( RemoteConnection* connection, 
			RemoteFactoryCreatorProtocol* protocol = new DefaultRemoteFactoryCreatorProtocol() );

		/**
		@remarks
			Destroys the InputManager
		@param manager
			Manager to destroy
		*/
		static void destroyInputSystem(RemoteInputManager* manager);

		//FactoryCreator Overrides
		/** @copydoc FactoryCreator::deviceList */
		DeviceList freeDeviceList();

		/** @copydoc FactoryCreator::totalDevices */
		int totalDevices(Type iType);

		/** @copydoc FactoryCreator::freeDevices */
		int freeDevices(Type iType);

		/** @copydoc FactoryCreator::vendorExist */
		bool vendorExist(Type iType, const std::string & vendor);

		/** @copydoc FactoryCreator::createObject */
		Object* createObject(InputManager* creator, Type iType, bool bufferMode,
			const std::string & vendor = "");

		Object* createObject(InputManager* creator, RemoteDeviceProtocol* protocol, bool bufferMode,
			const std::string & vendor = "");

		/** @copydoc FactoryCreator::destroyObject */
		void destroyObject(Object* obj);

	protected:
		/**
		@remarks
			Derived classes must provide input system name
		*/
		RemoteInputManager(const std::string& name);

		/**
		@remarks
			Virtual Destructor - this base class will clean up all devices still opened in mFactoryObjects list
		*/
		virtual ~RemoteInputManager();

		void _initialize(ParamList& paramList);

		void _initialize(RemoteConnection* connection, RemoteFactoryCreatorProtocol* protocol);

		RemoteConnection* mConnection;

		RemoteFactoryCreatorProtocol* mProtocol;

		bool mKeyboardUsed;

		bool mMouseUsed;
	};
}
#endif