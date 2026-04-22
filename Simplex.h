#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <vector>
#include <string>

struct Constraint {
    std::vector<double> coeffs;
    std::string op;
    double rhs;
};

class Simplex {
private:
    int rows, cols;
    int numVars;
    std::vector<std::vector<double>> tableau;
    bool isMinimization;
    double M;

    int findPivotColumn();
    int findPivotRow(int pivotCol);
    void pivot(int pivotRow, int pivotCol);
    void printResults();

    std::vector<double> optimalVariables;
    double optimalObjective;
    bool isUnbounded;
    bool isInfeasible;

public:
    Simplex(std::vector<double> obj, std::vector<Constraint> constraints, bool isMin);
    void solve();
    std::vector<double> getOptimalVariables() { return optimalVariables; }
    double getOptimalObjective() { return optimalObjective; }
    bool getIsUnbounded() { return isUnbounded; }
    bool getIsInfeasible() { return isInfeasible; }
};

#endif // SIMPLEX_H