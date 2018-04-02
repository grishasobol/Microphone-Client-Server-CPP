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

int main()
{
  const unsigned port = 8080;

  if (!SendRecoder::isAvailable())
    throw std::exception("Microphone isn't available");

  sf::TcpSocket client_tcp;
  client_tcp.setBlocking(false);
  
  while (true) {
    std::cout << "Listen to the client" << std::endl;
    sf::TcpListener listener;
    if (listener.listen(port) != sf::Socket::Done)
    {
      throw PortBindingException(port);
    }
    if (listener.accept(client_tcp) != sf::Socket::Done)
      throw std::exception("Cannot accept connection");

    std::cout << "Start recording and sending to client" << std::endl;
    SendRecoder recoder;
    recoder.setChannelCount(1);
    recoder.setClientSocket(client_tcp);
    recoder.start();

    while (true) {
      size_t delay;
      std::cout << "Enter delay in miliseconds: " << std::flush;
      std::cin >> delay;
      if (std::cin.fail()){
        std::cout << "Invalid value" << std::endl;
        std::cin.clear();
        std::cin.ignore();
        continue;
      }
      if (recoder.isStopped) {
        std::cout << "Recoder was stopped" << std::endl;
        break;
      }
      else if (delay < 10000) {
        recoder.setDelay(sf::milliseconds(delay));
      }
    }
  }

  return 0;
}