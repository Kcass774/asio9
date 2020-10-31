#ifndef ASIO9_TCPCLIENT_HPP_
#define ASIO9_TCPCLIENT_HPP_

#include "TcpSession.hpp"

namespace asio9 {
	class TcpClient : public asio9::TcpSession
	{
	public:
		TcpClient(asio9::basic_type::io_type* io)
			:TcpSession(io, std::move(basic_type::socket_type(*io)))
		{
		}

		void run(const asio9::basic_type::endpoint_type& endpoint) {
			this->get_executor().context().dispatch(std::bind(&TcpClient::do_connect, std::dynamic_pointer_cast<TcpClient>(TcpSession::shared_from_this()), endpoint));
		}

		virtual void on_connected(const basic_type::ec_type& ec) {}

	private:
		void do_connect(const asio9::basic_type::endpoint_type& endpoint) {
			this->get_socket()->async_connect(endpoint,
				std::bind(&TcpClient::conn_handler, std::dynamic_pointer_cast<TcpClient>(TcpSession::shared_from_this()), std::placeholders::_1));
		};

		void conn_handler(const boost::system::error_code& ec)
		{
			this->on_connected(ec);
			TcpSession::run();
		}
	};
}

#endif