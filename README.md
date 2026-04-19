# 🧮 Simplex Solver (Big-M Method)

![C++](https://img.shields.io/badge/C++-11%2B-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A lightweight, terminal-based C++ application that solves linear programming problems using the **Simplex Method** with **Big-M penalty** support. 

## ✨ Features

- **Maximization & Minimization**: Supports both optimization types.
- **Flexible Constraints**: Handles `<=`, `>=`, and `=` constraints seamlessly.
- **Big-M Method Integration**: Automatically incorporates artificial variables and applies the Big-M penalty for `>=` and `=` constraints.
- **Clear Output**: Determines if a problem is unbounded or infeasible, and prints the optimal objective value along with the values for all decision variables.

## 🚀 Getting Started

### Prerequisites
- A standard C++ compiler (e.g., GCC, Clang, MSVC) supporting C++11 or higher.

### Compilation
Run the following command in the project directory to build the executable:

```bash
g++ -o SimplexSolver main.cpp Simplex.cpp
```

### Usage
Execute the compiled binary:

```bash
./SimplexSolver
```

Follow the interactive prompts to define your objective function, variables, and constraints:
1. Choose to `maximise` or `minimise`.
2. Enter the number of variables and constraints.
3. Input the coefficients of the objective function.
4. Input the coefficients, operator, and Right-Hand Side (RHS) for each constraint iteratively.

## 📝 License

This project is licensed under the [MIT License](LICENSE).
