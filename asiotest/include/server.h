#ifndef SERVER_H
#define SERVER_H

#include "ioservicespool.h"
#include "acceptor.h"
#include <iostream>
#include <queue>
#include <memory>
#include <experimental/coroutine>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class Server {
private:
  using SocketPtr = std::shared_ptr<boost::asio::ip::tcp::socket>;

public:
  Server(uint16_t port);

  bool await_ready() const;
  void await_suspend(std::experimental::coroutine_handle<> handle);
  std::shared_ptr<boost::asio::ip::tcp::socket> await_resume();

private:
  void push_incoming_connection(const SocketPtr& incoming_connection);
  SocketPtr extract_incoming_connection();

  bool has_awaiter() const;
  std::experimental::coroutine_handle<> extract_awaiter();

private:
  IoServicesPool io_service_pool_;
  std::unique_ptr<Acceptor> acceptor_;

  mutable std::mutex incoming_connections_mutex_;
  std::queue<SocketPtr> incoming_connections_;

  std::queue<std::experimental::coroutine_handle<>> awaiters_;
};

#endif // SERVER_H
