#include "grapher/ConsoleGrapher.h"
#include "cas/SymbolicEngine.h"
#include <iostream>

void demonstrateIntegratedCAS() {
    std::cout << "=== Integrated CAS Grapher Demo ===" << std::endl;
    std::cout << "This demo shows the integration between parser, symbolic engine, and grapher." << std::endl;
    
    // Create console grapher
    ConsoleGrapher::PlotSettings settings;
    settings.width = 80;
    settings.height = 30;
    settings.xMin = -5.0;
    settings.xMax = 5.0;
    settings.yMin = -5.0;
    settings.yMax = 5.0;
    
    ConsoleGrapher grapher(settings);
    SymbolicEngine symbolicEngine;
    
    // Original function
    std::string originalFunc = "x^2 + 2*x + 1";
    std::cout << "\n1. Original function: " << originalFunc << std::endl;
    
    // Parse and convert to symbolic
    ExpressionParser parser;
    if (parser.parse(originalFunc)) {
        if (symbolicEngine.parseFromAST(parser.getAST())) {
            std::cout << "   Symbolic representation: " << symbolicEngine.toString() << std::endl;
            
            // Differentiate
            auto derivative = symbolicEngine.differentiate("x");
            std::cout << "   Derivative: " << derivative->toString() << std::endl;
            
            // Integrate
            auto integral = symbolicEngine.integrate("x");
            std::cout << "   Integral: " << integral->toString() << std::endl;
            
            // Add functions to plot
            grapher.addFunction(originalFunc, "Original", '1');
            grapher.addFunction(derivative->toString(), "Derivative", '2');
            grapher.addFunction(integral->toString(), "Integral", '3');
        }
    }
    
    // Plot the functions
    std::cout << "\n2. Plotting original function, its derivative, and integral:" << std::endl;
    grapher.plot();
    
    // Clear and try another example
    grapher.clearFunctions();
    
    std::cout << "\n3. Trigonometric function example:" << std::endl;
    std::string trigFunc = "sin(x) * cos(x)";
    std::cout << "   Original function: " << trigFunc << std::endl;
    
    if (parser.parse(trigFunc)) {
        if (symbolicEngine.parseFromAST(parser.getAST())) {
            std::cout << "   Symbolic representation: " << symbolicEngine.toString() << std::endl;
            
            // Differentiate
            auto derivative = symbolicEngine.differentiate("x");
            std::cout << "   Derivative: " << derivative->toString() << std::endl;
            
            // Add functions to plot
            grapher.addFunction(trigFunc, "Original", '1');
            grapher.addFunction(derivative->toString(), "Derivative", '2');
        }
    }
    
    // Plot with different range for better visibility
    grapher.setPlotRange(-3.14, 3.14, -2.0, 2.0);
    std::cout << "\n4. Plotting trigonometric function and its derivative:" << std::endl;
    grapher.plot();
}

void demonstrateEquationSolving() {
    std::cout << "\n=== Equation Solving Demo ===" << std::endl;
    
    ConsoleGrapher::PlotSettings settings;
    settings.width = 60;
    settings.height = 20;
    settings.xMin = -5.0;
    settings.xMax = 5.0;
    settings.yMin = -5.0;
    settings.yMax = 5.0;
    
    ConsoleGrapher grapher(settings);
    SymbolicEngine symbolicEngine;
    
    // Solve equation: 2x + 3 = 0
    std::string equation = "2*x + 3";
    std::cout << "Solving equation: " << equation << " = 0" << std::endl;
    
    ExpressionParser parser;
    if (parser.parse(equation)) {
        if (symbolicEngine.parseFromAST(parser.getAST())) {
            try {
                auto solution = symbolicEngine.solve("x");
                if (solution) {
                    std::cout << "   Solution: x = " << solution->toString() << std::endl;
                    
                    // Plot the function and show where it crosses zero
                    grapher.addFunction(equation, "2x + 3", '*');
                    grapher.addFunction("0", "y = 0", '+');
                    
                    std::cout << "\nPlotting the function and y = 0 line:" << std::endl;
                    grapher.plot();
                }
            } catch (const std::exception& e) {
                std::cout << "   Note: Equation solving is limited in the current implementation." << std::endl;
                std::cout << "   Plotting the function instead to show the x-intercept:" << std::endl;
                
                // Just plot the function to visualize where it crosses zero
                grapher.addFunction(equation, "2x + 3", '*');
                grapher.addFunction("0", "y = 0", '-');
                grapher.plot();
                
                std::cout << "\n   The x-intercept (solution) is where the curve crosses y = 0" << std::endl;
                std::cout << "   For 2x + 3 = 0, the solution is x = -1.5" << std::endl;
            }
        }
    }
}

void demonstrateFactoring() {
    std::cout << "\n=== Factoring Demo ===" << std::endl;
    
    ConsoleGrapher::PlotSettings settings;
    settings.width = 60;
    settings.height = 20;
    settings.xMin = -3.0;
    settings.xMax = 3.0;
    settings.yMin = -5.0;
    settings.yMax = 5.0;
    
    ConsoleGrapher grapher(settings);
    SymbolicEngine symbolicEngine;
    
    // Factor expression: x^2 + x
    std::string expression = "x^2 + x";
    std::cout << "Factoring expression: " << expression << std::endl;
    
    ExpressionParser parser;
    if (parser.parse(expression)) {
        if (symbolicEngine.parseFromAST(parser.getAST())) {
            auto factors = symbolicEngine.factor();
            if (!factors.empty()) {
                std::cout << "   Factored form: " << factors[0]->toString() << std::endl;
                
                // Plot original and factored forms
                grapher.addFunction(expression, "Original", '1');
                grapher.addFunction(factors[0]->toString(), "Factored", '2');
                
                std::cout << "\nPlotting original and factored forms:" << std::endl;
                grapher.plot();
            }
        }
    }
}

int main() {
    try {
        std::cout << "CAS Grapher Integration Demo" << std::endl;
        std::cout << "============================" << std::endl;
        
        int choice;
        std::cout << "Choose demo:" << std::endl;
        std::cout << "1. Integrated CAS demo (differentiation, integration)" << std::endl;
        std::cout << "2. Equation solving demo" << std::endl;
        std::cout << "3. Factoring demo" << std::endl;
        std::cout << "4. All demos" << std::endl;
        std::cout << "Enter choice (1-4): ";
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline
        
        switch (choice) {
            case 1:
                demonstrateIntegratedCAS();
                break;
            case 2:
                demonstrateEquationSolving();
                break;
            case 3:
                demonstrateFactoring();
                break;
            case 4:
                demonstrateIntegratedCAS();
                demonstrateEquationSolving();
                demonstrateFactoring();
                break;
            default:
                std::cout << "Invalid choice. Running integrated demo." << std::endl;
                demonstrateIntegratedCAS();
                break;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
