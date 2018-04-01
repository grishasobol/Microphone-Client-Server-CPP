#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include <iostream>
#include <thread>
#include <mutex>
#include <memory>

#include "SendRecoder.h"
#include "sfml_exceptioins.h"
#include "TimerBlock.h"

enum ServerState {
  establish_connection,
  start_recording,
  send_data
};

void ConnectionHandler(sf::TcpSocket& client_tcp, SendRecoder& recoder, 
                       unsigned port, const bool& close) 
{
  sf::TcpListener listener;
  if (listener.listen(port) != sf::Socket::Done)
  {
    throw PortBindingException(port);
  }
  listener.setBlocking(false);
  TimerBlock timer = sf::milliseconds(100);
  while (listener.accept(client_tcp) != sf::Socket::Done && !close)
  {
    timer.wait();
  }
  recoder.setClientAdress(client_tcp.getRemoteAddress());
  recoder.start();
}

int main()
{
  sf::Window window(sf::VideoMode(200, 200), "Microphone Server");
  
  const unsigned port = 8080;

  if (!SendRecoder::isAvailable())
    throw std::exception("Microphone isn't available");

  SendRecoder recoder;
  recoder.setClientPort(port);

  sf::TcpSocket client_tcp;
  client_tcp.setBlocking(false);

  std::unique_ptr<std::thread> thr = nullptr;

  bool close = false;
  auto server_state = ServerState::establish_connection;
  TimerBlock timer(sf::milliseconds(100));
  while (window.isOpen()) {
    switch (server_state)
    {
    case ServerState::establish_connection:
    {
      if (thr.get() != nullptr) {
        close = true;
        thr->join();
        delete thr.get();
        close = false;
        recoder.stop();
      }
      thr.reset(new std::thread(ConnectionHandler, std::ref(client_tcp), std::ref(recoder),
        port, std::cref(close)));
      server_state = ServerState::send_data;
    }
    case ServerState::send_data:
    {
      timer.wait();
      sf::Event event;
      window.waitEvent(event);
      switch (event.type)
      {
      case sf::Event::Closed:
        window.close();
        close = true;
        thr->join();
      default:
        break;
      }
    }
    default:
      break;
    }
  }

  return 0;
}