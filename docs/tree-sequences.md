# Tree Sequences: From Rooted to Unrooted Trees

This document explains the mathematical theory and implementation behind the `tree_sequences` example, which demonstrates the conversion from OEIS sequence A000081 (unlabeled rooted trees) to A000055 (unlabeled unrooted trees) using the dissymmetry theorem.

## Overview

The example computes two fundamental sequences in combinatorics:
- **A000081**: Number of unlabeled rooted trees with n nodes
- **A000055**: Number of unlabeled unrooted trees with n nodes

## Mathematical Background

### 1. Rooted Trees (A000081)

An **unlabeled rooted tree** is a tree with a distinguished root vertex, where two trees are considered the same if there's a graph isomorphism preserving the root.

The sequence begins: 1, 1, 2, 4, 9, 20, 48, 115, 286, 719, ...

#### Generating Function

Let R(x) = Σ_{n≥1} r_n x^n be the ordinary generating function (OGF) for rooted trees. Pólya's enumeration theorem gives the functional equation:

```
R(x) = x · exp(Σ_{k≥1} R(x^k)/k)
```

Equivalently:
```
R(x) = x · ∏_{m≥1} (1 - x^m)^{-r_m}
```

#### Recurrence Formula

From the functional equation, we can derive a recurrence relation for the coefficients:

```
a(n+1) = (1/n) · Σ_{k=1}^{n} (Σ_{d|k} d·a(d)) · a(n-k+1)
```

Or in alternative indexing:
```
r_n = (1/(n-1)) · Σ_{k=1}^{n-1} (Σ_{d|k} d·r_d) · r_{n-k}
```

This recurrence allows efficient computation of the sequence.

### 2. Unrooted Trees (A000055)

An **unlabeled unrooted tree** is a tree without a distinguished root, where two trees are considered the same if there's a graph isomorphism between them.

The sequence begins: 1, 1, 1, 2, 3, 6, 11, 23, 47, 106, ...

Note that for n≥2, unrooted trees are less numerous than rooted trees because multiple rootings can give the same rooted tree.

### 3. The Dissymmetry Theorem

The **dissymmetry theorem for trees** provides a beautiful relationship between rooted and unrooted structures:

```
(unrooted at nothing) = (rooted at a vertex) + (rooted at an edge) - (rooted at a directed edge)
```

#### Translation to Generating Functions

Let's translate each term:

1. **Vertex-rooted**: V(x) = R(x)
   - Simply the rooted tree sequence

2. **Edge-rooted (undirected)**: E(x) = (1/2)(R(x)² + R(x²))
   - Glue two rooted trees at their roots, unordered
   - The R(x²) term corrects for symmetric pairs

3. **Directed-edge-rooted**: A(x) = R(x)²
   - Glue two rooted trees at their roots, ordered

#### The Main Formula

Plugging into the dissymmetry equation:

```
U(x) = V(x) + E(x) - A(x)
     = R(x) + (1/2)(R(x)² + R(x²)) - R(x)²
     = R(x) - (1/2)(R(x)² - R(x²))     ★
```

This is the fundamental generating function relationship.

### 4. Coefficient Form

Extracting the coefficient of x^n from equation (★):

```
u_n = r_n - (1/2)(Σ_{i=1}^{n-1} r_i·r_{n-i} - δ_{2|n}·r_{n/2})
```

where:
- The sum Σ r_i·r_{n-i} is the convolution from R(x)²
- δ_{2|n} = 1 if n is even, 0 otherwise
- The term r_{n/2} corrects for symmetric edge-rootings

## Implementation Details

### Computing Rooted Trees

```cpp
class RootedTreeSequence {
  void compute(size_t max_n) {
    for (size_t n = 1; n < max_n; n++) {
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
};
```

### Computing Unrooted Trees

```cpp
class UnrootedTreeSequence {
  void compute(const RootedTreeSequence& rooted, size_t max_n) {
    for (size_t n = 1; n <= max_n; n++) {
      long long r_n = rooted[n];
      
      // Compute convolution: sum r_i * r_{n-i}
      long long convolution = 0;
      for (size_t i = 1; i < n; i++) {
        convolution += rooted[i] * rooted[n - i];
      }
      
      // Correction for even n
      long long correction = (n % 2 == 0) ? rooted[n / 2] : 0;
      
      // Apply dissymmetry formula
      u[n] = r_n - (convolution - correction) / 2;
    }
  }
};
```

## Worked Examples

### Example: n = 4

Given r = [0, 1, 1, 2, 4, ...]:

1. **Convolution**:
   - r₁·r₃ = 1·2 = 2
   - r₂·r₂ = 1·1 = 1
   - r₃·r₁ = 2·1 = 2
   - Total = 5

2. **Correction** (n=4 is even):
   - r_{4/2} = r₂ = 1

3. **Result**:
   - u₄ = r₄ - (1/2)(5 - 1) = 4 - 2 = 2

### Example: n = 6

Given r = [0, 1, 1, 2, 4, 9, 20, ...]:

1. **Convolution**:
   - r₁·r₅ = 1·9 = 9
   - r₂·r₄ = 1·4 = 4
   - r₃·r₃ = 2·2 = 4
   - r₄·r₂ = 4·1 = 4
   - r₅·r₁ = 9·1 = 9
   - Total = 30

2. **Correction** (n=6 is even):
   - r_{6/2} = r₃ = 2

3. **Result**:
   - u₆ = r₆ - (1/2)(30 - 2) = 20 - 14 = 6

## Using the Example

### Basic Usage

```bash
# Compute sequences up to n=10 (default is 20)
./tree_sequences 10
```

### Output

The program will:
1. Compute the rooted tree sequence (A000081)
2. Derive the unrooted tree sequence (A000055)
3. Display both sequences
4. Validate against known OEIS values
5. Show step-by-step calculations for n=4 and n=6
6. Display the Taskflow task graph

### Sample Output

```
=== Tree Sequence Generation ===
Computing sequences up to n = 10

A000081 (rooted trees): 1, 1, 2, 4, 9, 20, 48, 115, 286, 719
A000055 (unrooted trees): 1, 1, 1, 2, 3, 6, 11, 23, 47, 106

=== Validation ===
   n      rooted    expected    unrooted    expected    status
--------------------------------------------------------------
   1           1           1           1           1        OK
   2           1           1           1           1        OK
   ...
  10         719         719         106         106        OK

Validation: PASSED
```

## Taskflow Integration

The example demonstrates Taskflow's capabilities by:

1. **Parallel Task Execution**: Computing rooted trees can be independent from setting up the validation data
2. **Task Dependencies**: Unrooted tree computation depends on rooted trees being computed first
3. **Multiple Dependent Tasks**: Display, validation, and demonstration all depend on the unrooted computation
4. **Task Graph Visualization**: The program outputs a DOT format graph showing task dependencies

### Task Graph Structure

```
Compute Rooted
      ↓
Compute Unrooted
      ↓
  ┌───┼───┐
  ↓   ↓   ↓
Display Validate Demonstrate
```

## References

1. **OEIS A000081**: Unlabeled rooted trees
   - https://oeis.org/A000081

2. **OEIS A000055**: Unlabeled unrooted trees
   - https://oeis.org/A000055

3. **Pólya Enumeration Theorem**: 
   - Pólya, G. (1937). "Kombinatorische Anzahlbestimmungen für Gruppen, Graphen und chemische Verbindungen"

4. **Dissymmetry Theorem for Trees**:
   - Harary, F., & Palmer, E. M. (1973). "Graphical Enumeration"

5. **Generating Functions**:
   - Wilf, H. S. (1994). "Generatingfunctionology"

## Mathematical Insights

### Why the Correction Term?

The term R(x²) in the edge-rooting formula accounts for trees where both sides of the edge are isomorphic. When we form R(x)², we count ordered pairs of rooted trees. But for an undirected edge, we want unordered pairs.

For most pairs (A,B) with A≠B, the ordered pairs (A,B) and (B,A) both appear in R(x)², so dividing by 2 gives the correct count. But for pairs (A,A), there's only one ordered pair in R(x)², and dividing by 2 would undercount. The R(x²) term adds back exactly these symmetric cases.

### Complexity

- **Time**: O(n² log n) for computing n terms
  - O(n²) for the main recurrences
  - O(log n) average for divisor sums
  
- **Space**: O(n) for storing the sequences

### Numerical Stability

For large n, the values grow exponentially. The current implementation uses `long long` which handles approximately n ≤ 25. For larger values, consider:
- Using arbitrary precision arithmetic (e.g., GMP)
- Working with logarithms for approximate values
- Computing modulo a prime for certain applications

## Extensions

Potential extensions to this example:

1. **Free Trees with Labeled Vertices**: Sequence A000272 (Cayley's formula: n^{n-2})
2. **Binary Trees**: Catalan numbers (A000108)
3. **Plane Trees**: A000108 (also Catalan numbers)
4. **k-ary Trees**: Generalizations of rooted trees
5. **Trees with Constraints**: Bounded degree, height restrictions, etc.

## Conclusion

This example demonstrates the power of generating functions and the dissymmetry theorem in combinatorial enumeration. The relationship between rooted and unrooted trees is a beautiful example of how algebraic manipulations of generating functions translate to concrete combinatorial identities.

The Taskflow implementation shows how computational mathematics problems can benefit from parallel task execution and dependency management, even for problems that might seem inherently sequential.
