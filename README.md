# 🧮 Simplex Solver (Big-M Method)

![C++](https://img.shields.io/badge/C++-11%2B-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A lightweight, terminal-based C++ application that solves linear programming problems using the **Simplex Method** with **Big-M penalty** support. 

## ✨ Features

- **Natural Math Input**: Just type equations like `max 3x1 - 4x2` and `2x1 + x2 <= 20` directly.
- **2D Visualization**: Automatically generates beautiful graphs of the feasible region and optimal point for 2-variable problems using Python and Matplotlib.
- **Maximization & Minimization**: Supports both optimization types dynamically derived from your input.
- **Flexible Constraints**: Handles `<=`, `>=`, and `=` constraints seamlessly.
- **Big-M Method Integration**: Automatically incorporates artificial variables and applies the Big-M penalty for `>=` and `=` constraints.
- **Clear Output**: Determines if a problem is unbounded or infeasible, and prints the optimal objective value along with the values for all decision variables.

## 🚀 Getting Started

### Prerequisites
- A standard C++ compiler (e.g., GCC, Clang, MSVC) supporting C++11 or higher.
- *(Optional)* Python 3 and `matplotlib` if you want to use the 2D visualizer for 2-variable problems. `pip install numpy matplotlib`

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
```text
--- Simplex Solver ---
Enter the objective function (e.g., 'max 3x1 - 4x2 + 5x3' or 'min x1 + 2x2'):
> max 3x1 + 4.5x2

Enter constraints one by one (e.g., '3x1 + x2 <= 10').
Type 'done' or press Enter on an empty line to start solving:
> 2x1 + x2 <= 20
> x1 - x2 >= 5
> 
Starting Big-M Simplex Solver...
```

## 📝 License

This project is licensed under the [MIT License](LICENSE).
