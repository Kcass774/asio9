#include <iostream>
#include <string>

#include "asio9/TcpServer.h"

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
	asio9::basic_type::io_type io;
	asio9::basic_type::endpoint_type endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8889);
	asio9::TcpServer server(&io, endpoint);
	server.setReadBufferSize(1000);
	server.run();
	io.run();

	return 0;
}
