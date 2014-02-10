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
#include "RemoteOISPrereqs.h"

namespace RemoteOIS
{
	class _RemoteOISExport MouseProtocol : public DeviceProtocol
	{
	public:
		virtual OIS::MouseState parseResponse(WindowDataResponse) const = 0;
	};

	class _RemoteOISExport Mouse : public OIS::Mouse, public MessageListener
	{
	public:
		Mouse( InputManager* creator, Connection* connection, bool buffered, DeviceProtocol* protocol);

		virtual ~Mouse();
		
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

        const OIS::MouseState& getRawMouseState() const;

	protected:
		Connection* mConnection;

		DeviceProtocol* mProtocol;
		
		OIS::MouseState mTempState;

		bool mUpdated;

		std::mutex mUpdateLock;

		std::condition_variable mUpdateCv;

        float mWidthScale, mHeightScale;
	};
}

#endif //_REMOTE_MOUSE_H_EADER_