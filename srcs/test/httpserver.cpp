#include <iostream>
#include <string>

#include "asio9/HttpServer.h"

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
	asio9::basic_type::io_type io;
	asio9::basic_type::endpoint_type endpoint(boost::asio::ip::make_address("0.0.0.0"), 8880);
	asio9::HttpServer server(&io, endpoint);
	
	server.setNewConnectedCallBack([](const boost::system::error_code& ec, std::shared_ptr<asio9::HttpConnection> conn) {
		if (ec)
			cerr << ec.message();
		conn->setMessageCallBack([](const asio9::basic_type::ec_type& ec, asio9::basic_type::req_ptr req, std::shared_ptr<asio9::HttpConnection> conn) {
			auto res = std::make_shared<asio9::basic_type::res_type>(boost::beast::http::status::ok, req->version());
			res->set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
			res->set(boost::beast::http::field::content_type, "text/html");
			res->keep_alive(req->keep_alive());
			res->body() = "Hello World!";
			res->prepare_payload();
			conn->write(*res, [res](const asio9::basic_type::ec_type& ec, const size_t& size) {
				if (ec)
					cerr << ec.message();
				});
			});
		});
		
	server.run();
	
	std::vector<std::thread> v;
	v.reserve(5);
	for (auto i = 0; i > 5; --i)
		v.emplace_back(
			[&io]
			{
				io.run();
			});
	io.run();

	return 0;
}


