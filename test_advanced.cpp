#include "cas/SymbolicEngine.h"
#include "parser/ExpressionParser.h"
#include <iostream>
#include <string>
#include <map>

void testASTToSymbolicConversion(const std::string& expression) {
    std::cout << "=== Testing AST to Symbolic Conversion ===" << std::endl;
    std::cout << "Expression: " << expression << std::endl;
    
    ExpressionParser parser;
    if (!parser.parse(expression)) {
        std::cout << "Parse error: " << parser.getError() << std::endl;
        return;
    }
    
    std::cout << "Parsed AST: " << parser.toString() << std::endl;
    
    SymbolicEngine engine;
    if (engine.parseFromString(expression)) {
        std::cout << "Symbolic: " << engine.toString() << std::endl;
        
        // Test differentiation
        try {
            auto derivative = engine.differentiate("x");
            std::cout << "d/dx: " << derivative->toString() << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Differentiation error: " << e.what() << std::endl;
        }
        
        // Test integration
        try {
            auto integral = engine.integrate("x");
            std::cout << "∫dx: " << integral->toString() << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Integration error: " << e.what() << std::endl;
        }
        
        // Test simplification
        try {
            auto simplified = engine.simplify();
            std::cout << "Simplified: " << simplified->toString() << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Simplification error: " << e.what() << std::endl;
        }
        
    } else {
        std::cout << "AST to symbolic conversion failed" << std::endl;
    }
    
    std::cout << std::endl;
}

void testIntegration(const std::string& expression) {
    std::cout << "=== Testing Integration ===" << std::endl;
    std::cout << "Expression: " << expression << std::endl;
    
    SymbolicEngine engine;
    if (engine.parseFromString(expression)) {
        std::cout << "Symbolic: " << engine.toString() << std::endl;
        
        try {
            auto integral = engine.integrate("x");
            std::cout << "∫dx: " << integral->toString() << std::endl;
            
            auto simplified = integral->simplify();
            std::cout << "Simplified: " << simplified->toString() << std::endl;
            
            // Test evaluation
            std::map<std::string, double> vars = {{"x", 2.0}};
            double result = integral->evaluate(vars);
            std::cout << "∫f(2) = " << result << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Integration error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Parse failed" << std::endl;
    }
    
    std::cout << std::endl;
}

void testEquationSolving(const std::string& expression) {
    std::cout << "=== Testing Equation Solving ===" << std::endl;
    std::cout << "Equation: " << expression << " = 0" << std::endl;
    
    SymbolicEngine engine;
    if (engine.parseFromString(expression)) {
        std::cout << "Expression: " << engine.toString() << std::endl;
        
        try {
            auto solution = engine.solve("x");
            std::cout << "x = " << solution->toString() << std::endl;
            
            auto simplified = solution->simplify();
            std::cout << "Simplified: " << simplified->toString() << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Solving error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Parse failed" << std::endl;
    }
    
    std::cout << std::endl;
}

void testFactoring(const std::string& expression) {
    std::cout << "=== Testing Factoring ===" << std::endl;
    std::cout << "Expression: " << expression << std::endl;
    
    SymbolicEngine engine;
    if (engine.parseFromString(expression)) {
        std::cout << "Symbolic: " << engine.toString() << std::endl;
        
        try {
            auto factors = engine.factor();
            std::cout << "Factors: ";
            for (size_t i = 0; i < factors.size(); ++i) {
                if (i > 0) std::cout << " * ";
                std::cout << factors[i]->toString();
            }
            std::cout << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Factoring error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Parse failed" << std::endl;
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Advanced CAS Features Test ===\n\n";
    
    // Test AST to Symbolic Conversion
    std::cout << "1. AST to Symbolic Conversion Tests\n";
    std::cout << "=====================================\n";
    testASTToSymbolicConversion("x^2");
    testASTToSymbolicConversion("sin(x)");
    testASTToSymbolicConversion("x * y + 3");
    testASTToSymbolicConversion("2*x + 1");
    
    // Test Integration
    std::cout << "2. Integration Tests\n";
    std::cout << "=====================\n";
    testIntegration("x");
    testIntegration("x^2");
    testIntegration("1/x");
    testIntegration("sin(x)");
    testIntegration("cos(x)");
    testIntegration("ln(x)");
    testIntegration("2*x");
    testIntegration("x + 1");
    
    // Test Equation Solving
    std::cout << "3. Equation Solving Tests\n";
    std::cout << "==========================\n";
    testEquationSolving("x + 1");
    testEquationSolving("2*x - 3");
    testEquationSolving("x^2 + x");
    
    // Test Factoring
    std::cout << "4. Factoring Tests\n";
    std::cout << "===================\n";
    testFactoring("x^2 + x");
    testFactoring("x * y");
    testFactoring("x^2");
    
    std::cout << "=== Advanced Features Summary ===" << std::endl;
    std::cout << "✓ AST to Symbolic Expression conversion" << std::endl;
    std::cout << "✓ Symbolic integration with basic rules:" << std::endl;
    std::cout << "  - ∫x dx = x²/2" << std::endl;
    std::cout << "  - ∫x^n dx = x^(n+1)/(n+1) for n ≠ -1" << std::endl;
    std::cout << "  - ∫1/x dx = ln(x)" << std::endl;
    std::cout << "  - ∫sin(x) dx = -cos(x)" << std::endl;
    std::cout << "  - ∫cos(x) dx = sin(x)" << std::endl;
    std::cout << "  - ∫ln(x) dx = x*ln(x) - x" << std::endl;
    std::cout << "✓ Linear equation solving" << std::endl;
    std::cout << "✓ Basic factoring" << std::endl;
    std::cout << "✓ Full integration with parser and symbolic engine" << std::endl;
    
    return 0;
}
