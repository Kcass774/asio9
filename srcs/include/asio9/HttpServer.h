#ifndef ASIO9_HTTPSERVER_H_
#define ASIO9_HTTPSERVER_H_

#include <memory>
#include <set>

#include <boost/beast.hpp>

#include "common.h"

namespace asio9 {
	class HttpConnection;

	namespace basic_type {
		typedef boost::beast::http::request<boost::beast::http::string_body> req_type;
		typedef std::shared_ptr<req_type> req_ptr;
		typedef boost::beast::http::response<boost::beast::http::string_body> res_type;
		typedef std::shared_ptr<res_type> res_ptr;
	}

	namespace {
		typedef HttpConnection connection_type;
		typedef std::shared_ptr<connection_type> connection_ptr;
		typedef std::function<void(const basic_type::ec_type& ec, const size_t& size)> WriteCallBack;
		typedef std::function<void(const basic_type::ec_type& ec, basic_type::req_ptr req, connection_ptr conn)> MessageCallBack;
	}

	class HttpConnection
		: public std::enable_shared_from_this<HttpConnection>
	{
	public:
		HttpConnection(basic_type::socket_type socket, basic_type::io_type* io)
			: m_stream(std::move(socket))//初始化TCP流
		{
			this->m_io = io;
		};
		~HttpConnection() {

		}

		//运行
		inline void run() { this->m_io->dispatch(std::bind(&HttpConnection::do_read, this->shared_from_this())); };

		//关闭连接
		inline void close() { this->do_close(); };

		//发送数据
		inline void write(basic_type::res_type& res, WriteCallBack callback) {
			boost::beast::http::async_write(
				this->m_stream,
				res,
				std::bind(&HttpConnection::write_handler, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2, callback));
		};

		//设置超时时间
		inline void setExpiryTime(const std::chrono::nanoseconds& expiry_time) { this->m_stream.expires_after(expiry_time); };

		//消息回调
		inline void setMessageCallBack(MessageCallBack callback) { this->m_MessageCallBack = callback; };

	private:
		//读取请求
		inline void do_read() {
			//初始化请求
			basic_type::req_ptr req = std::make_shared<basic_type::req_type>();

			//读取请求
			boost::beast::http::async_read(this->m_stream, this->m_buffer, *req,
				std::bind(&HttpConnection::read_handler, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2, req));
		}
		//关闭连接
		inline void do_close() { this->m_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send); };


		void read_handler(basic_type::ec_type ec, size_t bytes_transferred, basic_type::req_ptr req);
		void write_handler(basic_type::ec_type ec, size_t bytes_transferred, WriteCallBack callback);

		basic_type::io_type* m_io = nullptr;
		boost::beast::tcp_stream m_stream;
		boost::beast::flat_buffer m_buffer;

		MessageCallBack m_MessageCallBack;
	};

	namespace {
		typedef std::function<void(const boost::system::error_code&, std::shared_ptr<HttpConnection> conn)> NewConnectedCallBack;
	}

	class HttpServer
		: public std::enable_shared_from_this<HttpServer>
	{
	public:
		HttpServer(basic_type::io_type* io, const basic_type::endpoint_type& endpoint);

		inline void run() { this->do_accept(); };

		inline void setNewConnectedCallBack(NewConnectedCallBack callback) { this->m_NewConnectedCallBack = callback; };

	private:
		inline void do_accept() {
			this->m_acceptor.async_accept(
				boost::asio::make_strand(*this->m_io),
				boost::beast::bind_front_handler(
					&HttpServer::on_accept,
					this));
		};

		void on_accept(boost::beast::error_code ec, basic_type::socket_type socket);

		basic_type::io_type* m_io = nullptr;
		basic_type::acceptor_type m_acceptor;
		std::set<HttpConnection> m_connections;

		NewConnectedCallBack m_NewConnectedCallBack = nullptr;

	};
}

#endif // !ASIO9_HTTPSERVER_H_
