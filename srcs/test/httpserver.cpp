#include <iostream>
#include <chrono>
#include <string>

#include <asio9/HttpSession.hpp>
#include <asio9/TcpServer.hpp>
#include <asio9/HttpParse.hpp>

using std::cerr;
using std::cout;
using std::endl;



class session : public asio9::HttpSession {
public:
	session(asio9::basic_type::io_type* io, asio9::basic_type::socket_type socket)
		:HttpSession(io, std::move(socket))
	{

	}

	void on_message(const asio9::basic_type::ec_type& ec, size_t bytes_transferred, asio9::basic_type::req_ptr req)
	{
		auto start = std::chrono::system_clock::now();

		auto target = req->target();
		auto pTarget = asio9::parse_url(target);
		auto pVar = asio9::parse_urlvar(pTarget.second);

		auto res = std::make_shared<asio9::basic_type::res_type>(boost::beast::http::status::ok, req->version());
		res->set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
		res->set(boost::beast::http::field::content_type, "text/html");
		res->keep_alive(req->keep_alive());
		std::string body = "Target: ";
		body += pTarget.first;
		body += "\nVar:\n";
		for (auto iter = pVar.begin(); iter != pVar.end(); iter++) {
			body += '\t';
			body += iter->first;
			body += ": ";
			body += iter->second;
			body += '\n';
		}

		auto end = std::chrono::system_clock::now();
		body += "Time: ";
		body += std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
		body += " ms\n";

		res->body() = std::move(body);
		res->prepare_payload();
		this->write(res);
	}

	void on_write(asio9::basic_type::res_ptr res, const asio9::basic_type::ec_type& ec, const size_t& size)
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
		cout << "新连接:" << session->get_tcp_stream()->socket().remote_endpoint() << endl;
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


