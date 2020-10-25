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

	void on_connected(std::shared_ptr<TcpSession> this_ptr, const asio9::basic_type::ec_type& ec) {
		cout << "连接成功" << endl;
	}

	void on_close(std::shared_ptr<TcpSession> this_ptr) {
		cout << "连接丢失" << endl;
	}

	void on_message(std::shared_ptr<TcpSession> this_ptr, const char* data, const size_t& size)
	{
		cout << "接收数据:" << std::string_view(data, size) << endl;
		this->write("NihaoShijie", 11);
	}

	void after_write(std::shared_ptr<TcpSession> this_ptr, const asio9::basic_type::ec_type& ec, const size_t& size)
	{
		//cout << "已传输" << size << "Bytes" << endl;
	}
};

void CreateClient() {
	boost::pool<> pool(sizeof(char) * 4000);

	auto client = std::make_shared<customclient>(&io);
	asio9::basic_type::endpoint_type endpoint(boost::asio::ip::make_address("127.0.0.1"), 8888);
	client->initBuffer(4096);
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