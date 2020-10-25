#include <iostream>
#include <string>

#include <asio9/HttpSession.hpp>
#include <asio9/TcpServer.hpp>

using std::cerr;
using std::cout;
using std::endl;



class session : public asio9::HttpSession {
public:
	session(asio9::basic_type::io_type* io, asio9::basic_type::socket_type socket)
		:HttpSession(io, std::move(socket))
	{

	}

	void on_message(std::shared_ptr<HttpSession> session, const asio9::basic_type::ec_type& ec, size_t bytes_transferred, asio9::basic_type::req_ptr req)
	{
		auto res = std::make_shared<asio9::basic_type::res_type>(boost::beast::http::status::ok, req->version());
		res->set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
		res->set(boost::beast::http::field::content_type, "text/html");
		res->keep_alive(req->keep_alive());
		res->body() = "Hello World!";
		res->prepare_payload();
		this->write(res);
	}

	void after_write(std::shared_ptr<HttpSession> session, const asio9::basic_type::ec_type& ec, const size_t& size)
	{
		//cout << "已传输" << size << "Bytes" << endl;
	}
};

class customserver : public asio9::TcpServer<session> {
public:
	customserver(asio9::basic_type::io_type* io, const asio9::basic_type::endpoint_type& endpoint)
		:TcpServer(io, endpoint)
	{
	}

	void on_accept(asio9::basic_type::ec_type ec, session_ptr session) {
		//cout << "新连接:" << session->getTcpStreamPtr()->socket().remote_endpoint() << endl;
	};
};

int main(int argc, char* argv[]) {
	asio9::basic_type::io_type io;
	asio9::basic_type::endpoint_type endpoint(boost::asio::ip::make_address("0.0.0.0"), 8880);

	auto server = std::make_shared<customserver>(&io, endpoint);
	server->run();
	
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


