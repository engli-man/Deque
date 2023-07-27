#include "stress_tests.cpp"
#include "tests.cpp"

int main() {
  std::cout << "Start simple tests"
            << "\n";
  OTHER();
  CONSTRUCTOR();
  ACCESS();
  ITERATORS();
  MODS();
  EXCEPTS();
  CONSTRUCTORS_P2();
  PROPAGATE();
  ACCOUNTANT();
  EXCEPT_P2();
  CONSTRUCT_AND_MOD();
  std::cout << "Finish simple tests"
            << "\n"
            << "Start stress tests"
            << "\n";

  std::vector<size_t> vector_with_random_numbers;
  FillVectorWithRandomNumbers(vector_with_random_numbers, kTestSize,
                              kDistrBegin, kDistrEnd);

  auto start = std::chrono::high_resolution_clock::now();
  TestFunction(vector_with_random_numbers);
  auto stop = std::chrono::high_resolution_clock::now();

  auto duration_in_seconds =
      std::chrono::duration_cast<std::chrono::seconds>(stop - start).count();

  std::cout << "Stress test took " << duration_in_seconds << " seconds"
            << std::endl;

  return duration_in_seconds > kNormalDuration ? 1 : 0;
}