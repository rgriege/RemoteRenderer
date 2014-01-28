#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include <condition_variable>
#include <mutex>

#include "RemotePrereqs.h"

using websocketpp::connection_hdl;

namespace OIS
{
	class WebSocketppConnection : public RemoteConnection
	{
		typedef websocketpp::endpoint<websocketpp::connection<websocketpp::config::asio>, 
			websocketpp::config::asio>::connection_ptr connection_ptr;
		typedef websocketpp::server<websocketpp::config::asio>::message_ptr message_ptr;
		typedef std::set<MessageListener*> message_listener_set;

	public:
		WebSocketppConnection(connection_ptr connection);

		virtual void open() {}
		/*virtual void addOpenListener();
		virtual void removeOpenListener();*/
		virtual void send(WindowDataRequest) const;
		virtual void addMessageListener(MessageListener*);
		virtual void removeMessageListener(MessageListener*) {}
		/*virtual void addCloseListener();
		virtual void removeCloseListener();*/
		virtual void close() {}
		virtual std::string name() const { return ""; }

	protected:
		virtual void _onmessage(connection_hdl hdl, message_ptr msg);

		std::set<MessageListener*> mMessageListeners;
		connection_ptr mConnection;
		std::condition_variable cv;
	};
}