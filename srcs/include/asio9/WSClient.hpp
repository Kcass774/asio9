#ifndef AISO9_WSCLIENT_HPP
#define AISO9_WSCLIENT_HPP

#include "WSSession.hpp"

namespace asio9 {
	class WebSocketClient : public WebSocketSession
	{
	public:
		WebSocketClient(asio9::basic_type::io_type* io)
			:WebSocketSession(io, std::move(basic_type::socket_type(*io)))
		{
		}

		void run(const asio9::basic_type::endpoint_type& endpoint, const std::string& host, const std::string& path) {
			this->getIo_context()->dispatch(
				std::bind(&WebSocketClient::do_connect, std::dynamic_pointer_cast<WebSocketClient>(WebSocketSession::shared_from_this())
				, endpoint, host, path));
		}

		void run(const std::string& host, const std::string& path) {
		}

		void run() = delete;

		virtual void on_connected(std::shared_ptr<WebSocketClient> this_ptr, const basic_type::ec_type& ec) {}
	private:

		void do_connect(const asio9::basic_type::endpoint_type& endpoint, const std::string& host, const std::string& path) {
			this->getTcpStreamPtr()->async_connect(
				endpoint,
				std::bind(&WebSocketClient::conn_handler, std::dynamic_pointer_cast<WebSocketClient>(WebSocketSession::shared_from_this())
					, std::placeholders::_1, host, path));
		};

		void conn_handler(const basic_type::ec_type& ec, const std::string& host, const std::string& path)
		{
			this->getWSStreamPtr()->async_handshake(host, path,
				std::bind(&WebSocketClient::handshake_handler, std::dynamic_pointer_cast<WebSocketClient>(WebSocketSession::shared_from_this())
					, std::placeholders::_1));
		}

		void handshake_handler(const basic_type::ec_type& ec)
		{
			this->on_connected(std::dynamic_pointer_cast<WebSocketClient>(WebSocketSession::shared_from_this()), ec);
			WebSocketSession::do_read();
		}
	};
}

#endif
