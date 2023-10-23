#pragma once
#include <chrono>
#include <iostream>
#include <string>
using namespace std::chrono_literals;

template <class T, class U>
std::string duration_to_string(std::chrono::duration<T, U> duration) {
  auto ns = duration / 1ns;
  if (ns <= 1000) {
    return std::to_string(ns) + " ns";
  }
  double us = duration / 1.0us;
  if (us <= 1000) {
    return std::to_string(us) + " us";
  }
  double ms = duration / 1.0ms;
  if (ms <= 1000) {
    return std::to_string(ms) + " ms";
  }
  double s = duration / 1.0s;
  return std::to_string(s) + " s";
}

class ScopeTimer {
public:
  ScopeTimer(std::string name)
      : m_name(std::move(name)),
        m_beg(std::chrono::high_resolution_clock::now()), m_tick(m_beg) {}
  void tick(const std::string &name) {
    auto end = std::chrono::high_resolution_clock::now();
    auto dur =
        std::chrono::duration_cast<std::chrono::microseconds>(end - m_tick);
    std::cout << ">>> " << m_name << " >>> " << name << ": "
              << duration_to_string(dur) << "\n";
    m_tick = std::chrono::high_resolution_clock::now();
  }
  ~ScopeTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto dur =
        std::chrono::duration_cast<std::chrono::microseconds>(end - m_beg);
    std::cout << ">>> " << m_name << ": " << duration_to_string(dur) << "\n";
  }

private:
  std::string m_name;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_beg;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_tick;
};