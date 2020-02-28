#include "server.h"
#include "common.h"
#include <thread>

Server::Server(int16_t port)
  : endpoint_(boost::asio::ip::tcp::v4(), port)
  , acceptor_(accept_io_service_, endpoint_) {
}

Server::~Server() {
  if (acceptor_thread.joinable()) {
    acceptor_thread.join();
  }
}

void Server::start_loop() {
  accept_incoming_connection(std::make_shared<boost::asio::ip::tcp::socket>(io_service_));
  std::thread acceptor_thread(&Server::acceptor_thread, this);
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

  {
    std::lock_guard<std::mutex> locker(incoming_sockets_mutex_);
    incoming_sockets_.push(socket);
  }

  accept_incoming_connection(std::make_shared<boost::asio::ip::tcp::socket>(io_service_));
}

void Server::acceptor_thread() {
  Common::set_thread_name("Acceptor Thread");
  accept_io_service_.run();
}

void Server::payload_thread() {
  while(true) {
    SockPtr incoming_socket = extract_incoming_socket();

    if (!incoming_socket) {
      std::this_thread::yield();
    }

    char data[512] = "This is a test message";

    boost::asio::async_write(*incoming_socket, boost::asio::buffer(data), [this] (const boost::system::error_code&, size_t) {
      std::cout << "data sent\n";
    });
  }
}

SockPtr Server::extract_incoming_socket() {
  std::lock_guard<std::mutex> locker(incoming_sockets_mutex_);

  if (incoming_sockets_.empty()) {
    return nullptr;
  }

  SockPtr incoming_socket = incoming_sockets_.front();
  incoming_sockets_.pop();

  return incoming_socket;
}
