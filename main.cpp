#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include "Simplex.h"

using namespace std;

int main() {
    string optType;
    int numVars, numConstraints;

    cout << "Do you want to 'maximise' or 'minimise'? ";
    cin >> optType;
    
    transform(optType.begin(), optType.end(), optType.begin(), ::tolower);
    bool isMinimization = (optType == "minimise" || optType == "minimize" || optType == "min");

    cout << "Enter the number of variables (e.g., 2 for x1, x2): ";
    cin >> numVars;

    cout << "Enter the number of constraints: ";
    cin >> numConstraints;
    cin.ignore();

    vector<double> objFunc(numVars);
    cout << "\nType the coefficients of the initial problem separated by spaces." << endl;
    cout << "(Example: for 3x1 + 5x2, type '3 5'): ";
    string line;
    getline(cin, line);
    stringstream ssObj(line);
    for (int i = 0; i < numVars; i++) {
        ssObj >> objFunc[i];
    }

    vector<Constraint> constraints;
    cout << "\nFor each constraint, you will be asked for the coefficients, the operator, and the RHS." << endl;
    
    for (int i = 0; i < numConstraints; i++) {
        Constraint c;
        c.coeffs.resize(numVars);
        
        cout << "\n--- Constraint " << i + 1 << " ---" << endl;
        cout << "Coefficients (separated by spaces): ";
        getline(cin, line);
        stringstream ssConst(line);
        for (int j = 0; j < numVars; j++) {
            ssConst >> c.coeffs[j];
        }

        cout << "Operator (<=, <, >=, >, =): ";
        getline(cin, c.op);

        cout << "Right-Hand Side (RHS): ";
        string rhsStr;
        getline(cin, rhsStr);
        c.rhs = stod(rhsStr);

        constraints.push_back(c);
    }

    cout << "\nStarting Big-M Simplex Solver..." << endl;
    
    Simplex solver(objFunc, constraints, isMinimization);
    solver.solve();

    if (numVars == 2 && !solver.getIsUnbounded() && !solver.getIsInfeasible()) {
        cout << "\nExporting 2D data for visualization..." << endl;
        ofstream outFile("data.json");
        outFile << "{\n";
        outFile << "  \"isMinimization\": " << (isMinimization ? "true" : "false") << ",\n";
        
        outFile << "  \"objective\": [" << objFunc[0] << ", " << objFunc[1] << "],\n";
        
        outFile << "  \"constraints\": [\n";
        for (size_t i = 0; i < constraints.size(); ++i) {
            outFile << "    {\"coeffs\": [" << constraints[i].coeffs[0] << ", " << constraints[i].coeffs[1] << "], ";
            outFile << "\"op\": \"" << constraints[i].op << "\", ";
            outFile << "\"rhs\": " << constraints[i].rhs << "}";
            if (i < constraints.size() - 1) outFile << ",";
            outFile << "\n";
        }
        outFile << "  ],\n";

        vector<double> optVars = solver.getOptimalVariables();
        outFile << "  \"optimal_point\": [" << optVars[0] << ", " << optVars[1] << "],\n";
        outFile << "  \"optimal_value\": " << solver.getOptimalObjective() << "\n";
        outFile << "}\n";
        outFile.close();

        cout << "Launching Python visualizer..." << endl;
        system("python plot_simplex.py data.json");
    }

    return 0;
}