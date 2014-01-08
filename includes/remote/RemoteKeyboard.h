#ifndef _REMOTE_KEYBOARD_H
#define _REMOTE_KEYBOARD_H

#include "OISKeyboard.h"
#include "remote/RemotePrereqs.h"

namespace OIS
{
	class RemoteKeyboard : public Keyboard
	{
	public:
		RemoteKeyboard(InputManager* creator, RemoteConnection* connection, bool buffered, RemoteDeviceProtocol* protocol);

		virtual bool isKeyDown(KeyCode key) const { return false; }

		virtual const std::string& getAsString(KeyCode kc) { return ""; }

		virtual void copyKeyStates(char keys[256]) const {}

		/** @copydoc Object::setBuffered */
		virtual void setBuffered(bool buffered) {}
		
		/** @copydoc Object::capture */
		virtual void capture() {}

		/** @copydoc Object::queryInterface */
		virtual Interface* queryInterface(Interface::IType type) {return 0;}
		
		/** @copydoc Object::_initialize */
		virtual void _initialize() {}
	};
}

#endif