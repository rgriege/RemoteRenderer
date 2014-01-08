#ifndef _DEFAULT_REMOTE_DEVICE_PROTOCOL	
#define _DEFAULT_REMOTE_DEVICE_PROTOCOL

#include <sstream>
#include <vector>
#include <cassert>

#include "remote/RemotePrereqs.h"
#include "remote/RemoteMouse.h"

namespace OIS
{
	class DefaultRemoteMouseProtocol : public RemoteMouseProtocol
	{
		virtual WindowDataRequest createCaptureRequest(Type iType) const
		{
			WindowDataRequest request = { calloc(2, 1), 2 };
			static_cast<char*>(request.data)[0] = 'M';
			return request;
		}

		virtual bool canParseResponse(WindowDataResponse response) const
		{
			return static_cast<const char*>(response.data)[0] == 'M';
		}

		virtual MouseState parseResponse(WindowDataResponse response) const
		{
			if (!canParseResponse(response))
				throw std::invalid_argument("Can't parse this response.");

			std::string msg(static_cast<const char*>(response.data)+1,
				static_cast<const char*>(response.data) + response.len);
			std::stringstream sstream(msg);
			std::vector<std::string> args;
			bool done = false;
			while (!done) {
				std::string s;
				std::getline(sstream, s, ' ');
				args.push_back(s);
				done = sstream.eof();
			}

			assert (args.size() != 0);
			
			MouseState state;
			state.width = atoi(args[0].c_str());
			state.height = atoi(args[1].c_str());
			state.X.abs = atoi(args[2].c_str());
			state.Y.abs = atoi(args[3].c_str());
			state.buttons = atoi(args[4].c_str());
			return state;
		}

		virtual Type deviceType() const { return OISMouse; }
	};
}

#endif