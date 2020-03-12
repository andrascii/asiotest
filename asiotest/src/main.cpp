#include "server.h"
#include "client.h"
#include "common.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <experimental/coroutine>

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
  std::shared_ptr<boost::asio::ip::tcp::socket> socket = co_await server;
  std::cout << "accepted connection";

  co_return std::string("test");
}

int main() {
  Server server(12345);
  auto f = accept_connections(server);

  std::thread client_thread = start_client();
  client_thread.join();

  f.get();
}
