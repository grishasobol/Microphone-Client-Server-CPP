#pragma once
#include <SFML/System.hpp>

class TimerBlock : sf::Clock
{
  sf::Time timeout;
public:
  void wait();
  TimerBlock(const sf::Time& timeout);
  ~TimerBlock();
};

