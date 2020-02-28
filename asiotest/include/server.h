#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <queue>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class Server {
 private:
  using SocketPtr = std::shared_ptr<boost::asio::ip::tcp::socket>;

 public:
  Server(int16_t port);
  ~Server();
  void start_loop();

 private:
  SocketPtr extract_incoming_socket();

  void accept_incoming_connection(const SocketPtr& incoming_socket);
  void handle_accept(SocketPtr socket, const boost::system::error_code& err);

  void acceptor_thread();
  void payload_thread();

 private:
  boost::asio::io_service io_service_;
  boost::asio::io_service accept_io_service_;
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::ip::tcp::acceptor acceptor_;

  mutable std::mutex incoming_sockets_mutex_;
  std::queue<SocketPtr> incoming_sockets_;
};

#endif // SERVER_H
