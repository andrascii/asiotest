#include "client.h"

Client::Client()
  : socket_(io_service_) {
}

void Client::start_messaging() {
  const auto local_ip_addr = boost::asio::ip::address::from_string("127.0.0.1");
  const auto local_server_endpoint = boost::asio::ip::tcp::endpoint(local_ip_addr, 12345);
  socket_.async_connect(local_server_endpoint, boost::bind(&Client::connected, this, _1));
  io_service_.run();
}

void Client::connected(const boost::system::error_code& error) {
  if (error) {
    std::cout << "Client::connected: " << error.message() << std::endl;
    return;
  }

  char data[512] = {0};

  const auto read_callback = boost::bind(
    &Client::handle_read,
    this,
    boost::asio::placeholders::error,
    boost::asio::placeholders::bytes_transferred
  );

  boost::asio::async_read(socket_, boost::asio::buffer(data, 512), boost::asio::transfer_at_least(1), read_callback);
}

void Client::handle_read(const boost::system::error_code& error, size_t bytes_received) {
  if (error) {
    std::cout << "Client::handle_read: " << error.message() << std::endl;
    return;
  }

  std::cout << "Bytes received: " << bytes_received << std::endl;
}
