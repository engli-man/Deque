/** @author yaishenka
    @date 29.01.2023 */
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "deque.hpp"

void FillVectorWithRandomNumbers(std::vector<size_t>& v, size_t numbers_count,
                                 size_t begin, size_t end) {
  v.resize(numbers_count);

  std::random_device rnd_device;
  std::mt19937 mersenne_engine{rnd_device()};
  std::uniform_int_distribution<size_t> dist{begin, end};

  auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

  std::generate(v.begin(), v.end(), gen);
}

void TestFunction(const std::vector<size_t>& test_vector) {
  Deque<size_t> d;

  for (const auto& number : test_vector) {
    d.push_back(number);
  }

  for (const auto& number : test_vector) {
    d.push_front(number);
  }

  while (!d.empty()) {
    d.pop_back();
  }
}

static constexpr size_t kTestSize = 10000000;
static constexpr size_t kDistrBegin = 1;
static constexpr size_t kDistrEnd = 100;
static constexpr long long kNormalDuration = 5;
