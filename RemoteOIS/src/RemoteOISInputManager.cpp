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
#include "RemoteOISInputManager.h"
#include "RemoteOISKeyBoard.h"
#include "RemoteOISMouse.h"
//#include "RemoteJoyStick.h"
#include "RemoteOISConnectionManager.h"
#include "OISException.h"

using namespace RemoteOIS;

//--------------------------------------------------------------------------------//
InputManager::InputManager()
	: OIS::InputManager("Remote Input Manager"),
	mConnection(0),
	mProtocol(0),
	mKeyboardUsed(false),
	mMouseUsed(false)
{
	mFactories.push_back(this);
}

//--------------------------------------------------------------------------------//
InputManager::~InputManager()
{
}

//----------------------------------------------------------------------------//
InputManager* InputManager::createInputSystem( OIS::ParamList &paramList )
{
	InputManager* im = new InputManager();

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
InputManager* InputManager::createInputSystem(Connection* connection, 
	FactoryCreatorProtocol* protocol)
{
	InputManager* im = new InputManager();

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
void InputManager::_initialize( OIS::ParamList &paramList )
{
	//First of all, get the Windows Handle and Instance
	OIS::ParamList::iterator i = paramList.find("CONNECTION");
	if(i == paramList.end())
		OIS_EXCEPT( OIS::E_InvalidParam, "InputManager::InputManager >> No CONNECTION found!" );

	_initialize(ConnectionManager::getInstance()->find(i->second), 0);
}

//--------------------------------------------------------------------------------//
void InputManager::_initialize( Connection* connection,
									  FactoryCreatorProtocol* protocol )
{
	mConnection = connection;
	if (!mConnection)
		OIS_EXCEPT( OIS::E_InvalidParam, "InputManager::InputManager >> No CONNECTION found!" );

	mProtocol = protocol ? protocol : new DefaultFactoryCreatorProtocol();
}

//--------------------------------------------------------------------------------//
OIS::DeviceList InputManager::freeDeviceList()
{
	return OIS::DeviceList();
}

//--------------------------------------------------------------------------------//
int InputManager::totalDevices(OIS::Type iType)
{
	return 0;
}

//--------------------------------------------------------------------------------//
int InputManager::freeDevices(OIS::Type iType)
{
	return 1;
}

//--------------------------------------------------------------------------------//
bool InputManager::vendorExist(OIS::Type iType, const std::string & vendor)
{
	return true;
}

//--------------------------------------------------------------------------------//
OIS::Object* InputManager::createObject(OIS::InputManager* creator, OIS::Type iType, bool bufferMode, const std::string & vendor)
{
	OIS::Object *obj = 0;
	switch (iType)
	{
	case OIS::OISMouse:
		obj = createObject(creator, new DefaultMouseProtocol(), bufferMode, vendor);
		break;
	case OIS::OISKeyboard:
		obj = createObject(creator, new DefaultRemoteKeyboardProtocol(), bufferMode, vendor);
		break;
	}
	return obj;
}

OIS::Object* InputManager::createObject(OIS::InputManager* creator, DeviceProtocol* protocol, bool bufferMode, const std::string & vendor)
{
	OIS::Object *obj = 0;

	switch(protocol->deviceType())
	{
	case OIS::OISKeyboard: 
	{
		if( mKeyboardUsed == false )
			obj = new Keyboard(this, mConnection, bufferMode, protocol);
		mKeyboardUsed = true;
		break;
	}
	case OIS::OISMouse:
	{
		if( mMouseUsed == false )
			obj = new Mouse(this, mConnection, bufferMode, protocol);
		mMouseUsed = true;
		break;
	}
	default:
		break;
	}

	if( obj == 0 )
		OIS_EXCEPT(OIS::E_InputDeviceNonExistant, "No devices match requested type.");
	
	return obj;
}

//--------------------------------------------------------------------------------//
void InputManager::destroyObject(OIS::Object* obj)
{
	delete obj;
}