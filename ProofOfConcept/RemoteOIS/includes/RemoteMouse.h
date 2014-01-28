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
#ifndef _REMOTE_MOUSE_H_EADER_
#define _REMOTE_MOUSE_H_EADER_

#include <condition_variable>
#include <mutex>

#include "OISMouse.h"
#include "remote/RemotePrereqs.h"

namespace OIS
{
	class RemoteMouseProtocol : public RemoteDeviceProtocol
	{
	public:
		virtual MouseState parseResponse(WindowDataResponse) const = 0;
	};

	class RemoteMouse : public Mouse, public MessageListener
	{
	public:
		RemoteMouse( InputManager* creator, RemoteConnection* connection, bool buffered, RemoteDeviceProtocol* protocol);

		virtual ~RemoteMouse();
		
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
		RemoteConnection* mConnection;

		RemoteDeviceProtocol* mProtocol;
		
		MouseState mTempState;

		bool mUpdated;

		std::mutex mUpdateLock;

		std::condition_variable mUpdateCv;
	};
}

#endif //_REMOTE_MOUSE_H_EADER_