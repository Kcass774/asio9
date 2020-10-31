#include <iostream>
#include <string>

#include <asio9/TcpServer.hpp>

using std::cerr;
using std::cout;
using std::endl;

asio9::basic_type::io_type io;
asio9::basic_type::io_type::work work(io);
boost::pool<> pool(sizeof(char) * 4000);
asio9::basic_type::endpoint_type endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8889);

class session : public asio9::TcpSession {
public:
	session(asio9::basic_type::io_type* io, asio9::basic_type::socket_type socket)
		:TcpSession(io, std::move(socket))
	{

	}

	void on_close() {
		cout << "连接丢失" << endl;
	}

	void on_readready(const asio9::basic_type::ec_type& ec, const size_t& size)
	{
		if (ec) {
			cerr << ec.message() << endl;
		}
		else {
			auto buf = this->get_streambuf();
			auto view = std::string_view((char*)buf->data().data(), buf->size());
			cout << "缓冲区:" << view << endl;
			if (buf->size() > 10)
				buf->consume(10);
			this->write("NihaoShijie", 11);
		}
	}

	void on_write(const asio9::basic_type::ec_type& ec, const size_t& size)
	{
		if (ec) {
			cerr << ec.message() << endl;
		}
		else {
			cout << "已传输" << size << "Bytes" << endl;
		}
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
			cerr << ec;
		else
			cout << "新连接:" << session->get_socket()->remote_endpoint() << endl;
	}
};

int main(int argc, char* argv[]) {
	auto server = std::make_shared<customserver>(&io, endpoint);
	server->run();
	try {
		io.run();
	}
	catch (boost::system::error_code ec) {
		cerr << ec.message() << endl;
	}
	return 0;
}
