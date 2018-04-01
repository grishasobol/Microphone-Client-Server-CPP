#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

class SendRecoder : public sf::SoundRecorder
{
  sf::UdpSocket socket;
  sf::IpAddress client_address;
  unsigned port;
public:
  virtual bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount);
  void setClientAdress(sf::IpAddress);
  void setClientPort(unsigned);
  SendRecoder();
  SendRecoder(sf::IpAddress, unsigned port);
  ~SendRecoder();
};

