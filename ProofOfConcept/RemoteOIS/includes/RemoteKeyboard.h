#ifndef _REMOTE_KEYBOARD_H
#define _REMOTE_KEYBOARD_H

#include <mutex>
#include <condition_variable>

#include "OISKeyboard.h"
#include "RemotePrereqs.h"

namespace OIS
{
	class RemoteKeyboardProtocol : public RemoteDeviceProtocol
	{
	public:
		virtual void parseResponse(WindowDataResponse, char*, unsigned int&) const = 0;
	};

	class RemoteKeyboard : public Keyboard, public MessageListener
	{
	public:
		RemoteKeyboard(InputManager* creator, RemoteConnection* connection, bool buffered, RemoteDeviceProtocol* protocol);

		virtual ~RemoteKeyboard();

		virtual bool isKeyDown(KeyCode key) const;

		virtual const std::string& getAsString(KeyCode kc);

		virtual void copyKeyStates(char keys[256]) const;

		/** @copydoc Object::setBuffered */
		virtual void setBuffered(bool buffered);
		
		/** @copydoc Object::capture */
		virtual void capture();

		/** @copydoc Object::queryInterface */
		virtual Interface* queryInterface(Interface::IType type) {return 0;}
		
		/** @copydoc Object::_initialize */
		virtual void _initialize();
		
		virtual bool understands(WindowDataResponse);

		virtual void interpret(WindowDataResponse);

	protected:
		//! Internal method for translating KeyCodes to Text
		int _translateText( KeyCode kc );

		RemoteConnection* mConnection;

		RemoteDeviceProtocol* mProtocol;
		
		char KeyBuffer[256];

		char TempKeyBuffer[256];

		unsigned int mTempModifiers;

		bool mUpdated;

		std::mutex mUpdateLock;

		std::condition_variable mUpdateCv;
		
		std::string mGetString;
	};
}

#endif