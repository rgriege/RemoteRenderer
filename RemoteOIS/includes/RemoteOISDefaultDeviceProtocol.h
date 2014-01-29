#ifndef _DEFAULT_REMOTE_DEVICE_PROTOCOL	
#define _DEFAULT_REMOTE_DEVICE_PROTOCOL

#include <sstream>
#include <vector>
#include <cassert>

#include "RemoteOISPrereqs.h"
#include "RemoteOISMouse.h"
#include "RemoteOISKeyboard.h"

namespace RemoteOIS
{
	class _RemoteOISExport DefaultMouseProtocol : public MouseProtocol
	{
	public:
		DefaultMouseProtocol()
			: prevX(0), prevY(0) {}

		virtual WindowDataRequest createCaptureRequest() const
		{
			WindowDataRequest request = { calloc(1, 1), 1 };
			reinterpret_cast<char*>(request.data)[0] = 'M';
			return request;
		}

		virtual bool canParseResponse(WindowDataResponse response) const
		{
			return reinterpret_cast<const char*>(response.data)[0] == 'M';
		}

		virtual OIS::MouseState parseResponse(WindowDataResponse response) const
		{
			if (!canParseResponse(response))
				throw std::invalid_argument("Can't parse this response.");

			std::string msg(reinterpret_cast<const char*>(response.data)+1,
				reinterpret_cast<const char*>(response.data) + response.len);
			std::stringstream sstream(msg);
			std::vector<std::string> args;
			bool done = false;
			while (!done) {
				std::string s;
				getline(sstream, s, ' ');
				args.push_back(s);
				done = sstream.eof();
			}

			assert (args.size() != 0);
			
			OIS::MouseState state;
			state.width = atoi(args[0].c_str());
			state.height = atoi(args[1].c_str());
			state.X.abs = atoi(args[2].c_str());
			state.X.rel = state.X.abs - prevX;
			prevX = state.X.abs;
			state.Y.abs = atoi(args[3].c_str());
			state.Y.rel = state.Y.abs - prevY;
			prevY = state.Y.abs;
			state.buttons = atoi(args[4].c_str());
			return state;
		}

		virtual OIS::Type deviceType() const { return OIS::OISMouse; }

	private:
		mutable int prevX;
		mutable int prevY;
	};

	class _RemoteOISExport DefaultRemoteKeyboardProtocol : public KeyboardProtocol
	{
		virtual WindowDataRequest createCaptureRequest() const
		{
			WindowDataRequest request = { calloc(1, 1), 1 };
			static_cast<char*>(request.data)[0] = 'K';
			return request;
		}

		virtual bool canParseResponse(WindowDataResponse response) const
		{
			return reinterpret_cast<const char*>(response.data)[0] == 'K';
		}

		virtual void parseResponse(WindowDataResponse response, char* buffer, unsigned int& modifiers) const
		{
			if (!canParseResponse(response))
				throw std::invalid_argument("Can't parse this response.");

			for (int i = 0; i < 256; ++i) {
				buffer[i] = reinterpret_cast<const char*>(response.data)[i+1] - 48;
			}

			modifiers = *reinterpret_cast<const unsigned int*>(reinterpret_cast<const char*>(response.data) + 257);
		}

		virtual OIS::Type deviceType() const { return OIS::OISKeyboard; }
	};
}

#endif