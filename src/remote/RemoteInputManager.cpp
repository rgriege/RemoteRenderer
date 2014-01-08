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
#include "remote/RemoteInputManager.h"
#include "remote/RemoteKeyBoard.h"
#include "remote/RemoteMouse.h"
//#include "remote/RemoteJoyStick.h"
#include "remote/RemoteConnectionManager.h"
#include "OISException.h"

using namespace OIS;

//--------------------------------------------------------------------------------//
RemoteInputManager::RemoteInputManager()
	: InputManager("Remote Input Manager"),
	mConnection(0),
	mProtocol(0),
	mKeyboardUsed(false),
	mMouseUsed(false)
{
	mFactories.push_back(this);
}

//--------------------------------------------------------------------------------//
RemoteInputManager::~RemoteInputManager()
{
}

//----------------------------------------------------------------------------//
RemoteInputManager* RemoteInputManager::createInputSystem( ParamList &paramList )
{
	RemoteInputManager* im = new RemoteInputManager();

	try
	{
		im->_initialize(paramList);
	}
	catch(...)
	{
		delete im;
		throw; //rethrow
	}

	return im;
}

//--------------------------------------------------------------------------------//
RemoteInputManager* RemoteInputManager::createInputSystem(RemoteConnection* connection, 
	RemoteFactoryCreatorProtocol* protocol)
{
	RemoteInputManager* im = new RemoteInputManager();

	try
	{
		im->_initialize(connection, protocol);
	}
	catch(...)
	{
		delete im;
		throw; //rethrow
	}

	return im;
}

//--------------------------------------------------------------------------------//
void RemoteInputManager::_initialize( ParamList &paramList )
{
	//First of all, get the Windows Handle and Instance
	ParamList::iterator i = paramList.find("CONNECTION");
	if(i == paramList.end())
		OIS_EXCEPT( E_InvalidParam, "RemoteInputManager::RemoteInputManager >> No CONNECTION found!" );

	_initialize(RemoteConnectionManager::getInstance()->find(i->second), 0);
}

//--------------------------------------------------------------------------------//
void RemoteInputManager::_initialize( RemoteConnection* connection,
									  RemoteFactoryCreatorProtocol* protocol )
{
	mConnection = connection;
	if (!mConnection)
		OIS_EXCEPT( E_InvalidParam, "RemoteInputManager::RemoteInputManager >> No CONNECTION found!" );

	mProtocol = protocol ? protocol : new DefaultRemoteFactoryCreatorProtocol();
}

//--------------------------------------------------------------------------------//
DeviceList RemoteInputManager::freeDeviceList()
{
	return DeviceList(); //mWindow->requestFreeDeviceList();
}

//--------------------------------------------------------------------------------//
int RemoteInputManager::totalDevices(Type iType)
{
	return 0; //mWindow->requestTotalDeviceCount(iType);
}

//--------------------------------------------------------------------------------//
int RemoteInputManager::freeDevices(Type iType)
{
	return 1; //mWindow->requestFreeDeviceCount(iType);
}

//--------------------------------------------------------------------------------//
bool RemoteInputManager::vendorExist(Type iType, const std::string & vendor)
{
	return true;
}

//--------------------------------------------------------------------------------//
Object* RemoteInputManager::createObject(InputManager* creator, Type iType, bool bufferMode, const std::string & vendor)
{
	Object *obj = 0;
	switch (iType)
	{
	case OISMouse:
		obj = createObject(creator, new DefaultRemoteMouseProtocol(), bufferMode, vendor);
		break;
	}
	return obj;
}

Object* RemoteInputManager::createObject(InputManager* creator, RemoteDeviceProtocol* protocol, bool bufferMode, const std::string & vendor)
{
	Object *obj = 0;

	switch(protocol->deviceType())
	{
	case OISKeyboard: 
	{
		//if( mKeyboardUsed == false )
			//obj = new RemoteKeyboard(this, mConnection, bufferMode, protocol);
		mKeyboardUsed = true;
		break;
	}
	case OISMouse:
	{
		if( mMouseUsed == false )
			obj = new RemoteMouse(this, mConnection, bufferMode, protocol);
		mMouseUsed = true;
		break;
	}
	default:
		break;
	}

	if( obj == 0 )
		OIS_EXCEPT(E_InputDeviceNonExistant, "No devices match requested type.");
	
	return obj;
}

//--------------------------------------------------------------------------------//
void RemoteInputManager::destroyObject(Object* obj)
{
	delete obj;
}