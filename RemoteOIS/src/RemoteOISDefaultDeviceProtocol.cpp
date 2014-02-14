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

#include "RemoteOISDefaultDeviceProtocol.h"

using namespace RemoteOIS;

//---------------------------------------------------------------------------//
WindowDataRequest DefaultMouseProtocol::createCaptureRequest() const
{
    WindowDataRequest request = { calloc(1, 1), 1 };
    reinterpret_cast<char*>(request.data)[0] = 'M';
    return request;
}

//---------------------------------------------------------------------------//
bool DefaultMouseProtocol::canParseResponse(WindowDataResponse response) const
{
    return reinterpret_cast<const char*>(response.data)[0] == 'M';
}

//---------------------------------------------------------------------------//
OIS::MouseState DefaultMouseProtocol::parseResponse(WindowDataResponse response) const
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

//---------------------------------------------------------------------------//
WindowDataRequest DefaultRemoteKeyboardProtocol::createCaptureRequest() const
{
    WindowDataRequest request = { calloc(1, 1), 1 };
    static_cast<char*>(request.data)[0] = 'K';
    return request;
}

//---------------------------------------------------------------------------//
bool DefaultRemoteKeyboardProtocol::canParseResponse(WindowDataResponse response) const
{
    return reinterpret_cast<const char*>(response.data)[0] == 'K';
}

//---------------------------------------------------------------------------//
void DefaultRemoteKeyboardProtocol::parseResponse(WindowDataResponse response,
                                                  char* buffer,
                                                  unsigned int& modifiers) const
{
    if (!canParseResponse(response))
        throw std::invalid_argument("Can't parse this response.");

    for (int i = 0; i < 256; ++i)
        buffer[i] = reinterpret_cast<const char*>(response.data)[i+1] - 48;

    modifiers = *reinterpret_cast<const unsigned int*>(
        reinterpret_cast<const char*>(response.data) + 257);
}