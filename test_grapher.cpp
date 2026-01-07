#include "grapher/Grapher.h"
#include <iostream>

void demonstrateGrapher() {
    std::cout << "=== CAS Grapher Demo ===" << std::endl;
    std::cout << "Creating a grapher with multiple functions..." << std::endl;
    
    // Create grapher with custom settings
    Grapher::PlotSettings settings;
    settings.width = 1000;
    settings.height = 700;
    settings.title = "CAS Grapher - Function Plotting Demo";
    settings.xMin = -5.0;
    settings.xMax = 5.0;
    settings.yMin = -5.0;
    settings.yMax = 5.0;
    settings.backgroundColor = sf::Color(240, 240, 240); // Light gray background
    
    Grapher grapher(settings);
    
    // Add various functions to plot
    std::cout << "Adding functions to plot..." << std::endl;
    
    // Basic functions
    grapher.addFunction("x", "y = x", sf::Color::Blue);
    grapher.addFunction("x^2", "y = x²", sf::Color::Red);
    grapher.addFunction("sin(x)", "y = sin(x)", sf::Color::Green);
    grapher.addFunction("cos(x)", "y = cos(x)", sf::Color::Magenta);
    
    // More complex functions
    grapher.addFunction("x^3 - 2*x", "y = x³ - 2x", sf::Color(255, 165, 0)); // Orange
    grapher.addFunction("1/x", "y = 1/x", sf::Color::Cyan);
    grapher.addFunction("exp(-x^2/2)", "y = e^(-x²/2)", sf::Color(128, 0, 128)); // Purple
    
    std::cout << "Functions added successfully!" << std::endl;
    std::cout << "Press ESC to close the window." << std::endl;
    std::cout << "Starting grapher..." << std::endl;
    
    // Run the grapher
    grapher.run();
    
    std::cout << "Grapher closed." << std::endl;
}

void demonstrateInteractiveGrapher() {
    std::cout << "\n=== Interactive Grapher Demo ===" << std::endl;
    std::cout << "This demo allows you to enter functions interactively." << std::endl;
    
    Grapher::PlotSettings settings;
    settings.width = 800;
    settings.height = 600;
    settings.title = "CAS Grapher - Interactive Mode";
    settings.xMin = -10.0;
    settings.xMax = 10.0;
    settings.yMin = -10.0;
    settings.yMax = 10.0;
    
    Grapher grapher(settings);
    
    std::string input;
    std::cout << "Enter functions to plot (type 'quit' to exit):" << std::endl;
    std::cout << "Example functions: x^2, sin(x), exp(x), 1/x, etc." << std::endl;
    
    while (true) {
        std::cout << "\nEnter function expression: ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        // Generate a color based on the number of functions
        sf::Color color;
        switch (grapher.getFunctionCount() % 6) {
            case 0: color = sf::Color::Blue; break;
            case 1: color = sf::Color::Red; break;
            case 2: color = sf::Color::Green; break;
            case 3: color = sf::Color::Magenta; break;
            case 4: color = sf::Color(255, 165, 0); break; // Orange
            case 5: color = sf::Color::Cyan; break;
        }
        
        if (grapher.addFunction(input, "y = " + input, color)) {
            std::cout << "Function added successfully!" << std::endl;
            std::cout << "Opening grapher window..." << std::endl;
            grapher.run();
        } else {
            std::cout << "Failed to parse function. Please check the syntax." << std::endl;
        }
    }
}

int main() {
    try {
        std::cout << "CAS Grapher Test Program" << std::endl;
        std::cout << "=========================" << std::endl;
        
        int choice;
        std::cout << "Choose demo mode:" << std::endl;
        std::cout << "1. Predefined functions demo" << std::endl;
        std::cout << "2. Interactive function input" << std::endl;
        std::cout << "Enter choice (1 or 2): ";
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline
        
        if (choice == 1) {
            demonstrateGrapher();
        } else if (choice == 2) {
            demonstrateInteractiveGrapher();
        } else {
            std::cout << "Invalid choice. Running predefined demo." << std::endl;
            demonstrateGrapher();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
