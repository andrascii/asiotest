#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class Server {
 private:
  using SocketPtr = std::shared_ptr<boost::asio::ip::tcp::socket>;

 public:
  Server(int16_t port);
  void start_loop();

 private:
  void accept_incoming_connection(const SocketPtr& incoming_socket);
  void handle_accept(SocketPtr socket, const boost::system::error_code& err);

 private:
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::ip::tcp::acceptor acceptor_;
};

#endif // SERVER_H
