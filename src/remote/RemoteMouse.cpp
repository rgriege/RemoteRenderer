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
#include "remote/RemoteMouse.h"
#include "remote/RemoteInputManager.h"
#include "OISException.h"
#include "OISEvents.h"

using namespace OIS;

//--------------------------------------------------------------------------------------------------//
RemoteMouse::RemoteMouse( InputManager* creator, RemoteConnection* connection, bool buffered, RemoteDeviceProtocol* protocol )
	: Mouse(creator->inputSystemName(), buffered, 0, creator), mConnection(connection), mProtocol(protocol)
{
	mConnection->addMessageListener(this);
}

//--------------------------------------------------------------------------------------------------//
void RemoteMouse::_initialize()
{
	//Clear old state
	mState.clear();
	mTempState.clear();
}

//--------------------------------------------------------------------------------------------------//
RemoteMouse::~RemoteMouse()
{
	mConnection->removeMessageListener(this);
}

//--------------------------------------------------------------------------------------------------//
void RemoteMouse::capture()
{
	//Clear old relative values
	mState.X.rel = mState.Y.rel = mState.Z.rel = 0;

	WindowDataRequest request = mProtocol->createCaptureRequest(mType);
	{
		std::lock_guard<std::mutex> updateGuard(mUpdateLock);
		mUpdated = true;
	}
	mConnection->send(request);
	//wait on interpret to be called with matching response
	std::unique_lock<std::mutex> updateLk(mUpdateLock);
	mUpdateCv.wait(updateLk, [&]() { return mUpdated; });
	updateLk.unlock();

	bool mouseMoved = false;
	uint8_t buttonPressed = 0;
	uint8_t buttonReleased = 0;
	    
	if(mTempState.X.rel || mTempState.Y.rel || mTempState.Z.rel)
	{
		//printf("%i %i %i\n\n", mTempState.X.rel, mTempState.Y.rel, mTempState.Z.rel);

		//Set new relative motion values
		mState.X.rel = mTempState.X.rel;
		mState.Y.rel = mTempState.Y.rel;
		mState.Z.rel = mTempState.Z.rel;
		
		//Update absolute position
		mState.X.abs += mTempState.X.rel;
		mState.Y.abs += mTempState.Y.rel;
		mState.Z.abs += mTempState.Z.rel;
		
		//Clip values to window
		if(mState.X.abs > mState.width)
			mState.X.abs = mState.width;
		else if(mState.X.abs < 0)
			mState.X.abs = 0;

		if(mState.Y.abs > mState.height)
			mState.Y.abs = mState.height;
		else if(mState.Y.abs < 0)
			mState.Y.abs = 0;
		
		mouseMoved = true;
	}

	for (uint8_t i = 0; i < 8; ++i) {
		MouseButtonID id = static_cast<MouseButtonID>(i);
		if (mTempState.buttonDown(id) && !mState.buttonDown(id))
			buttonPressed |= (1 << i);
		else if (!mTempState.buttonDown(id) && mState.buttonDown(id))
			buttonReleased |= (1 << i);
	}
	mState.buttons = mTempState.buttons;
	
	//Fire off events
	if (mListener && mBuffered) {
		MouseEvent mouseEvent(this, mState);
		if (mouseMoved)
			mListener->mouseMoved(mouseEvent);
		for (uint8_t i = 0; i < 8; ++i) {
			MouseButtonID id = static_cast<MouseButtonID>(i);
			if (buttonPressed & (1 << i))
				mListener->mousePressed(mouseEvent, id);
			else if (buttonReleased & (1 << i))
				mListener->mouseReleased(mouseEvent, id);
		}
	}

	mTempState.clear();
}

//--------------------------------------------------------------------------------------------------//
void RemoteMouse::setBuffered(bool buffered)
{
	mBuffered = buffered;
}
		
bool RemoteMouse::understands(WindowDataResponse response)
{
	return mProtocol->canParseResponse(response);
}

void RemoteMouse::interpret(WindowDataResponse response)
{
	mTempState = static_cast<RemoteMouseProtocol*>(mProtocol)->parseResponse(response);
	{
		std::lock_guard<std::mutex> updateGuard(mUpdateLock);
		mUpdated = true;
	}
	mUpdateCv.notify_one();
}