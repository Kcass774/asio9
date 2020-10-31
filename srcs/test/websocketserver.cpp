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

	void on_accept(const asio9::basic_type::ec_type& ec) {
		if (ec)
			cerr << ec.message() << endl;
	};

	void on_message(const asio9::basic_type::ec_type& ec, const char* data, const size_t& size)
	{
		if (ec)
			cerr << ec.message() << endl;
		else {
			cout << "接收数据:" << std::string_view(data, size) << endl;
			this->write("NihaoShijie", 11);
		}
	}

	void on_write(const asio9::basic_type::ec_type& ec, const size_t& size)
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
		if (ec)
			cerr << ec.message() << endl;
		cout << "新连接:" << session->get_websocket_stream()->next_layer().socket().remote_endpoint() << endl;
	};
};

int main(int argc, char* argv[]) {
	asio9::basic_type::io_type io;
	asio9::basic_type::endpoint_type endpoint(boost::asio::ip::make_address("0.0.0.0"), 8881);

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