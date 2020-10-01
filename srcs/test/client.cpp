#include <iostream>
#include <memory>
#include <string_view>
#include <string>

#include <asio9/TcpClient.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;


asio9::basic_type::io_type io;
asio9::basic_type::io_type::work work(io);

void CreateClient() {
	boost::pool<> pool(sizeof(char) * 4000);

	auto client = std::make_shared<asio9::TcpClient>(&io);
	client->initBuffer(&pool);
	asio9::basic_type::endpoint_type endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8888);
	client->Connect(endpoint);
	client->setConnectCallBack([](asio9::TcpClient::client_ptr client, const asio9::basic_type::ec_type& ec) {
		if (ec)
			std::cerr << ec.message();
		else
			std::cout << "已连接到127.0.0.1:8888" << std::endl;
		});
	client->setMessageCallBack([](asio9::TcpClient::client_ptr client, const char* data, const size_t& size) {
		std::cout << "127.0.0.1:8888 >> " << std::string_view(data, size) << std::endl;
		});
	client->setCloseCallBack([](asio9::TcpClient::client_ptr client) {
		std::cout << "已断开127.0.0.1:8888" << std::endl;
		});
}

int main(int argc, char* argv[]) {
	io.post(&CreateClient);
	io.run();
	return 0;
}