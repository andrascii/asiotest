#include <iostream>
#include <chrono>
#include <thread>
#include "server.h"
#include "client.h"
#include "common.h"

std::thread start_server() {
  std::thread server_thread([]{
    Common::set_thread_name("Server Thread");

    Server server(12345);
    server.start_loop();

    std::cout << "Server done!" << std::endl;
  });

  return server_thread;
}

std::thread start_client() {
  using namespace std::chrono_literals;
  //std::this_thread::sleep_for(200ms);

  std::thread client_thread([]{
    Common::set_thread_name("Client Thread");

    Client client;
    client.start_messaging();

    std::cout << "Client done!" << std::endl;
  });

  return client_thread;
}

int main() {
  std::thread server_thread = start_server();
  std::thread client_thread = start_client();
  server_thread.join();
  client_thread.join();
}
