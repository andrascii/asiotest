#ifndef IOSERVICESPOOL_H
#define IOSERVICESPOOL_H

#include "common.h"
#include <vector>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

class IoServicesPool final {
public:
  using IoService = boost::asio::io_service;
  using IoServiceWork = IoService::work;

  IoServicesPool() {
    io_thread_work_ = std::make_shared<IoServiceWork>(io_service_);

    for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
      thread_group_.create_thread([this, i] {
        std::stringstream stream;
        stream << "Socket IO Thread #" << i;
        io_service_thread(io_service_, stream.str());
      });
    }
  }

  ~IoServicesPool() {
    io_thread_work_.reset();
    thread_group_.join_all();
  }

  IoService& io_service() {
    return io_service_;
  }

private:
  void io_service_thread(IoService& io_service, const std::string& thread_name) const {
    Common::set_thread_name(thread_name);
    io_service.run();
  }

private:
  IoService io_service_;
  std::shared_ptr<IoServiceWork> io_thread_work_;

  boost::thread_group thread_group_;
};

#endif
