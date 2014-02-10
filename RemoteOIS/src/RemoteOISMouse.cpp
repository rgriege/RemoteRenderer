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
#include "RemoteOISMouse.h"
#include "RemoteOISInputManager.h"
#include "OISException.h"
#include "OISEvents.h"

using namespace RemoteOIS;

//--------------------------------------------------------------------------------------------------//
Mouse::Mouse( InputManager* creator, Connection* connection, bool buffered, DeviceProtocol* protocol )
	: OIS::Mouse(creator->inputSystemName(), buffered, 0, creator), mConnection(connection),
    mProtocol(protocol), mScalingInput(true)
{
	mConnection->addMessageListener(this);
	mState.width = 0;
	mState.height = 0;
}

//--------------------------------------------------------------------------------------------------//
void Mouse::_initialize()
{
	//Clear old state
	mState.clear();
	mTempState.clear();
}

//--------------------------------------------------------------------------------------------------//
Mouse::~Mouse()
{
	mConnection->removeMessageListener(this);
}

//--------------------------------------------------------------------------------------------------//
void Mouse::capture()
{
	//Clear old relative values
	mState.X.rel = mState.Y.rel = mState.Z.rel = 0;

	WindowDataRequest request = mProtocol->createCaptureRequest();
	{
		std::lock_guard<std::mutex> updateGuard(mUpdateLock);
		mUpdated = false;
	}
	mConnection->send(request);
	//wait on interpret to be called with matching response
	std::unique_lock<std::mutex> updateLk(mUpdateLock);
	mUpdateCv.wait(updateLk, [&]() { return mUpdated; });
	updateLk.unlock();

	bool mouseMoved = false;
	uint8_t buttonPressed = 0;
	uint8_t buttonReleased = 0;

    if (mScalingInput) {
        if(mState.width != mTempState.width)
            mWidthScale = ((float) mState.width) / mTempState.width;
        if(mState.height != mTempState.height)
            mHeightScale = ((float) mState.height) / mTempState.height;
    }

	if(mTempState.X.rel || mTempState.Y.rel || mTempState.Z.rel)
	{
		//printf("%i %i %i\n\n", mTempState.X.rel, mTempState.Y.rel, mTempState.Z.rel);

		//Set new relative motion values
        mState.X.rel = mScalingInput ? mTempState.X.rel * mWidthScale : mTempState.X.rel;
        mState.Y.rel = mScalingInput ? mTempState.Y.rel * mHeightScale : mTempState.Y.rel;
		mState.Z.rel = mTempState.Z.rel;
		
		//Update absolute position
		mState.X.abs += mState.X.rel;
		mState.Y.abs += mState.Y.rel;
		mState.Z.abs += mState.Z.rel;
		
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
		OIS::MouseButtonID id = static_cast<OIS::MouseButtonID>(i);
		if (mTempState.buttonDown(id) && !mState.buttonDown(id))
			buttonPressed |= (1 << i);
		else if (!mTempState.buttonDown(id) && mState.buttonDown(id))
			buttonReleased |= (1 << i);
	}
	mState.buttons = mTempState.buttons;
	
	//Fire off events
	if (mListener && mBuffered) {
		OIS::MouseEvent mouseEvent(this, mState);
		if (mouseMoved)
			mListener->mouseMoved(mouseEvent);
		for (uint8_t i = 0; i < 8; ++i) {
			OIS::MouseButtonID id = static_cast<OIS::MouseButtonID>(i);
			if (buttonPressed & (1 << i))
				mListener->mousePressed(mouseEvent, id);
			else if (buttonReleased & (1 << i))
				mListener->mouseReleased(mouseEvent, id);
		}
	}

	mTempState.clear();
}

//--------------------------------------------------------------------------------------------------//
void Mouse::setBuffered(bool buffered)
{
	mBuffered = buffered;
}
		
bool Mouse::understands(WindowDataResponse response)
{
	return mProtocol->canParseResponse(response);
}

void Mouse::interpret(WindowDataResponse response)
{
	mTempState = static_cast<MouseProtocol*>(mProtocol)->parseResponse(response);
	{
		std::lock_guard<std::mutex> updateGuard(mUpdateLock);
		mUpdated = true;
	}
	mUpdateCv.notify_one();
}

void Mouse::setScaling(bool scaling)
{
    mScalingInput = scaling;
}

OIS::MouseState Mouse::getRawMouseState() const
{
    OIS::MouseState rawState = mState;
    rawState.width /= mWidthScale;
    rawState.X.abs /= mWidthScale;
    rawState.X.rel /= mWidthScale;
    rawState.height /= mHeightScale;
    rawState.Y.abs /= mHeightScale;
    rawState.Y.rel /= mHeightScale;
    return rawState;
}