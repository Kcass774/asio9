#ifndef ASIO9_TCPSESSION_HPP_
#define ASIO9_TCPSESSION_HPP_

#include <memory>

#include <boost/asio.hpp>

namespace asio9 {
	class TcpSession;

	namespace basic_type {
		typedef boost::asio::io_context io_type;
		typedef boost::asio::ip::tcp::socket socket_type;
		typedef std::shared_ptr<socket_type> sock_ptr;
		typedef boost::asio::ip::tcp::endpoint endpoint_type;
		typedef boost::system::error_code ec_type;
		typedef boost::asio::ip::tcp::acceptor acceptor_type;
		typedef boost::asio::ip::address address_type;
	}

	class TcpSession : public std::enable_shared_from_this<TcpSession>
	{
	public:
		TcpSession(basic_type::io_type* io, basic_type::socket_type socket)
			:m_socket(std::move(socket))
		{
			this->m_io = io;
		}

		~TcpSession()
		{
		}

		//����
		inline void run() 
		{
			this->m_io->dispatch(std::bind(&TcpSession::do_read, this->shared_from_this()));
		}

		//�ر�����
		inline void close() {
			this->m_io->post([this]() {
				this->m_socket.close();
				});
		}

		//��������
		inline void write(const char* data, const size_t size) {
			this->m_socket.async_write_some(boost::asio::buffer(data, size),
				std::bind(&asio9::TcpSession::on_write, this->shared_from_this(),
					std::placeholders::_1, std::placeholders::_2));
		}

		//����ÿ�δ�streambuf������ڴ��С
		inline void set_readsize(const size_t& size) { this->m_readsize = size; }
		//��streambuf�����ڴ�
		inline void consume(const size_t& size){
			this->m_streambuf.consume(size);
		}

		//��ȡsocket
		inline basic_type::socket_type* get_socket() { return &this->m_socket; }
		//��ȡexecutor
		inline basic_type::io_type::executor_type get_executor() { return this->m_io->get_executor(); };
		//��ȡÿ�δ�streambuf������ڴ��С
		inline size_t get_readsize() { return this->m_readsize; }
		//��ȡstreambuf
		inline boost::asio::streambuf* get_streambuf() { return &this->m_streambuf; };

		//�ص�����
		virtual void on_readready(const basic_type::ec_type& ec, const size_t& size) {}
		virtual void on_close() {}
		virtual void on_write(const basic_type::ec_type& ec, const size_t& size) {}
	private:
		void do_read() {
			auto buf = this->m_streambuf.prepare(this->m_readsize);
			this->m_socket.async_read_some(buf,
				std::bind(&asio9::TcpSession::read_handler, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		};

		void read_handler(const boost::system::error_code& ec, size_t length) {
			if (length)
				this->m_streambuf.commit(length);

			//���ӶϿ�
			if (ec.value() == boost::asio::error::eof)
				this->on_close();
			else
				this->on_readready(ec, length);

			//����������Ϣ
			if (!ec)
				this->do_read();
		};

		basic_type::io_type* m_io;
		basic_type::socket_type m_socket;

		boost::asio::streambuf m_streambuf;
		
		size_t m_readsize = 100;
	};
}

#endif

