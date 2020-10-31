#include <iostream>
#include <memory>
#include <string_view>
#include <string>

#include <asio9/TcpClient.hpp>

using std::cerr;
using std::cout;
using std::endl;
using std::string;


asio9::basic_type::io_type io;
asio9::basic_type::io_type::work work(io);

class customclient : public asio9::TcpClient {
public:
	customclient(asio9::basic_type::io_type* io)
		:TcpClient(io)
	{

	}

	void on_connected(const asio9::basic_type::ec_type& ec) {
		if (ec) {
			cerr << ec.message() << endl;
		}
		else {
			cout << "连接成功" << endl;
		}
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

void CreateClient() {
	boost::pool<> pool(sizeof(char) * 4000);

	auto client = std::make_shared<customclient>(&io);
	asio9::basic_type::endpoint_type endpoint(boost::asio::ip::make_address("127.0.0.1"), 8888);
	client->run(endpoint);
}

int main(int argc, char* argv[]) {
	io.post(&CreateClient);
	try {
		io.run();
	}
	catch (boost::system::error_code ec) {
		cerr << ec.message() << endl;
	}
	return 0;
}