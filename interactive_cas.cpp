#include "cas/SymbolicEngine.h"
#include "parser/ExpressionParser.h"
#include "grapher/Grapher.h"
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <limits>
#include <cmath>

void printHelp() {
    std::cout << "\n=== Interactive CAS Menu ===" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  parse <expr>           - Parse and display expression" << std::endl;
    std::cout << "  eval <expr>            - Evaluate expression (e.g., 'eval 2+3')" << std::endl;
    std::cout << "  diff <expr>            - Differentiate with respect to x" << std::endl;
    std::cout << "  integrate <expr>       - Integrate with respect to x" << std::endl;
    std::cout << "  simplify <expr>        - Simplify expression" << std::endl;
    std::cout << "  all <expr>             - Show parse, derivative, and integral" << std::endl;
    std::cout << "  graph <expr>           - Graph function in a GUI window" << std::endl;
    std::cout << "  graph <expr> [options] - Graph with custom range/size (GUI)" << std::endl;
    std::cout << "  help                   - Show this menu" << std::endl;
    std::cout << "  quit/exit              - Exit program" << std::endl;
    std::cout << "\nGraph options (after expression):" << std::endl;
    std::cout << "  xmin:<value> xmax:<value> ymin:<value> ymax:<value>" << std::endl;
    std::cout << "  width:<cols> height:<rows>" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  diff x^2 + 2*x + 1" << std::endl;
    std::cout << "  integrate sin(x)" << std::endl;
    std::cout << "  eval sin(3.14159/2)" << std::endl;
    std::cout << "  simplify x + x + x" << std::endl;
    std::cout << "  graph x^2" << std::endl;
    std::cout << "  graph sin(x) xmin:-6.28 xmax:6.28 ymin:-2 ymax:2" << std::endl;
    std::cout << "============================\n" << std::endl;
}

int main() {
    std::cout << "╔════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║     Interactive Computer Algebra System        ║" << std::endl;
    std::cout << "║     (Type 'help' for available commands)        ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
    
    printHelp();
    
    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        
        if (line.empty()) {
            continue;
        }
        
        // Parse command and arguments
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        // Get the rest of the line as the expression
        std::string expression;
        std::getline(iss, expression);
        
        // Trim leading whitespace from expression
        size_t start = expression.find_first_not_of(" \t");
        if (start != std::string::npos) {
            expression = expression.substr(start);
        }
        
        // Handle commands
        if (command == "quit" || command == "exit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        } 
        else if (command == "help") {
            printHelp();
        }
        else if (command == "parse") {
            if (expression.empty()) {
                std::cout << "Error: Please provide an expression" << std::endl;
                continue;
            }
            
            ExpressionParser parser;
            if (parser.parse(expression)) {
                std::cout << "✓ Parsed successfully" << std::endl;
                std::cout << "  AST: " << parser.toString() << std::endl;
            } else {
                std::cout << "✗ Parse error: " << parser.getError() << std::endl;
            }
        }
        else if (command == "eval") {
            if (expression.empty()) {
                std::cout << "Error: Please provide an expression" << std::endl;
                continue;
            }
            
            ExpressionParser parser;
            if (parser.parse(expression)) {
                try {
                    std::map<std::string, double> vars;
                    double result = parser.evaluate(vars);
                    std::cout << "  Result: " << result << std::endl;
                } catch (const std::exception& e) {
                    std::cout << "✗ Evaluation error: " << e.what() << std::endl;
                }
            } else {
                std::cout << "✗ Parse error: " << parser.getError() << std::endl;
            }
        }
        else if (command == "diff") {
            if (expression.empty()) {
                std::cout << "Error: Please provide an expression" << std::endl;
                continue;
            }
            
            try {
                SymbolicEngine engine;
                if (engine.parseFromString(expression)) {
                    auto derivative = engine.differentiate("x");
                    std::cout << "  d/dx of (" << expression << "):" << std::endl;
                    std::cout << "  = " << derivative->toString() << std::endl;
                    
                    auto simplified = derivative->simplify();
                    std::cout << "  Simplified: " << simplified->toString() << std::endl;
                } else {
                    std::cout << "✗ Parse error" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "✗ Error: " << e.what() << std::endl;
            }
        }
        else if (command == "integrate") {
            if (expression.empty()) {
                std::cout << "Error: Please provide an expression" << std::endl;
                continue;
            }
            
            try {
                SymbolicEngine engine;
                if (engine.parseFromString(expression)) {
                    auto integral = engine.integrate("x");
                    std::cout << "  ∫(" << expression << ")dx:" << std::endl;
                    std::cout << "  = " << integral->toString() << std::endl;
                    
                    auto simplified = integral->simplify();
                    std::cout << "  Simplified: " << simplified->toString() << " + C" << std::endl;
                } else {
                    std::cout << "✗ Parse error" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "✗ Error: " << e.what() << std::endl;
            }
        }
        else if (command == "simplify") {
            if (expression.empty()) {
                std::cout << "Error: Please provide an expression" << std::endl;
                continue;
            }
            
            try {
                SymbolicEngine engine;
                if (engine.parseFromString(expression)) {
                    auto simplified = engine.simplify();
                    std::cout << "  Original:  " << expression << std::endl;
                    std::cout << "  Simplified: " << simplified->toString() << std::endl;
                } else {
                    std::cout << "✗ Parse error" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "✗ Error: " << e.what() << std::endl;
            }
        }
        else if (command == "all") {
            if (expression.empty()) {
                std::cout << "Error: Please provide an expression" << std::endl;
                continue;
            }
            
            std::cout << "\n" << std::string(50, '=') << std::endl;
            std::cout << "Expression: " << expression << std::endl;
            std::cout << std::string(50, '=') << std::endl;
            
            try {
                SymbolicEngine engine;
                if (!engine.parseFromString(expression)) {
                    std::cout << "✗ Parse error" << std::endl;
                    continue;
                }
                
                std::cout << "1. Parsed form:" << std::endl;
                std::cout << "   " << engine.toString() << std::endl;
                
                // Derivative
                try {
                    auto derivative = engine.differentiate("x");
                    std::cout << "\n2. Derivative (d/dx):" << std::endl;
                    std::cout << "   " << derivative->toString() << std::endl;
                    auto simplifiedDeriv = derivative->simplify();
                    std::cout << "   Simplified: " << simplifiedDeriv->toString() << std::endl;
                } catch (const std::exception& e) {
                    std::cout << "\n2. Derivative: Error - " << e.what() << std::endl;
                }
                
                // Integral
                try {
                    auto integral = engine.integrate("x");
                    std::cout << "\n3. Integral (∫dx):" << std::endl;
                    std::cout << "   " << integral->toString() << std::endl;
                    auto simplifiedIntegral = integral->simplify();
                    std::cout << "   Simplified: " << simplifiedIntegral->toString() << " + C" << std::endl;
                } catch (const std::exception& e) {
                    std::cout << "\n3. Integral: Error - " << e.what() << std::endl;
                }
                
                std::cout << std::string(50, '=') << std::endl << std::endl;
            } catch (const std::exception& e) {
                std::cout << "✗ Error: " << e.what() << std::endl;
            }
        }
        else if (command == "graph") {
            if (expression.empty()) {
                std::cout << "Error: Please provide an expression" << std::endl;
                continue;
            }
            
            try {
                // Parse graph options from the rest of the line
                Grapher::PlotSettings settings;
                settings.width = 80;
                settings.height = 25;
                settings.xMin = -10.0;
                settings.xMax = 10.0;
                settings.yMin = -10.0;
                settings.yMax = 10.0;
                
                // Find where options start (look for patterns like "xmin:", "xmax:", etc.)
                std::string expr = expression;
                std::string options;
                
                // Look for option keywords
                std::vector<std::string> optionKeywords = {"xmin:", "xmax:", "ymin:", "ymax:", "width:", "height:"};
                size_t optionStart = std::string::npos;
                
                for (const auto& keyword : optionKeywords) {
                    size_t pos = expression.find(keyword);
                    if (pos != std::string::npos) {
                        if (optionStart == std::string::npos || pos < optionStart) {
                            optionStart = pos;
                        }
                    }
                }
                
                // Split expression and options
                if (optionStart != std::string::npos) {
                    // Find the start of the option (go back to find space before keyword)
                    size_t spaceBeforeOption = expression.rfind(' ', optionStart);
                    if (spaceBeforeOption != std::string::npos) {
                        expr = expression.substr(0, spaceBeforeOption);
                        options = expression.substr(spaceBeforeOption + 1);
                    }
                }
                
                // Flag to track if user specified y-range
                bool userSpecifiedYRange = false;
                
                // Parse options
                if (!options.empty()) {
                    std::istringstream optStream(options);
                    std::string opt;
                    while (optStream >> opt) {
                        size_t colonPos = opt.find(':');
                        if (colonPos != std::string::npos) {
                            std::string key = opt.substr(0, colonPos);
                            std::string value = opt.substr(colonPos + 1);
                            
                            try {
                                if (key == "xmin") settings.xMin = std::stod(value);
                                else if (key == "xmax") settings.xMax = std::stod(value);
                                else if (key == "ymin") { settings.yMin = std::stod(value); userSpecifiedYRange = true; }
                                else if (key == "ymax") { settings.yMax = std::stod(value); userSpecifiedYRange = true; }
                                else if (key == "width") settings.width = std::stoi(value);
                                else if (key == "height") settings.height = std::stoi(value);
                            } catch (...) {
                                std::cout << "Warning: Could not parse option " << opt << std::endl;
                            }
                        }
                    }
                }
                
                // Auto-adjust y-range if user didn't specify it
                if (!userSpecifiedYRange) {
                    std::cout << "Sampling function to auto-adjust y-range..." << std::endl;
                    
                    ExpressionParser parser;
                    if (parser.parse(expr)) {
                        double minY = std::numeric_limits<double>::max();
                        double maxY = std::numeric_limits<double>::lowest();
                        int samples = 100;
                        
                        // Sample the function across the x-range
                        for (int i = 0; i <= samples; i++) {
                            double x = settings.xMin + (settings.xMax - settings.xMin) * (i / static_cast<double>(samples));
                            std::map<std::string, double> vars = {{"x", x}};
                            
                            try {
                                double y = parser.evaluate(vars);
                                
                                // Skip inf and nan values
                                if (std::isfinite(y)) {
                                    minY = std::min(minY, y);
                                    maxY = std::max(maxY, y);
                                }
                            } catch (...) {
                                // Skip evaluation errors (e.g., domain issues)
                            }
                        }
                        
                        // Add padding to the y-range
                        if (std::isfinite(minY) && std::isfinite(maxY)) {
                            double padding = (maxY - minY) * 0.15; // 15% padding
                            if (padding < 1.0) padding = 1.0;  // Minimum padding
                            
                            settings.yMin = minY - padding;
                            settings.yMax = maxY + padding;
                            
                            std::cout << "Auto-adjusted y-range to [" << settings.yMin << ", " << settings.yMax << "]" << std::endl;
                        } else {
                            std::cout << "Warning: Could not sample function values. Using default y-range." << std::endl;
                        }
                    }
                }
                
                // Create and use GUI grapher (SFML)
                Grapher::PlotSettings gsettings;
                gsettings.width = settings.width * 10; // scale console cols->pixels
                gsettings.height = settings.height * 20; // scale console rows->pixels
                gsettings.xMin = settings.xMin;
                gsettings.xMax = settings.xMax;
                gsettings.yMin = settings.yMin;
                gsettings.yMax = settings.yMax;

                Grapher grapher(gsettings);
                // addFunction signature: (expression, name, color)
                if (grapher.addFunction(expr, expr)) {
                    std::cout << "\nOpening GUI grapher window for: " << expr << std::endl;
                    std::cout << "Range: x=[" << gsettings.xMin << ", " << gsettings.xMax 
                              << "], y=[" << gsettings.yMin << ", " << gsettings.yMax << "]" << std::endl;
                    std::cout << "Window: " << gsettings.width << "x" << gsettings.height << std::endl;
                    std::cout << std::string(50, '=') << std::endl;
                    grapher.run();
                    std::cout << std::string(50, '=') << std::endl;
                } else {
                    std::cout << "✗ Failed to add function to GUI grapher" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "✗ Graphing error: " << e.what() << std::endl;
            }
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands" << std::endl;
        }
    }
    
    return 0;
}
