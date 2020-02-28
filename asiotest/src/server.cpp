#include "server.h"

Server::Server(int16_t port)
  : endpoint_(boost::asio::ip::tcp::v4(), port)
  , acceptor_(io_service_, endpoint_) {
}

void Server::start_loop() {
  SocketPtr incoming_socket = std::make_shared<boost::asio::ip::tcp::socket>(io_service_);
  accept_incoming_connection(incoming_socket);
  io_service_.run();
}

void Server::accept_incoming_connection(const Server::SocketPtr& incoming_socket) {
  acceptor_.async_accept(*incoming_socket, boost::bind(&Server::handle_accept, this, incoming_socket, _1));
}

void Server::handle_accept(Server::SocketPtr socket, const boost::system::error_code& error) {
  if (error) {
    std::cout << "Server::handle_accept: " << error.message() << std::endl;
    return;
  }

  char data[512] = "This is a test message";
  boost::asio::async_write(*socket, boost::asio::buffer(data), [this] (const boost::system::error_code&, size_t) {});
}
