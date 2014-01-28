#include <sstream>
#include <deque>

#include "RemoteOISKeyboard.h"
#include "RemoteOISInputManager.h"
#include "OISException.h"
#include "OISEvents.h"

using namespace RemoteOIS;

//--------------------------------------------------------------------------------------------------//
Keyboard::Keyboard(InputManager* creator, Connection* connection, bool buffered, DeviceProtocol* protocol)
	: OIS::Keyboard(creator->inputSystemName(), buffered, 0, creator), mConnection(connection), mProtocol(protocol)
{
	mConnection->addMessageListener(this);

	//Clear our keyboard state buffer
	memset( &KeyBuffer, 0, 256 );
}

//--------------------------------------------------------------------------------------------------//
Keyboard::~Keyboard()
{
	mConnection->removeMessageListener(this);
}

//--------------------------------------------------------------------------------------------------//
bool Keyboard::isKeyDown(OIS::KeyCode key) const
{
	return KeyBuffer[key] != 0 ? true : false;
}

//--------------------------------------------------------------------------------------------------//
const std::string& Keyboard::getAsString(OIS::KeyCode kc)
{
	std::stringstream ss;
	ss << "Key_" << (int)kc;
	return mGetString.assign(ss.str());
}

//--------------------------------------------------------------------------------------------------//
void Keyboard::copyKeyStates(char keys[256]) const
{
	for(int i = 0; i < 256; ++i)
		keys[i] = KeyBuffer[i] > 0;
}

//--------------------------------------------------------------------------------------------------//
void Keyboard::setBuffered(bool buffered)
{
	mBuffered = buffered;
}

//--------------------------------------------------------------------------------------------------//	
void Keyboard::capture()
{
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
	
	std::deque<std::pair<uint8_t, char> > changes;
	for(int i = 0; i < 256; ++i) {
		if (KeyBuffer[i] != TempKeyBuffer[i])
			changes.push_back(std::make_pair(TempKeyBuffer[i], i));
		KeyBuffer[i] = TempKeyBuffer[i];
	}
	mModifiers = mTempModifiers;

	if (mListener && mBuffered) {
		std::deque<std::pair<uint8_t, char> >::iterator it = changes.begin(),
			end = changes.end();
		for ( ; it != end; ++it) {
			const OIS::KeyCode& kc = static_cast<OIS::KeyCode>(it->second);
			if (it->first)
				mListener->keyPressed(OIS::KeyEvent(this, kc, _translateText(kc)));
			else
				mListener->keyReleased(OIS::KeyEvent(this, kc, _translateText(kc)));
		}
	}
}

//--------------------------------------------------------------------------------------------------//
void Keyboard::_initialize()
{
}

//--------------------------------------------------------------------------------------------------//
bool Keyboard::understands(WindowDataResponse response)
{
	return mProtocol->canParseResponse(response);
}

//--------------------------------------------------------------------------------------------------//
void Keyboard::interpret(WindowDataResponse response)
{
	static_cast<KeyboardProtocol*>(mProtocol)->parseResponse(response, TempKeyBuffer, mTempModifiers);
	{
		std::lock_guard<std::mutex> updateGuard(mUpdateLock);
		mUpdated = true;
	}
	mUpdateCv.notify_one();
}

//--------------------------------------------------------------------------------------------------//
int Keyboard::_translateText(OIS::KeyCode kc)
{
	if( mTextMode == Off )
		return 0;
	else
		return -1;
}