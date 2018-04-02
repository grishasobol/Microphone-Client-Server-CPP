#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <vector>
#include <mutex>
#include <memory>

class SendRecoder : public sf::SoundRecorder
{
  sf::TcpSocket* socket;
  sf::Time delay = sf::milliseconds(0);
  void ConnectionHandler();
  std::vector<sf::Int16> samples;
  std::mutex samples_lock;
  std::thread connection_thread;
  size_t batch_size;
public:
  bool isStopped = true;
  virtual bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount);
  virtual bool onStart();
  virtual void onStop();
  void setClientSocket(sf::TcpSocket&);
  void setDelay(const sf::Time&);
  SendRecoder();
  ~SendRecoder();
};

