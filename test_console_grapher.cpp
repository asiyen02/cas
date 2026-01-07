#include "grapher/ConsoleGrapher.h"
#include <iostream>

void demonstrateConsoleGrapher() {
    std::cout << "=== Console Grapher Demo ===" << std::endl;
    std::cout << "Creating a console grapher with multiple functions..." << std::endl;
    
    // Create console grapher with custom settings
    ConsoleGrapher::PlotSettings settings;
    settings.width = 80;
    settings.height = 30;
    settings.xMin = -5.0;
    settings.xMax = 5.0;
    settings.yMin = -5.0;
    settings.yMax = 5.0;
    settings.gridChar = '.';
    settings.axesChar = '+';
    
    ConsoleGrapher grapher(settings);
    
    // Add various functions to plot
    std::cout << "Adding functions to plot..." << std::endl;
    
    // Basic functions
    grapher.addFunction("x", "y = x", '1');
    grapher.addFunction("x^2", "y = x²", '2');
    grapher.addFunction("sin(x)", "y = sin(x)", '3');
    grapher.addFunction("cos(x)", "y = cos(x)", '4');
    
    // More complex functions
    grapher.addFunction("x^3 - 2*x", "y = x³ - 2x", '5');
    grapher.addFunction("1/x", "y = 1/x", '6');
    grapher.addFunction("exp(-x^2/2)", "y = e^(-x²/2)", '7');
    
    std::cout << "Functions added successfully!" << std::endl;
    std::cout << "Plotting functions..." << std::endl;
    
    // Plot the functions
    grapher.plot();
    
    std::cout << "Console grapher demo completed." << std::endl;
}

void demonstrateInteractiveConsoleGrapher() {
    std::cout << "\n=== Interactive Console Grapher Demo ===" << std::endl;
    std::cout << "This demo allows you to enter functions interactively." << std::endl;
    
    ConsoleGrapher::PlotSettings settings;
    settings.width = 60;
    settings.height = 20;
    settings.xMin = -10.0;
    settings.xMax = 10.0;
    settings.yMin = -10.0;
    settings.yMax = 10.0;
    
    ConsoleGrapher grapher(settings);
    
    std::string input;
    std::cout << "Enter functions to plot (type 'quit' to exit):" << std::endl;
    std::cout << "Example functions: x^2, sin(x), exp(x), 1/x, etc." << std::endl;
    
    char symbols[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    int symbolIndex = 0;
    
    while (true) {
        std::cout << "\nEnter function expression: ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        char symbol = symbols[symbolIndex % 10];
        
        if (grapher.addFunction(input, "y = " + input, symbol)) {
            std::cout << "Function added successfully!" << std::endl;
            std::cout << "Plotting..." << std::endl;
            grapher.plot();
            symbolIndex++;
        } else {
            std::cout << "Failed to parse function. Please check the syntax." << std::endl;
        }
    }
}

void demonstrateDifferentRanges() {
    std::cout << "\n=== Different Plot Ranges Demo ===" << std::endl;
    
    ConsoleGrapher::PlotSettings settings;
    settings.width = 60;
    settings.height = 20;
    
    ConsoleGrapher grapher(settings);
    
    // Add a function
    grapher.addFunction("sin(x)", "y = sin(x)", '*');
    
    // Plot with different ranges
    std::cout << "Plotting sin(x) with range [-2π, 2π] x [-2, 2]:" << std::endl;
    grapher.setPlotRange(-6.28, 6.28, -2.0, 2.0);
    grapher.plot();
    
    std::cout << "\nPlotting sin(x) with range [-π, π] x [-1.5, 1.5]:" << std::endl;
    grapher.setPlotRange(-3.14, 3.14, -1.5, 1.5);
    grapher.plot();
    
    std::cout << "\nPlotting sin(x) with range [0, 4π] x [-1, 1]:" << std::endl;
    grapher.setPlotRange(0, 12.56, -1.0, 1.0);
    grapher.plot();
}

int main() {
    try {
        std::cout << "Console Grapher Test Program" << std::endl;
        std::cout << "=============================" << std::endl;
        
        int choice;
        std::cout << "Choose demo mode:" << std::endl;
        std::cout << "1. Predefined functions demo" << std::endl;
        std::cout << "2. Interactive function input" << std::endl;
        std::cout << "3. Different plot ranges demo" << std::endl;
        std::cout << "Enter choice (1, 2, or 3): ";
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline
        
        switch (choice) {
            case 1:
                demonstrateConsoleGrapher();
                break;
            case 2:
                demonstrateInteractiveConsoleGrapher();
                break;
            case 3:
                demonstrateDifferentRanges();
                break;
            default:
                std::cout << "Invalid choice. Running predefined demo." << std::endl;
                demonstrateConsoleGrapher();
                break;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
