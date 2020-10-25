#ifndef ASIO9_HTTPSESSION_HPP_
#define ASIO9_HTTPSESSION_HPP_

#include <memory>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "TcpSession.hpp"

namespace asio9 {
	class HttpSession;

	namespace basic_type {
		typedef boost::beast::http::request<boost::beast::http::string_body> req_type;
		typedef std::shared_ptr<req_type> req_ptr;
		typedef boost::beast::http::response<boost::beast::http::string_body> res_type;
		typedef std::shared_ptr<res_type> res_ptr;
	}

	class HttpSession : public std::enable_shared_from_this<HttpSession>
	{
	public:
		HttpSession(basic_type::io_type* io, basic_type::socket_type socket)
			: m_stream(std::move(socket))//初始化TCP流
		{
			this->m_io = io;
		};
		~HttpSession() {

		}

		//运行
		inline void run() { 
			this->init();
			this->m_io->dispatch(std::bind(&HttpSession::do_read, this->shared_from_this()));
		};

		//关闭连接
		inline void close() { this->do_close(); };

		//发送数据
		inline void write(basic_type::res_ptr& res) {
			boost::beast::http::async_write(
				this->m_stream,
				*res,
				std::bind(&HttpSession::after_write, this->shared_from_this(),
					this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		};

		//获取一些东西
		inline boost::beast::tcp_stream* getTcpStreamPtr() { return &this->m_stream; }
		inline basic_type::io_type* getIo_context() { return this->m_io; };

		//设置超时时间
		inline void setExpiryTime(const std::chrono::nanoseconds& expiry_time) { this->m_stream.expires_after(expiry_time); };

		virtual void init(){}
		virtual void on_message(std::shared_ptr<HttpSession> session, const basic_type::ec_type& ec, size_t bytes_transferred, basic_type::req_ptr req){}
		virtual void after_write(std::shared_ptr<HttpSession> session, const basic_type::ec_type& ec, const size_t& size){}

	private:
		//读取请求
		void do_read() {
			//初始化请求
			basic_type::req_ptr req = std::make_shared<basic_type::req_type>();

			//读取请求
			boost::beast::http::async_read(this->m_stream, this->m_buffer, *req,
				std::bind(&HttpSession::read_handler, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2, req));
		}
		//关闭连接
		inline void do_close() { this->m_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send); };


		void read_handler(basic_type::ec_type ec, size_t bytes_transferred, basic_type::req_ptr req)
		{
			if (!ec) {
				this->on_message(this->shared_from_this(), ec, bytes_transferred, req);

				//读取下一个请求
				do_read();
			}
		}

		basic_type::io_type* m_io = nullptr;
		boost::beast::tcp_stream m_stream;
		boost::beast::flat_buffer m_buffer;
	};
}

#endif