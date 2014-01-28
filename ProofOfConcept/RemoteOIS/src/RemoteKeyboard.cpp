#include <sstream>
#include <deque>

#include "remote/RemoteKeyboard.h"
#include "remote/RemoteInputManager.h"
#include "OISException.h"
#include "OISEvents.h"

using namespace OIS;

//--------------------------------------------------------------------------------------------------//
RemoteKeyboard::RemoteKeyboard(InputManager* creator, RemoteConnection* connection, bool buffered, RemoteDeviceProtocol* protocol)
	: Keyboard(creator->inputSystemName(), buffered, 0, creator), mConnection(connection), mProtocol(protocol)
{
	mConnection->addMessageListener(this);

	//Clear our keyboard state buffer
	memset( &KeyBuffer, 0, 256 );
}

//--------------------------------------------------------------------------------------------------//
RemoteKeyboard::~RemoteKeyboard()
{
	mConnection->removeMessageListener(this);
}

//--------------------------------------------------------------------------------------------------//
bool RemoteKeyboard::isKeyDown(KeyCode key) const
{
	return KeyBuffer[key] != 0 ? true : false;
}

//--------------------------------------------------------------------------------------------------//
const std::string& RemoteKeyboard::getAsString(KeyCode kc)
{
	std::stringstream ss;
	ss << "Key_" << (int)kc;
	return mGetString.assign(ss.str());
}

//--------------------------------------------------------------------------------------------------//
void RemoteKeyboard::copyKeyStates(char keys[256]) const
{
	for(int i = 0; i < 256; ++i)
		keys[i] = KeyBuffer[i] > 0;
}

//--------------------------------------------------------------------------------------------------//
void RemoteKeyboard::setBuffered(bool buffered)
{
	mBuffered = buffered;
}

//--------------------------------------------------------------------------------------------------//	
void RemoteKeyboard::capture()
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
			const KeyCode& kc = static_cast<KeyCode>(it->second);
			if (it->first)
				mListener->keyPressed(KeyEvent(this, kc, _translateText(kc)));
			else
				mListener->keyReleased(KeyEvent(this, kc, _translateText(kc)));
		}
	}
}

//--------------------------------------------------------------------------------------------------//
void RemoteKeyboard::_initialize()
{
}

//--------------------------------------------------------------------------------------------------//
bool RemoteKeyboard::understands(WindowDataResponse response)
{
	return mProtocol->canParseResponse(response);
}

//--------------------------------------------------------------------------------------------------//
void RemoteKeyboard::interpret(WindowDataResponse response)
{
	static_cast<RemoteKeyboardProtocol*>(mProtocol)->parseResponse(response, TempKeyBuffer, mTempModifiers);
	{
		std::lock_guard<std::mutex> updateGuard(mUpdateLock);
		mUpdated = true;
	}
	mUpdateCv.notify_one();
}

//--------------------------------------------------------------------------------------------------//
int RemoteKeyboard::_translateText(KeyCode kc)
{
	if( mTextMode == Off )
		return 0;
	else
		return -1;
}