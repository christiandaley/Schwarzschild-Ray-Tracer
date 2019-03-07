#pragma once

#include <chrono>
#include <iostream>

class Timer {
public:
  bool isStopped = false;
  double elapsed = 0.0;
  std::chrono::time_point<std::chrono::high_resolution_clock> t1;

  Timer() {
    this->start();
  }

  void reset() {
    this->t1 = std::chrono::high_resolution_clock::now();
    elapsed = 0.0;
  }

  void stop() {
    this->isStopped = true;
    auto t2 = std::chrono::high_resolution_clock::now();
    elapsed += std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - this->t1).count() / 1e9;
  }

  void start() {
    this->isStopped = false;
    this->t1 = std::chrono::high_resolution_clock::now();
  }

  void print_elapsed_time() {
    const bool should_start = this->isStopped == false;
    if (!this->isStopped) {
      this->stop();
    }

    double seconds = elapsed;
    const int hours = (int)(seconds / 3600.0);
    seconds -= hours * 3600.0;
    const int minutes = (int)(seconds / 60.0);
    seconds -= minutes * 60.0;

    if (hours > 0)
      std::cout << hours << "h ";
    if (minutes > 0)
      std::cout << minutes << "m ";

    std::cout << seconds << "s" << std::endl;

    if (should_start) {
      this->start();
    }
  }

  double seconds() const {
    return elapsed;
  }

  double milliseconds() const {
    return elapsed * 1000.0;
  }

  double nanoseconds() const {
    return elapsed * 1e9;
  }
};
