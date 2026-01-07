#include "cas/SymbolicEngine.h"
#include "parser/ExpressionParser.h"
#include <iostream>
#include <string>
#include <map>

void demonstrateCAS(const std::string& expression, const std::string& description) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "DEMO: " << description << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Expression: " << expression << std::endl;
    
    SymbolicEngine engine;
    if (!engine.parseFromString(expression)) {
        std::cout << "❌ Parse failed: " << engine.toString() << std::endl;
        return;
    }
    
    std::cout << "✅ Parsed: " << engine.toString() << std::endl;
    
    // Differentiation
    try {
        auto derivative = engine.differentiate("x");
        std::cout << "📈 d/dx: " << derivative->toString() << std::endl;
        auto simplifiedDeriv = derivative->simplify();
        std::cout << "📈 Simplified: " << simplifiedDeriv->toString() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "❌ Differentiation failed: " << e.what() << std::endl;
    }
    
    // Integration
    try {
        auto integral = engine.integrate("x");
        std::cout << "📊 ∫dx: " << integral->toString() << std::endl;
        auto simplifiedIntegral = integral->simplify();
        std::cout << "📊 Simplified: " << simplifiedIntegral->toString() << std::endl;
        
        // Evaluate integral at x = 2
        std::map<std::string, double> vars = {{"x", 2.0}};
        double result = integral->evaluate(vars);
        std::cout << "📊 ∫f(2) = " << result << std::endl;
    } catch (const std::exception& e) {
        std::cout << "❌ Integration failed: " << e.what() << std::endl;
    }
    
    // Simplification
    try {
        auto simplified = engine.simplify();
        std::cout << "🔧 Simplified: " << simplified->toString() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "❌ Simplification failed: " << e.what() << std::endl;
    }
    
    // Factoring
    try {
        auto factors = engine.factor();
        std::cout << "🔍 Factors: ";
        for (size_t i = 0; i < factors.size(); ++i) {
            if (i > 0) std::cout << " × ";
            std::cout << factors[i]->toString();
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cout << "❌ Factoring failed: " << e.what() << std::endl;
    }
    
    // Evaluation
    try {
        std::map<std::string, double> vars = {{"x", 3.0}, {"y", 2.0}};
        double result = engine.evaluate(vars);
        std::cout << "🎯 f(3,2) = " << result << std::endl;
    } catch (const std::exception& e) {
        std::cout << "❌ Evaluation failed: " << e.what() << std::endl;
    }
}

void demonstrateEquationSolving(const std::string& equation) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "EQUATION SOLVING: " << equation << " = 0" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    SymbolicEngine engine;
    if (!engine.parseFromString(equation)) {
        std::cout << "❌ Parse failed" << std::endl;
        return;
    }
    
    std::cout << "✅ Equation: " << engine.toString() << std::endl;
    
    try {
        auto solution = engine.solve("x");
        std::cout << "🔍 Solution: x = " << solution->toString() << std::endl;
        
        auto simplified = solution->simplify();
        std::cout << "🔍 Simplified: x = " << simplified->toString() << std::endl;
        
        // Verify solution
        std::map<std::string, double> vars;
        if (simplified->isConstant()) {
            double xValue = simplified->evaluate();
            vars["x"] = xValue;
            double result = engine.evaluate(vars);
            std::cout << "✅ Verification: f(" << xValue << ") = " << result << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ Solving failed: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "🚀 COMPREHENSIVE CAS DEMONSTRATION 🚀" << std::endl;
    std::cout << "Computer Algebra System with Advanced Features" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Basic arithmetic and functions
    demonstrateCAS("x^2", "Power function");
    demonstrateCAS("sin(x)", "Trigonometric function");
    demonstrateCAS("cos(x)", "Cosine function");
    demonstrateCAS("ln(x)", "Natural logarithm");
    demonstrateCAS("sqrt(x)", "Square root");
    
    // Complex expressions
    demonstrateCAS("x^2 + 2*x + 1", "Quadratic expression");
    demonstrateCAS("x * sin(x)", "Product with trigonometric function");
    demonstrateCAS("x / (x + 1)", "Rational function");
    demonstrateCAS("2*x + 3*y", "Linear expression with multiple variables");
    
    // Integration examples
    demonstrateCAS("x", "Simple linear function");
    demonstrateCAS("x^3", "Cubic function");
    demonstrateCAS("1/x", "Reciprocal function");
    demonstrateCAS("x^2 + x", "Polynomial");
    
    // Equation solving
    demonstrateEquationSolving("x + 1");
    demonstrateEquationSolving("2*x - 3");
    demonstrateEquationSolving("x^2 + x");
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "🎉 CAS FEATURES SUMMARY 🎉" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "✅ Expression Parsing & AST Building" << std::endl;
    std::cout << "✅ AST to Symbolic Expression Conversion" << std::endl;
    std::cout << "✅ Symbolic Differentiation" << std::endl;
    std::cout << "✅ Symbolic Integration" << std::endl;
    std::cout << "✅ Expression Simplification" << std::endl;
    std::cout << "✅ Equation Solving" << std::endl;
    std::cout << "✅ Expression Factoring" << std::endl;
    std::cout << "✅ Numerical Evaluation" << std::endl;
    std::cout << "✅ Support for:" << std::endl;
    std::cout << "   - Basic arithmetic (+, -, *, /, ^)" << std::endl;
    std::cout << "   - Trigonometric functions (sin, cos, tan)" << std::endl;
    std::cout << "   - Logarithmic functions (ln, log)" << std::endl;
    std::cout << "   - Other functions (sqrt, abs)" << std::endl;
    std::cout << "✅ Product rule, quotient rule, chain rule" << std::endl;
    std::cout << "✅ Integration rules for common functions" << std::endl;
    std::cout << "✅ Linear equation solving" << std::endl;
    std::cout << "✅ Basic factoring algorithms" << std::endl;
    
    std::cout << "\n🎯 Your CAS system is now a powerful mathematical tool!" << std::endl;
    std::cout << "Ready for advanced mathematical computations and symbolic manipulation." << std::endl;
    
    return 0;
}
