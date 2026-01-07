#include "cas/SymbolicEngine.h"
#include "parser/ExpressionParser.h"
#include <iostream>
#include <string>
#include <map>

void demonstrateSymbolicComputation(const std::string& expression) {
    std::cout << "=== Expression: " << expression << " ===" << std::endl;
    
    // Parse the expression
    ExpressionParser parser;
    if (!parser.parse(expression)) {
        std::cout << "Parse error: " << parser.getError() << std::endl;
        return;
    }
    
    std::cout << "Parsed AST: " << parser.toString() << std::endl;
    
    // Create symbolic expression manually (for now)
    // In a full implementation, you'd convert the AST to symbolic expressions
    std::cout << "Note: Full AST to symbolic conversion not implemented yet" << std::endl;
    std::cout << "Creating symbolic expression manually..." << std::endl;
    
    // For demonstration, let's create some symbolic expressions
    if (expression == "x^2") {
        auto xSquared = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::POWER,
                                            makeSymbolicVariable("x"),
                                            makeSymbolicNumber(2.0));
        
        std::cout << "Symbolic: " << xSquared->toString() << std::endl;
        
        auto derivative = xSquared->differentiate("x");
        std::cout << "d/dx: " << derivative->toString() << std::endl;
        
        auto simplified = derivative->simplify();
        std::cout << "Simplified: " << simplified->toString() << std::endl;
        
        std::map<std::string, double> vars = {{"x", 3.0}};
        double result = xSquared->evaluate(vars);
        std::cout << "f(3) = " << result << std::endl;
        
    } else if (expression == "sin(x)") {
        auto sinX = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::SIN,
                                       makeSymbolicVariable("x"));
        
        std::cout << "Symbolic: " << sinX->toString() << std::endl;
        
        auto derivative = sinX->differentiate("x");
        std::cout << "d/dx: " << derivative->toString() << std::endl;
        
        auto simplified = derivative->simplify();
        std::cout << "Simplified: " << simplified->toString() << std::endl;
        
        std::map<std::string, double> vars = {{"x", 1.0}};
        double result = sinX->evaluate(vars);
        std::cout << "f(1) = " << result << std::endl;
        
    } else if (expression == "x * y") {
        auto xTimesY = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                           makeSymbolicVariable("x"),
                                           makeSymbolicVariable("y"));
        
        std::cout << "Symbolic: " << xTimesY->toString() << std::endl;
        
        auto derivativeX = xTimesY->differentiate("x");
        std::cout << "d/dx: " << derivativeX->toString() << std::endl;
        
        auto derivativeY = xTimesY->differentiate("y");
        std::cout << "d/dy: " << derivativeY->toString() << std::endl;
        
        std::map<std::string, double> vars = {{"x", 2.0}, {"y", 3.0}};
        double result = xTimesY->evaluate(vars);
        std::cout << "f(2,3) = " << result << std::endl;
        
    } else {
        std::cout << "Manual symbolic expression not implemented for this expression" << std::endl;
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "=== CAS Symbolic Engine Demo ===\n\n";
    
    // Test some expressions
    demonstrateSymbolicComputation("x^2");
    demonstrateSymbolicComputation("sin(x)");
    demonstrateSymbolicComputation("x * y");
    
    std::cout << "=== Symbolic Engine Features ===" << std::endl;
    std::cout << "✓ Symbolic differentiation" << std::endl;
    std::cout << "✓ Expression simplification" << std::endl;
    std::cout << "✓ Numerical evaluation" << std::endl;
    std::cout << "✓ Support for:" << std::endl;
    std::cout << "  - Basic arithmetic (+, -, *, /, ^)" << std::endl;
    std::cout << "  - Trigonometric functions (sin, cos, tan)" << std::endl;
    std::cout << "  - Logarithmic functions (ln, log)" << std::endl;
    std::cout << "  - Other functions (sqrt, abs)" << std::endl;
    std::cout << "✓ Product rule, quotient rule, chain rule" << std::endl;
    std::cout << "✓ Constant folding and simplification" << std::endl;
    
    return 0;
}
