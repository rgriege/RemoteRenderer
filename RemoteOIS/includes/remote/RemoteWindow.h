#include "OIS.h"
#include "remote/RemotePrereqs.h"
#include <inttypes.h>

namespace OIS
{
	class RemoteMouse : public Mouse, MessageListener
	{
	public:
		RemoteMouse(const std::string &vendor, bool buffered, int devID, InputManager* creator, RemoteConnection& connection, RemoteDeviceProtocol& protocol)
			: Mouse(vendor, buffered, devID, creator), mConnection(connection), mProtocol(protocol)
		{
		}
		virtual void setBuffered(bool buffered) { mBuffered = buffered; }
		virtual void capture()
		{
			WindowDataRequest request = mProtocol.createCaptureRequest(mType);
			mConnection.send(request.data, request.len);
			//wait on interpret to be called with matching response
			//parse response
		}
		virtual Interface* queryInterface(Interface::IType type) { return 0; }
		virtual void _initialize()
		{
			mConnection.addMessageListener(this);
		}
		virtual bool understands(WindowDataResponse) = 0;
		virtual void interpret(WindowDataResponse) = 0;
	protected:
		RemoteConnection& mConnection;
		RemoteDeviceProtocol& mProtocol;
	};
}