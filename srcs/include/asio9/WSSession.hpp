#ifndef ASIO9_WSSESSION_HPP_
#define ASIO9_WSSESSION_HPP_

#include <memory>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "TcpSession.hpp"

namespace asio9 {
	class WebSocketSession;

	class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
	{
	public:
		WebSocketSession(basic_type::io_type* io, basic_type::socket_type socket)
			: m_ws(std::move(socket))//初始化TCP流
		{
			this->m_io = io;
			this->m_socket = &socket;
		};
		~WebSocketSession() {

		}

		//运行
		inline void run() {
			this->init();
			this->m_io->dispatch(std::bind(&WebSocketSession::do_run, this->shared_from_this()));
		};

		//关闭连接
		inline void close() { this->m_io->post(std::bind(&WebSocketSession::do_close, this)); };

		//发送数据
		inline void write(const char* data, const size_t size) {
			this->m_ws.async_write(
				boost::asio::buffer(data, size),
				std::bind(&WebSocketSession::after_write, this->shared_from_this(),
					this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		};

		//获取一些东西
		inline boost::beast::websocket::stream<boost::beast::tcp_stream>* getWSStreamPtr() { return &this->m_ws; }
		inline basic_type::socket_type* getSocketPtr() { return this->m_socket; }
		inline basic_type::io_type* getIo_context() { return this->m_io; };

		virtual void init() {}
		virtual void on_accept(std::shared_ptr<WebSocketSession> session, const basic_type::ec_type& ec) {}
		virtual void on_message(std::shared_ptr<WebSocketSession> session, const char* data, const size_t& size) {}
		virtual void after_write(std::shared_ptr<WebSocketSession> session, const basic_type::ec_type& ec, const size_t& size) {}

	private:
		void do_run() {
			//设置推荐超时时间
			this->m_ws.set_option(
				boost::beast::websocket::stream_base::timeout::suggested(
					boost::beast::role_type::server));
			//接收握手包
			m_ws.async_accept(std::bind(&WebSocketSession::accept_handler, this->shared_from_this(), std::placeholders::_1));
		}

		void accept_handler(basic_type::ec_type ec)
		{
			this->on_accept(this->shared_from_this(), ec);

			//开始读取
			do_read();
		}

		//读取请求
		void do_read() {
			this->m_ws.async_read(
				this->m_buffer,
				std::bind(&WebSocketSession::read_handler, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		}

		//关闭连接
		inline void do_close() {  };

		void read_handler(basic_type::ec_type ec, size_t bytes_transferred)
		{

			//连接关闭
			if (ec == boost::beast::websocket::error::closed)
				return;

			if (this->m_ws.got_text())
				this->on_message(this->shared_from_this(), (char*)this->m_buffer.data().data(), bytes_transferred);
			else
				this->on_message(this->shared_from_this(), nullptr, bytes_transferred);

			this->do_read();
		}

		basic_type::io_type* m_io = nullptr;
		basic_type::socket_type* m_socket = nullptr;
		boost::beast::websocket::stream<boost::beast::tcp_stream> m_ws;
		boost::beast::flat_buffer m_buffer;
	};
}

#endif