#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <regex>
#include <queue>
#include <cmath>
#include "Simplex.h"

using namespace std;

int main() {
    cout << "--- Simplex Solver ---" << endl;
    cout << "Enter the objective function (e.g., 'max 3x1 - 4x2 + 5x3' or 'min x1 + 2x2'):\n> ";
    string objLine;
    getline(cin, objLine);

    bool isMinimization = false;
    string lowerObj = objLine;
    transform(lowerObj.begin(), lowerObj.end(), lowerObj.begin(), ::tolower);
    if (lowerObj.find("min") != string::npos) {
        isMinimization = true;
    }

    int maxVarIdx = 0;
    
    auto parseExpression = [&](const string& expr, vector<double>& coeffs) {
        regex termRegex(R"(([+-]?\s*\d*\.?\d*)\s*[xX](\d+))");
        sregex_iterator words_begin(expr.begin(), expr.end(), termRegex);
        sregex_iterator words_end;

        for (sregex_iterator i = words_begin; i != words_end; ++i) {
            smatch match = *i;
            string coeffStr = match[1].str();
            int varIdx = stoi(match[2].str()) - 1; // 1-based to 0-based
            
            coeffStr.erase(remove_if(coeffStr.begin(), coeffStr.end(), ::isspace), coeffStr.end());
            
            double coeff = 1.0;
            if (coeffStr == "-" || coeffStr == "+-") coeff = -1.0;
            else if (coeffStr == "+" || coeffStr == "") coeff = 1.0;
            else coeff = stod(coeffStr);
            
            maxVarIdx = max(maxVarIdx, varIdx + 1);
            if (varIdx >= coeffs.size()) coeffs.resize(varIdx + 1, 0.0);
            coeffs[varIdx] = coeff;
        }
    };

    vector<double> objFunc;
    parseExpression(objLine, objFunc);

    vector<Constraint> constraints;
    cout << "\nEnter constraints one by one (e.g., '3x1 + x2 <= 10')." << endl;
    cout << "Type 'done' or press Enter on an empty line to start solving:\n";
    
    while (true) {
        cout << "> ";
        string constLine;
        getline(cin, constLine);
        
        string lowerConst = constLine;
        transform(lowerConst.begin(), lowerConst.end(), lowerConst.begin(), ::tolower);
        
        // Trim whitespace from string
        lowerConst.erase(0, lowerConst.find_first_not_of(" \t\r\n"));
        if (lowerConst == "done" || lowerConst.empty()) break;

        regex opRegex(R"((<=|>=|<|>|=))");
        smatch opMatch;
        if (regex_search(constLine, opMatch, opRegex)) {
            Constraint c;
            c.op = opMatch[1].str();
            
            string lhs = constLine.substr(0, opMatch.position());
            string rhsStr = constLine.substr(opMatch.position() + opMatch.length());
            
            c.rhs = stod(rhsStr);
            parseExpression(lhs, c.coeffs);
            constraints.push_back(c);
        } else {
            cout << "Invalid constraint format. Expected an operator (<=, >=, <, >, =).\n";
        }
    }

    int numVars = maxVarIdx;
    
    // Ensure all vectors evaluate up to numVars
    objFunc.resize(numVars, 0.0);
    for (auto& c : constraints) {
        c.coeffs.resize(numVars, 0.0);
    }

    cout << "\nDo you want to restrict variables to integers? (ILP Branch & Bound) [y/n]: ";
    string ilpChoice;
    getline(cin, ilpChoice);
    bool doILP = (ilpChoice == "y" || ilpChoice == "Y" || ilpChoice == "yes");

    if (doILP) {
        cout << "\n--- Starting Branch & Bound ILP Solver ---" << endl;
        
        struct Node {
            vector<Constraint> constraints;
            int depth;
        };

        queue<Node> q;
        q.push({constraints, 0});

        double bestZ = isMinimization ? 1e15 : -1e15;
        vector<double> bestVars;
        bool foundInteger = false;
        int nodesExplored = 0;

        while (!q.empty()) {
            Node curr = q.front();
            q.pop();
            nodesExplored++;

            Simplex solver(objFunc, curr.constraints, isMinimization, true);
            solver.solve();

            if (solver.getIsInfeasible() || solver.getIsUnbounded()) continue;

            double currentZ = solver.getOptimalObjective();
            
            // Prune branch if it's worse than the best integer solution found so far
            if (foundInteger) {
                if (isMinimization && currentZ >= bestZ) continue;
                if (!isMinimization && currentZ <= bestZ) continue;
            }

            vector<double> vars = solver.getOptimalVariables();
            
            // Check if all variables are integers
            int splitVar = -1;
            double splitVal = 0;
            for (int i = 0; i < numVars; i++) {
                double diff = abs(vars[i] - round(vars[i]));
                if (diff > 1e-5) {
                    splitVar = i;
                    splitVal = vars[i];
                    break; // Pick the first fractional variable
                }
            }

            if (splitVar == -1) {
                // We found a better perfect integer solution!
                bestZ = currentZ;
                bestVars = vars;
                foundInteger = true;
            } else {
                // Branching step
                // Branch 1: x <= floor(val)
                Node branch1 = curr;
                Constraint c1;
                c1.coeffs.resize(numVars, 0.0);
                c1.coeffs[splitVar] = 1.0;
                c1.op = "<=";
                c1.rhs = floor(splitVal);
                branch1.constraints.push_back(c1);
                branch1.depth++;
                q.push(branch1);

                // Branch 2: x >= ceil(val)
                Node branch2 = curr;
                Constraint c2;
                c2.coeffs.resize(numVars, 0.0);
                c2.coeffs[splitVar] = 1.0;
                c2.op = ">=";
                c2.rhs = ceil(splitVal);
                branch2.constraints.push_back(c2);
                branch2.depth++;
                q.push(branch2);
            }
        }

        cout << "\nBranch and Bound search completed!" << endl;
        cout << "Nodes explored: " << nodesExplored << endl;
        
        if (foundInteger) {
            cout << "\n=====================================" << endl;
            cout << "      Optimal Integer Solution!      " << endl;
            cout << "=====================================" << endl;
            cout << "Optimal Objective Value (Z): " << bestZ << "\n\n";
            cout << "--- Optimal Variable Values ---" << endl;
            for (int i = 0; i < numVars; i++) {
                cout << "x" << (i + 1) << " = " << bestVars[i] << endl;
            }
            cout << "-------------------------------" << endl;
        } else {
            cout << "No feasible integer solution exists!" << endl;
        }

    } else {
        cout << "\nStarting Two-Phase Simplex Solver..." << endl;
        
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
    }

    return 0;
}