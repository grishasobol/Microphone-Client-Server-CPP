#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

#include "../Server-Microphone/TimerBlock.h"
#include "../Server-Microphone/sfml_exceptioins.h"
#include "FFTAnalysis.h"

#include <iostream>
#include <memory>
#include <algorithm>
#include <thread>
#include <mutex>
#include <string>

void FFTConnectionHandler(double* Aval, double* Ftvl, size_t Nvl, size_t Nft,
  sf::TcpSocket& server_socket, const bool& close, std::mutex& fft_lock)
{
  // array of recieved samples
  sf::Int16* samples = new sf::Int16[Nvl];
  // temp array to store recieved samples
  double* tmp_aval = new double[Nvl];
  const double* tmp_pointer = tmp_aval;
  size_t recieved;
  // if server don't response wait 100 milliseconds
  TimerBlock timer(sf::milliseconds(100));
  // while not closed
  while (!close) {
    // recieve samples
    auto result = server_socket.receive(samples, Nvl, recieved);
    if (result != sf::Socket::Done && result != sf::Socket::Partial) {
      // wait if server don't response
      timer.wait();
    }
    else {
      // copy old samples to tmp array
      for (unsigned i = Nvl - 1; i >= recieved; i--) {
        tmp_aval[i - recieved] = Aval[i];
      }
      // copy new samples to tmp array
      for (unsigned i = 0; i < recieved; i++) {
        tmp_aval[i + Nvl - recieved] = samples[i];
      }
      // swap tmp array and Aval
      std::swap(tmp_aval, Aval);
      // FFT
      fft_lock.lock();
      FFTAnalysis(Aval, Ftvl, Nvl, Nft);
      fft_lock.unlock();
    }
  }
  delete[] samples;
  delete[] tmp_pointer;
}

int main()
{
  using cu_short = const unsigned short;

  // Connect to the server
  sf::TcpSocket server_socket;
  while (true) {
    std::string server_address;
    std::string port;
    std::cout << "Server address: " << std::flush;
    std::getline(std::cin, server_address);
    std::cout << "Server port: " << std::flush;
    std::getline(std::cin, port);
    std::cout << "Trying to connect" << std::endl;

    if (server_address.empty())
      server_address = "127.0.0.1";
    if (port.empty())
      port = "8080";

    sf::Socket::Status status = server_socket.connect(server_address, std::stoi(port), 
      sf::milliseconds(9999));
    if (status != sf::Socket::Done) {
      std::cout << "Connection failed=(\n"
                   "Please repeate" << std::endl;
    }
    else {
      std::cout << "Connection established!!!" << std::endl;
      break;
    }
  }

  // initialize constant variables
  const size_t Nvl = 16384;    // number of sound countings
  const size_t Nft = Nvl / 2;  // number of fft countings

  double Aval[Nvl] = {0}; // array of sound countings
  double Ftvl[Nft] = {0}; // array of sound fft

  // create thread to handle connection with server and evaluate fft
  bool close = false; // informs thread, that it is tame to finish
  std::mutex fft_lock; // lock fft drawing, if we evaluate it and vice versa
  std::thread fft_handler(FFTConnectionHandler, Aval, Ftvl, Nvl, Nft, 
    std::ref(server_socket), std::cref(close), std::ref(fft_lock));

  // graphic represantaion of the fft
  cu_short columns_number = 128;
  cu_short columns_weight = 5;
  cu_short columns_space = 1;
  cu_short indentation = 50;
  cu_short horizontal_win_size = columns_number * (columns_space + columns_weight) 
    + indentation * 2;
  cu_short vertical_win_size = 600;

  cu_short columns_density = Nft / columns_number;

  double columns[columns_number];

  // create window
  sf::RenderWindow window(sf::VideoMode(horizontal_win_size, vertical_win_size), "Client-FFT");
  window.setActive(true);

  // update window each 100 milliseconds
  TimerBlock timer(sf::milliseconds(100));
  // draw fft until window will be closed
  while (window.isOpen())
  {
    // handle close event
    sf::Event event;
    while (window.pollEvent(event))
    {
      switch (event.type)
      {
      case sf::Event::Closed: 
        window.close();
        close = true;
        continue;
      default:
        break;
      }
    }

    // evaluate columns size and draw them
    window.clear();
    fft_lock.lock();
    for (unsigned i = 0; i < columns_number; i++) {
      columns[i] = 0;
      for (unsigned j = 0; j < columns_density; j++) {
        columns[i] += Ftvl[j + columns_density * i];
      }
      columns[i] /= columns_density;
      sf::RectangleShape shape(sf::Vector2f(columns_weight, columns[i] + 1));
      shape.setFillColor(sf::Color::Green);
      shape.setPosition(indentation + i * (columns_space + columns_weight),
        vertical_win_size - columns[i] - 3);
      window.draw(shape);
    }
    fft_lock.unlock();
    window.display();
    timer.wait();
  }

  fft_handler.join();

  return 0;
}