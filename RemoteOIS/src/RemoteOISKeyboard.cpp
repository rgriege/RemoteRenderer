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

#include <sstream>
#include <deque>
#include <cstring>

#include "RemoteOISKeyboard.h"
#include "RemoteOISInputManager.h"
#include "OISException.h"
#include "OISEvents.h"

using namespace RemoteOIS;

//---------------------------------------------------------------------------//
Keyboard::Keyboard(InputManager* creator, Connection* connection,
                   bool buffered, DeviceProtocol* protocol)
                   : OIS::Keyboard(creator->inputSystemName(), buffered, 0, creator),
                   mConnection(connection), mProtocol(protocol)
{
    mConnection->addConnectionListener(this);
}

//---------------------------------------------------------------------------//
Keyboard::~Keyboard()
{
    mConnection->removeConnectionListener(this);
}

//---------------------------------------------------------------------------//
bool Keyboard::isKeyDown(OIS::KeyCode key) const
{
    return KeyBuffer[key] != 0 ? true : false;
}

//---------------------------------------------------------------------------//
const std::string& Keyboard::getAsString(OIS::KeyCode kc)
{
    std::stringstream ss;
    ss << "Key_" << (int)kc;
    return mGetString.assign(ss.str());
}

//---------------------------------------------------------------------------//
void Keyboard::copyKeyStates(char keys[256]) const
{
    for(int i = 0; i < 256; ++i)
        keys[i] = KeyBuffer[i] > 0;
}

//---------------------------------------------------------------------------//
void Keyboard::setBuffered(bool buffered)
{
    mBuffered = buffered;
}

//---------------------------------------------------------------------------//	
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

//---------------------------------------------------------------------------//
void Keyboard::_initialize()
{
    //Clear our keyboard state buffer
    memset( &KeyBuffer, 0, 256 );
}

//---------------------------------------------------------------------------//
bool Keyboard::understands(WindowDataResponse response)
{
    return mProtocol->canParseResponse(response);
}

//---------------------------------------------------------------------------//
void Keyboard::interpret(WindowDataResponse response)
{
    static_cast<KeyboardProtocol*>(mProtocol)->parseResponse(response, TempKeyBuffer, mTempModifiers);
    {
        std::lock_guard<std::mutex> updateGuard(mUpdateLock);
        mUpdated = true;
    }
    mUpdateCv.notify_one();
}

//---------------------------------------------------------------------------//
int Keyboard::_translateText(OIS::KeyCode kc)
{
    if(mTextMode == Off)
        return 0;
    else
        return -1;
}
