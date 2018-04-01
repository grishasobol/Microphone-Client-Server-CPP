#include "TimerBlock.h"

void TimerBlock::wait() {
  if (getElapsedTime() < timeout) {
    sf::sleep(getElapsedTime() - timeout);
  }
  restart();
}

TimerBlock::TimerBlock(const sf::Time& timeout) : Clock(), timeout(timeout)
{
}


TimerBlock::~TimerBlock()
{
}
