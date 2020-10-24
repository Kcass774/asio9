#ifndef ASIO9_TCPSERVER_HPP_
#define ASIO9_TCPSERVER_HPP_

#include <boost/asio.hpp>

#include "TcpSession.hpp"

namespace asio9 {
	template<class SessionType>
	class TcpServer : public std::enable_shared_from_this<TcpServer<SessionType>>
	{
	public:
		typedef std::shared_ptr<SessionType> session_ptr;

		TcpServer(basic_type::io_type* io, const basic_type::endpoint_type& endpoint)
			:m_acceptor(boost::asio::make_strand(*io))
		{
			this->m_acceptor.open(endpoint.protocol());

			//设置复用socket
			this->m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

			this->m_acceptor.bind(endpoint);
			this->m_acceptor.listen(boost::asio::socket_base::max_listen_connections);

			this->m_io = io;
		}
		~TcpServer() {
		}

		//开始监听
		inline void run() {
			this->do_accept();
		};

		//停止监听
		inline void Close(basic_type::ec_type* ec) {
			this->m_io->post([this]() {
				this->m_acceptor.close();
				});
		};

		//是否监听
		inline bool isListen() { return this->m_acceptor.is_open(); };

		//新连接回调
		virtual void on_accept(asio9::basic_type::ec_type ec, session_ptr session) {};

	private:
		inline void do_accept() {
			this->m_acceptor.async_accept(
				boost::asio::make_strand(*this->m_io), 
				std::bind(&asio9::TcpServer<SessionType>::accept_handler, this, std::placeholders::_1, std::placeholders::_2));
		};

		void accept_handler(boost::system::error_code ec, basic_type::socket_type socket)
		{
			//检查错误
			if (ec)
			{
				this->on_accept(ec, nullptr);
			}
			else
			{
				//构造并运行Session
				auto session = std::make_shared<SessionType>(this->m_io, std::move(socket));
				session->run();

				//调用回调
				this->on_accept(ec, session);

				//接收下个连接
				this->do_accept();
			}
		}

		basic_type::io_type* m_io = nullptr;
		basic_type::acceptor_type m_acceptor;
	};
}

#endif