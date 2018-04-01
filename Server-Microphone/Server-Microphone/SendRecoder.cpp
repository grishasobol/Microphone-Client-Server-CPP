#include "SendRecoder.h"

#include "sfml_exceptioins.h"

SendRecoder::~SendRecoder()
{
  stop();
}

SendRecoder::SendRecoder() {}

SendRecoder::SendRecoder(sf::IpAddress client_address, unsigned port) : 
  client_address(client_address), port(port)
{
  auto result = socket.bind(port);
  if (result != sf::Socket::Done)
    throw PortBindingException(port);
}

void SendRecoder::setClientAdress(sf::IpAddress client_address) {
  this->client_address = client_address;
}

void SendRecoder::setClientPort(unsigned port){
  this->port = port;
  auto result = socket.bind(port);
  if (result != sf::Socket::Done)
    throw PortBindingException(port);
}

bool SendRecoder::onProcessSamples(const sf::Int16* samples, std::size_t sampleCount) {
  auto result = socket.send(samples, sampleCount, client_address, port);
  if (result != sf::Socket::Done)
    return false;
  else
    return true;
}