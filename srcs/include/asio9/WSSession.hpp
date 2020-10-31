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
		typedef boost::beast::websocket::stream<boost::beast::tcp_stream> websocket_stream;

		WebSocketSession(basic_type::io_type* io, basic_type::socket_type socket)
			: m_ws(std::move(socket))//��ʼ��WebSocket��
		{
			this->m_io = io;
		};
		~WebSocketSession() {

		}

		//����
		inline void run() {
			this->m_io->dispatch(std::bind(&WebSocketSession::do_run, this->shared_from_this()));
		};

		//�ر�����
		inline void close() { this->m_io->post(std::bind(&WebSocketSession::do_close, this)); };

		//��������
		inline void write(const char* data, const size_t size) {
			this->m_ws.async_write(
				boost::asio::buffer(data, size),
				std::bind(&WebSocketSession::on_write, this->shared_from_this(),
					std::placeholders::_1, std::placeholders::_2));
		};

		//��ȡ����
		void do_read() {
			this->m_ws.async_read(
				this->m_buffer,
				std::bind(&WebSocketSession::read_handler, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		}

		//��ȡws_stream
		inline websocket_stream* get_websocket_stream() { return &this->m_ws; }
		//��ȡexecutor
		inline basic_type::io_type::executor_type get_executor() { return this->m_io->get_executor(); };

		virtual void on_accept(const basic_type::ec_type& ec) {}
		virtual void on_message(const basic_type::ec_type& ec, const char* data, const size_t& size) {}
		virtual void on_close(const basic_type::ec_type& ec) {};
		virtual void on_write(const basic_type::ec_type& ec, const size_t& size) {}

	private:
		void do_run() {
			//�����Ƽ���ʱʱ��
			this->m_ws.set_option(
				boost::beast::websocket::stream_base::timeout::suggested(
					boost::beast::role_type::server));
			//�������ְ�
			m_ws.async_accept(std::bind(&WebSocketSession::accept_handler, this->shared_from_this(), std::placeholders::_1));
		}

		void accept_handler(basic_type::ec_type ec)
		{
			this->on_accept(ec);

			//��ʼ��ȡ
			do_read();
		}

		//�ر�����
		inline void do_close() {  };

		void read_handler(basic_type::ec_type ec, size_t bytes_transferred)
		{

			if (!ec) {
				this->on_message(ec, (char*)this->m_buffer.data().data(), bytes_transferred);
				this->do_read();
			}
			else {
				this->on_message(ec, nullptr, 0);
			}
		}

		basic_type::io_type* m_io = nullptr;
		websocket_stream m_ws;
		boost::beast::flat_buffer m_buffer;
	};
}

#endif