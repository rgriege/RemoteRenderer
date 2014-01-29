#ifndef _REMOTE_KEYBOARD_H
#define _REMOTE_KEYBOARD_H

#include <mutex>
#include <condition_variable>

#include "OISKeyboard.h"
#include "RemoteOISPrereqs.h"

namespace RemoteOIS
{
	class _RemoteOISExport KeyboardProtocol : public DeviceProtocol
	{
	public:
		virtual void parseResponse(WindowDataResponse, char*, unsigned int&) const = 0;
	};

	class _RemoteOISExport Keyboard : public OIS::Keyboard, public MessageListener
	{
	public:
		Keyboard(InputManager* creator, Connection* connection, bool buffered, DeviceProtocol* protocol);

		virtual ~Keyboard();

		virtual bool isKeyDown(OIS::KeyCode key) const;

		virtual const std::string& getAsString(OIS::KeyCode kc);

		virtual void copyKeyStates(char keys[256]) const;

		/** @copydoc OIS::Object::setBuffered */
		virtual void setBuffered(bool buffered);
		
		/** @copydoc OIS::Object::capture */
		virtual void capture();

		/** @copydoc OIS::Object::queryInterface */
		virtual OIS::Interface* queryInterface(OIS::Interface::IType type) {return 0;}
		
		/** @copydoc OIS::Object::_initialize */
		virtual void _initialize();
		
		virtual bool understands(WindowDataResponse);

		virtual void interpret(WindowDataResponse);

	protected:
		//! Internal method for translating KeyCodes to Text
		int _translateText( OIS::KeyCode kc );

		Connection* mConnection;

		DeviceProtocol* mProtocol;
		
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