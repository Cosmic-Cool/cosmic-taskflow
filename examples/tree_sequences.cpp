// This example demonstrates computing unlabeled tree sequences using generating functions
// Implements the conversion from A000081 (rooted trees) to A000055 (unrooted trees)
// using the dissymmetry theorem for trees.

#include <taskflow/taskflow.hpp>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

// Compute A000081 (unlabeled rooted trees) using Pólya's functional equation
// R(x) = x * exp(sum_{k>=1} R(x^k)/k) = x * prod_{m>=1} (1-x^m)^{-r_m}
// We compute coefficients r_n iteratively using the recurrence formula
class RootedTreeSequence {
private:
  std::vector<long long> r;  // r[n] = number of rooted trees with n nodes
  
public:
  RootedTreeSequence(size_t max_n) : r(max_n + 1, 0) {
    if (max_n >= 1) {
      r[1] = 1;  // Single node tree
    }
    compute(max_n);
  }
  
  // Compute rooted tree counts using the functional equation
  // The recurrence (from OEIS A000081) is:
  // a(n+1) = (1/n) * sum_{k=1}^{n} (sum_{d|k} d * a(d)) * a(n-k+1)
  // Or equivalently: a(m) = (1/(m-1)) * sum_{k=1}^{m-1} (sum_{d|k} d * a(d)) * a(m-k)
  void compute(size_t max_n) {
    for (size_t n = 1; n < max_n; n++) {
      // Compute r[n+1]
      long long sum = 0;
      
      for (size_t k = 1; k <= n; k++) {
        // Compute a_k = sum of d*r[d] where d divides k
        long long a_k = 0;
        for (size_t d = 1; d <= k; d++) {
          if (k % d == 0) {
            a_k += d * r[d];
          }
        }
        sum += a_k * r[n - k + 1];
      }
      
      r[n + 1] = sum / n;
    }
  }
  
  long long operator[](size_t n) const {
    return (n < r.size()) ? r[n] : 0;
  }
  
  size_t size() const { return r.size(); }
  
  void print(size_t max_n) const {
    std::cout << "A000081 (rooted trees): ";
    for (size_t i = 1; i <= max_n && i < r.size(); i++) {
      std::cout << r[i];
      if (i < max_n && i + 1 < r.size()) std::cout << ", ";
    }
    std::cout << std::endl;
  }
};

// Compute A000055 (unlabeled unrooted trees) from A000081
// Using dissymmetry theorem: U(x) = R(x) - (1/2)(R(x)^2 - R(x^2))
class UnrootedTreeSequence {
private:
  std::vector<long long> u;  // u[n] = number of unrooted trees with n nodes
  
public:
  UnrootedTreeSequence(const RootedTreeSequence& rooted, size_t max_n) : u(max_n + 1, 0) {
    compute(rooted, max_n);
  }
  
  // Apply the dissymmetry formula coefficient-wise
  void compute(const RootedTreeSequence& rooted, size_t max_n) {
    for (size_t n = 1; n <= max_n; n++) {
      long long r_n = rooted[n];
      
      // Compute the convolution sum: sum_{i=1}^{n-1} r_i * r_{n-i}
      long long convolution = 0;
      for (size_t i = 1; i < n; i++) {
        convolution += rooted[i] * rooted[n - i];
      }
      
      // Correction term for even n: +r_{n/2} (from R(x^2))
      long long correction = 0;
      if (n % 2 == 0) {
        correction = rooted[n / 2];
      }
      
      // Apply formula: u_n = r_n - (1/2)(convolution - correction)
      // u_n = r_n - (1/2)*(sum r_i*r_{n-i} - 1_{2|n}*r_{n/2})
      u[n] = r_n - (convolution - correction) / 2;
    }
  }
  
  long long operator[](size_t n) const {
    return (n < u.size()) ? u[n] : 0;
  }
  
  size_t size() const { return u.size(); }
  
  void print(size_t max_n) const {
    std::cout << "A000055 (unrooted trees): ";
    for (size_t i = 1; i <= max_n && i < u.size(); i++) {
      std::cout << u[i];
      if (i < max_n && i + 1 < u.size()) std::cout << ", ";
    }
    std::cout << std::endl;
  }
};

// Validate computed sequences against known OEIS values
bool validate_sequences(const RootedTreeSequence& rooted, 
                        const UnrootedTreeSequence& unrooted) {
  // Known values from OEIS
  const std::vector<long long> expected_rooted = {0, 1, 1, 2, 4, 9, 20, 48, 115, 286, 719};
  const std::vector<long long> expected_unrooted = {0, 1, 1, 1, 2, 3, 6, 11, 23, 47, 106};
  
  bool all_correct = true;
  
  std::cout << "\n=== Validation ===" << std::endl;
  std::cout << std::setw(4) << "n" 
            << std::setw(12) << "rooted" 
            << std::setw(12) << "expected"
            << std::setw(12) << "unrooted"
            << std::setw(12) << "expected"
            << std::setw(10) << "status" << std::endl;
  std::cout << std::string(62, '-') << std::endl;
  
  for (size_t n = 1; n < expected_rooted.size(); n++) {
    bool rooted_ok = (rooted[n] == expected_rooted[n]);
    bool unrooted_ok = (unrooted[n] == expected_unrooted[n]);
    bool row_ok = rooted_ok && unrooted_ok;
    
    std::cout << std::setw(4) << n
              << std::setw(12) << rooted[n]
              << std::setw(12) << expected_rooted[n]
              << std::setw(12) << unrooted[n]
              << std::setw(12) << expected_unrooted[n]
              << std::setw(10) << (row_ok ? "OK" : "FAIL") << std::endl;
    
    all_correct = all_correct && row_ok;
  }
  
  return all_correct;
}

// Demonstrate the step-by-step calculation for small n
void demonstrate_calculation(const RootedTreeSequence& rooted, size_t n) {
  std::cout << "\n=== Step-by-step calculation for n = " << n << " ===" << std::endl;
  
  long long r_n = rooted[n];
  std::cout << "r_" << n << " = " << r_n << std::endl;
  
  // Show the convolution
  std::cout << "\nConvolution sum_{i=1}^{" << (n-1) << "} r_i * r_{" << n << "-i}:" << std::endl;
  long long convolution = 0;
  for (size_t i = 1; i < n; i++) {
    long long term = rooted[i] * rooted[n - i];
    convolution += term;
    std::cout << "  r_" << i << " * r_" << (n-i) << " = " 
              << rooted[i] << " * " << rooted[n-i] << " = " << term << std::endl;
  }
  std::cout << "Total convolution = " << convolution << std::endl;
  
  // Show correction term
  long long correction = 0;
  if (n % 2 == 0) {
    correction = rooted[n / 2];
    std::cout << "\nCorrection term (n is even): r_{" << (n/2) << "} = " << correction << std::endl;
  } else {
    std::cout << "\nCorrection term (n is odd): 0" << std::endl;
  }
  
  // Final calculation
  long long u_n = r_n - (convolution - correction) / 2;
  std::cout << "\nu_" << n << " = r_" << n << " - (1/2)(" << convolution 
            << " - " << correction << ")" << std::endl;
  std::cout << "u_" << n << " = " << r_n << " - " << ((convolution - correction) / 2) 
            << " = " << u_n << std::endl;
}

int main(int argc, char* argv[]) {
  // Default to computing first 20 terms
  size_t max_n = 20;
  
  if (argc > 1) {
    max_n = std::atoi(argv[1]);
    if (max_n < 1 || max_n > 100) {
      std::cerr << "Error: max_n should be between 1 and 100\n";
      return EXIT_FAILURE;
    }
  }
  
  std::cout << "=== Tree Sequence Generation ===" << std::endl;
  std::cout << "Computing sequences up to n = " << max_n << std::endl;
  std::cout << "\nUsing dissymmetry theorem:" << std::endl;
  std::cout << "U(x) = R(x) - (1/2)(R(x)^2 - R(x^2))" << std::endl;
  std::cout << std::endl;
  
  // Use Taskflow to compute sequences in parallel
  tf::Executor executor;
  tf::Taskflow taskflow("Tree Sequence Computation");
  
  // Storage for results
  RootedTreeSequence* rooted = nullptr;
  UnrootedTreeSequence* unrooted = nullptr;
  
  // Task 1: Compute rooted tree sequence (A000081)
  tf::Task compute_rooted = taskflow.emplace([&]() {
    std::cout << "Computing A000081 (rooted trees)..." << std::endl;
    rooted = new RootedTreeSequence(max_n);
  }).name("Compute Rooted");
  
  // Task 2: Compute unrooted tree sequence (A000055) - depends on rooted
  tf::Task compute_unrooted = taskflow.emplace([&]() {
    std::cout << "Computing A000055 (unrooted trees)..." << std::endl;
    unrooted = new UnrootedTreeSequence(*rooted, max_n);
  }).name("Compute Unrooted");
  
  // Task 3: Display results
  tf::Task display = taskflow.emplace([&]() {
    std::cout << "\n=== Results ===" << std::endl;
    rooted->print(std::min(max_n, size_t(15)));
    unrooted->print(std::min(max_n, size_t(15)));
  }).name("Display");
  
  // Task 4: Validate against known values
  tf::Task validate = taskflow.emplace([&]() {
    bool valid = validate_sequences(*rooted, *unrooted);
    std::cout << "\nValidation: " << (valid ? "PASSED" : "FAILED") << std::endl;
  }).name("Validate");
  
  // Task 5: Demonstrate calculation for specific values
  tf::Task demonstrate = taskflow.emplace([&]() {
    if (max_n >= 4) demonstrate_calculation(*rooted, 4);
    if (max_n >= 6) demonstrate_calculation(*rooted, 6);
  }).name("Demonstrate");
  
  // Set up dependencies
  compute_rooted.precede(compute_unrooted);
  compute_unrooted.precede(display, validate, demonstrate);
  
  // Execute the taskflow
  executor.run(taskflow).wait();
  
  // Dump the task graph
  std::cout << "\n=== Task Graph ===" << std::endl;
  taskflow.dump(std::cout);
  
  // Cleanup
  delete rooted;
  delete unrooted;
  
  return 0;
}
