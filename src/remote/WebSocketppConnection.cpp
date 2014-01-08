#include "remote/WebSocketppConnection.h"

using namespace OIS;

WebSocketppConnection::WebSocketppConnection(connection_ptr connection)
	: mConnection(connection)
{
	mConnection->set_message_handler(boost::bind(&WebSocketppConnection::_onmessage, this, _1, _2));
}

void WebSocketppConnection::send(WindowDataRequest request) const
{
	mConnection->send(request.data, request.len, websocketpp::frame::opcode::binary);
}

void WebSocketppConnection::addMessageListener(MessageListener* listener)
{
	mMessageListeners.insert(listener);
}

void WebSocketppConnection::_onmessage(connection_hdl hdl, message_ptr msg)
{
	WindowDataResponse response = { 
		reinterpret_cast<const void*>(msg->get_raw_payload().c_str()),
		msg->get_payload().length()
	};
	message_listener_set::iterator it = mMessageListeners.begin(), end = mMessageListeners.end();
	for ( ; it != end; ++it) {
		MessageListener* const& listener = *it;
		if (listener->understands(response))
			listener->interpret(response);
	}
}