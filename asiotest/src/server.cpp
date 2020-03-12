#include "server.h"
#include "common.h"
#include <thread>
#include <cassert>

Server::Server(uint16_t port) {
  acceptor_ = std::make_unique<Acceptor>(io_service_pool_.io_service(),
    [this](const SocketPtr& socket) { push_incoming_connection(socket); },
    port);
}

bool Server::await_ready() const {
  std::lock_guard locker(incoming_connections_mutex_);
  return !incoming_connections_.empty();
}

void Server::await_suspend(std::experimental::coroutine_handle<> handle) {
  awaiters_.push(handle);
}

std::shared_ptr<boost::asio::ip::tcp::socket> Server::await_resume() {
  SocketPtr socket = extract_incoming_connection();
  assert(socket);
  return socket;
}

/*
void Server::payload_thread() {
  while(true) {
    SocketPtr incoming_socket = extract_incoming_connection();

    if (!incoming_socket) {
      std::this_thread::yield();
    }

    char data[512] = "This is a test message";

    boost::asio::async_write(*incoming_socket, boost::asio::buffer(data),
      [this] (const boost::system::error_code&, size_t) {
        std::cout << "data sent\n";
      });
  }
}
*/

void Server::push_incoming_connection(const SocketPtr& incoming_connection) {
  {
    std::lock_guard locker(incoming_connections_mutex_);
    incoming_connections_.push(incoming_connection);
  }
  
  std::experimental::coroutine_handle<> awaiter = extract_awaiter();
  
  if (awaiter) {
    awaiter.resume();
  }
}

Server::SocketPtr Server::extract_incoming_connection() {
  std::lock_guard locker(incoming_connections_mutex_);

  if (incoming_connections_.empty()) {
    return nullptr;
  }

  SocketPtr incoming_socket = incoming_connections_.front();
  incoming_connections_.pop();

  return incoming_socket;
}

std::experimental::coroutine_handle<> Server::extract_awaiter() {
  std::lock_guard locker(awaiters_mutex_);
  
  if (awaiters_.empty()) {
    return std::experimental::coroutine_handle<>{};
  }
  
  const auto awaiter_handle = awaiters_.front();
  awaiters_.pop();
  return awaiter_handle;
}
