#include <iostream>
#include <string>

#include <asio9/TcpServer.hpp>
#include <asio9/WSSession.hpp>

using std::cerr;
using std::cout;
using std::endl;

class session : public asio9::WebSocketSession {
public:
	session(asio9::basic_type::io_type* io, asio9::basic_type::socket_type socket)
		:WebSocketSession(io, std::move(socket))
	{

	}

	void on_accept(std::shared_ptr<WebSocketSession> session, asio9::basic_type::ec_type ec) {
		if (ec)
			cerr << ec.message() << endl;
	};

	void on_message(std::shared_ptr<WebSocketSession> session, const char* data, const size_t& size)
	{
		cout << "接收数据:" << std::string_view(data, size) << endl;

		this->write("NihaoShijie", 11);
	}

	void after_write(std::shared_ptr<WebSocketSession> session, const asio9::basic_type::ec_type& ec, const size_t& size)
	{
		cout << "已传输" << size << "Bytes" << endl;
	}
};

class customserver : public asio9::TcpServer<session> {
public:
	customserver(asio9::basic_type::io_type* io, const asio9::basic_type::endpoint_type& endpoint)
		:TcpServer(io, endpoint)
	{
	}

	void on_accept(asio9::basic_type::ec_type ec, session_ptr session) {
		//cout << "新连接:" << session->getSocketPtr()->remote_endpoint() << endl;
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