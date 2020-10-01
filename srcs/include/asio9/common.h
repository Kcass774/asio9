#ifndef ASIO9_COMMON_H_
#define ASIO9_COMMON_H_

#include <memory>

#include <boost/asio.hpp>

namespace asio9 {
	namespace basic_type {
		typedef boost::asio::io_context io_type;
		typedef boost::asio::ip::tcp::socket socket_type;
		typedef std::shared_ptr<socket_type> sock_ptr;
		typedef boost::asio::ip::tcp::endpoint endpoint_type;
		typedef boost::system::error_code ec_type;
		typedef boost::asio::ip::tcp::acceptor acceptor_type;
		typedef boost::asio::ip::address address_type;
	}
}

#endif

