#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "common.h"
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>

class Acceptor final {
public:
  using IncomingConnectionNotifier = std::function<void(const std::shared_ptr<boost::asio::ip::tcp::socket>&)>;

  Acceptor(boost::asio::io_service& read_write_io_service, IncomingConnectionNotifier notifier, uint16_t port)
    : read_write_io_service_(read_write_io_service)
    , endpoint_(boost::asio::ip::tcp::v4(), port)
    , acceptor_(io_service_, endpoint_)
    , notifier_(notifier) {
    io_thread_ = std::thread(&Acceptor::io_thread_, this);
  }

  ~Acceptor() {
    stop();

    io_thread_.join();
  }

  void stop() {
    std::lock_guard locker(work_mutex_);
    work_.reset();
  }

  void async_accept() {
    const std::shared_ptr<boost::asio::ip::tcp::socket> listen_socket =
      std::make_shared<boost::asio::ip::tcp::socket>(read_write_io_service_);

    acceptor_.async_accept(*listen_socket, [=](const boost::system::error_code& error) {
      handle_accept(listen_socket, error);
    });
  }

private:
  void acceptor_thread() {
    Common::set_thread_name("Acceptor Thread");

    {
      std::lock_guard locker(work_mutex_);
      work_ = std::make_shared<boost::asio::io_service::work>(io_service_);
    }

    async_accept();
    io_service_.run();
  }

  void handle_accept(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const boost::system::error_code& error) {
    if (error) {
      std::cerr << "Server::handle_accept: " << error.message() << std::endl;
      return;
    }

    notifier_(socket);
    async_accept();
  }

private:
  boost::asio::io_service& read_write_io_service_;

  boost::asio::io_service io_service_;

  mutable std::mutex work_mutex_;
  std::shared_ptr<boost::asio::io_service::work> work_;

  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::ip::tcp::acceptor acceptor_;

  std::thread io_thread_;

  IncomingConnectionNotifier notifier_;
};

#endif
