#include <iostream>
#include <chrono>
#include <thread>
#include <sys/prctl.h>
#include <linux/sched.h>
#include <pthread.h>
#include "server.h"
#include "client.h"

namespace {

void set_thread_name(const char* name) {
  prctl(PR_SET_NAME, name, 0, 0, 0);
}

void run_service(boost::asio::io_service& service) {
  set_thread_name("run_service thread");
  service.run();
}

}

std::thread start_server() {
  std::thread server_thread([]{
    set_thread_name("Server Thread");

    Server server(12345);
    server.start_loop();

    std::cout << "Server done!" << std::endl;
  });

  return server_thread;
}

std::thread start_client() {
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(200ms);

  std::thread client_thread([]{
    set_thread_name("Client Thread");

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
