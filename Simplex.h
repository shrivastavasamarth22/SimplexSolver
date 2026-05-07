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
    bool runSimplexPhase();
    void printResults();

    std::vector<double> originalObjective;
    int artificialStartCol;
    int artificialCount;

    std::vector<double> optimalVariables;
    std::vector<double> shadowPrices;
    std::vector<int> constraintSlackCols;
    double optimalObjective;
    bool isUnbounded;
    bool isInfeasible;
    bool quiet;

public:
    Simplex(std::vector<double> obj, std::vector<Constraint> constraints, bool isMin, bool quiet = false);
    void solve();
    std::vector<double> getOptimalVariables() { return optimalVariables; }
    std::vector<double> getShadowPrices() { return shadowPrices; }
    double getOptimalObjective() { return optimalObjective; }
    bool getIsUnbounded() { return isUnbounded; }
    bool getIsInfeasible() { return isInfeasible; }
};

#endif // SIMPLEX_H