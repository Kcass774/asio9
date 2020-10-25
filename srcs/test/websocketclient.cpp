#include <iostream>
#include <memory>
#include <string_view>
#include <string>

#include <asio9/WSClient.hpp>

using std::cerr;
using std::cout;
using std::endl;
using std::string;


asio9::basic_type::io_type io;
asio9::basic_type::io_type::work work(io);

class customclient : public asio9::WebSocketClient {
public:
	customclient(asio9::basic_type::io_type* io)
		:WebSocketClient(io)
	{

	}

	void on_connected(std::shared_ptr<WebSocketClient> this_ptr, const asio9::basic_type::ec_type& ec) {
		cout << "���ӳɹ�" << endl;
		this->getWSStreamPtr()->text(true);
		this->write("NihaoShijie", 11);
	}

	void on_close(std::shared_ptr<WebSocketSession> this_ptr) {
		cout << "���Ӷ�ʧ" << endl;
	}

	void on_message(std::shared_ptr<WebSocketSession> this_ptr, const char* data, const size_t& size)
	{
		cout << "��������:" << std::string_view(data, size) << endl;
		this->write("NihaoShijie", 11);
	}

	void after_write(std::shared_ptr<WebSocketSession> this_ptr, const asio9::basic_type::ec_type& ec, const size_t& size)
	{
		cout << "�Ѵ���" << size << "Bytes" << endl;
	}
};

void CreateClient() {
	boost::pool<> pool(sizeof(char) * 4000);

	auto client = std::make_shared<customclient>(&io);
	asio9::basic_type::endpoint_type endpoint(boost::asio::ip::make_address("127.0.0.1"), 8880);
	client->run(endpoint, "localhost", "/");
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