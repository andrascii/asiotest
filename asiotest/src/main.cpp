#include "server.h"
#include "client.h"
#include "common.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <experimental/coroutine>

namespace std {
namespace experimental {

template <typename T, typename... X>
struct coroutine_traits<std::future<T>, X...> {
  struct promise_type {
    std::promise<T> state;
    
    auto get_return_object() {
      return state.get_future();
    }
    
    auto initial_suspend() {
      return std::experimental::suspend_never();
    }
    
    auto final_suspend() {
      return std::experimental::suspend_never();
    }
    
    template <typename U>
    void return_value(U&& value) {
      state.set_value(T{ std::forward<U>(value) });
    }
    
    void unhandled_exception() {
      state.set_exception(std::current_exception());
    }
  };
};

}  // namespace experimental
}  // namespace std

std::thread start_client() {
  using namespace std::chrono_literals;

  std::thread client_thread([]{
    Common::set_thread_name("Client Thread");

    Client client;
    client.start_messaging();

    std::cout << "Client done!" << std::endl;
  });

  return client_thread;
}

std::future<std::string> accept_connections(Server& server) {
  for (;;) {
    std::shared_ptr<boost::asio::ip::tcp::socket> socket = co_await server;
    std::cout << "accepted connection... ";
    
    char data[512] = "This is a test message";

    boost::asio::async_write(*socket, boost::asio::buffer(data),
      [] (const boost::system::error_code&, size_t) {
        std::cout << "sent welcome message" << std::endl;
      });
  }

  co_return std::string("test");
}

int main() {
  Server server(12345);
  auto f = accept_connections(server);

  std::thread client_thread = start_client();
  client_thread.join();

  f.get();
}
