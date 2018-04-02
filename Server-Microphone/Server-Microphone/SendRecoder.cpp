#include "SendRecoder.h"
#include "sfml_exceptioins.h"
#include "TimerBlock.h"

#include <iostream>
#include <thread>

SendRecoder::~SendRecoder()
{
  if (!isStopped) {
    stop();
  }
  if (connection_thread.joinable())
    connection_thread.join();
}

SendRecoder::SendRecoder() {
  setProcessingInterval(sf::milliseconds(10));
  batch_size = 10000;
}

bool SendRecoder::onStart() {
  isStopped = false;
  connection_thread = std::thread{ &SendRecoder::ConnectionHandler, this };
  return true;
}

void SendRecoder::onStop() {
  isStopped = true;
}

void SendRecoder::ConnectionHandler() {
  TimerBlock timer(sf::milliseconds(10));
  size_t sended;
  while (!isStopped) {
    while (!samples.empty()) {
      auto result = socket->send(&samples[0] , std::min(samples.size(), batch_size), sended);
      if (result == sf::Socket::Disconnected) {
        isStopped = true;
        stop();
        break;
      }
      samples.erase(samples.begin(), samples.begin() + sended);
      sf::sleep(delay);
    }
    timer.wait();
  }
}

void SendRecoder::setClientSocket(sf::TcpSocket& client_socket) {
  this->socket = &client_socket;
}

void SendRecoder::setDelay(const sf::Time& delay) {
  this->delay = delay;
}

bool SendRecoder::onProcessSamples(const sf::Int16* samples, std::size_t sampleCount) {
  this->samples.insert(this->samples.end(), samples, samples + sampleCount);
  return true;
}