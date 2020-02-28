#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class Client {
 public:
  Client();
  void start_messaging();

 private:
  void connected(const boost::system::error_code& error);
  void handle_read(const boost::system::error_code& error, size_t bytes_received);

 private:
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::socket socket_;
};
#endif // CLIENT_H
