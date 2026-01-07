#include "parser/ExpressionParser.h"
#include <iostream>
#include <string>
#include <map>

int main() {
    std::cout << "CAS Calculator - Expression Parser Demo\n";
    std::cout << "=====================================\n\n";
    
    ExpressionParser parser;
    std::map<std::string, double> variables;
    
    std::cout << "Enter mathematical expressions (type 'quit' to exit):\n";
    std::cout << "Examples: 2 + 3, x * y, sin(3.14), sqrt(16)\n\n";
    
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        if (parser.parse(input)) {
            std::cout << "  AST: " << parser.toString() << std::endl;
            
            try {
                double result = parser.evaluate(variables);
                std::cout << "  Result: " << result << std::endl;
            } catch (const std::exception& e) {
                std::cout << "  Evaluation error: " << e.what() << std::endl;
            }
        } else {
            std::cout << "  Parse error: " << parser.getError() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    std::cout << "Goodbye!\n";
    return 0;
}
