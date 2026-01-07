#include "cas/SymbolicEngine.h"
#include <iostream>
#include <map>

void testSymbolicExpression(const std::string& name, std::unique_ptr<SymbolicExpression> expr) {
    std::cout << "=== " << name << " ===" << std::endl;
    std::cout << "Expression: " << expr->toString() << std::endl;
    
    // Test simplification
    auto simplified = expr->simplify();
    std::cout << "Simplified: " << simplified->toString() << std::endl;
    
    // Test differentiation with respect to x
    try {
        auto derivative = expr->differentiate("x");
        std::cout << "d/dx: " << derivative->toString() << std::endl;
        
        auto simplifiedDeriv = derivative->simplify();
        std::cout << "Simplified d/dx: " << simplifiedDeriv->toString() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Differentiation error: " << e.what() << std::endl;
    }
    
    // Test evaluation
    std::map<std::string, double> vars = {{"x", 2.0}, {"y", 3.0}};
    try {
        double result = expr->evaluate(vars);
        std::cout << "f(" << vars["x"] << ", " << vars["y"] << ") = " << result << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Evaluation error: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Symbolic Engine Test ===\n\n";
    
    // Test 1: Simple constant
    testSymbolicExpression("Constant", makeSymbolicNumber(5.0));
    
    // Test 2: Variable
    testSymbolicExpression("Variable x", makeSymbolicVariable("x"));
    
    // Test 3: Simple addition
    auto expr1 = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::ADD,
                                     makeSymbolicVariable("x"),
                                     makeSymbolicNumber(3.0));
    testSymbolicExpression("x + 3", std::move(expr1));
    
    // Test 4: Multiplication
    auto expr2 = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                     makeSymbolicVariable("x"),
                                     makeSymbolicVariable("y"));
    testSymbolicExpression("x * y", std::move(expr2));
    
    // Test 5: Power
    auto expr3 = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::POWER,
                                     makeSymbolicVariable("x"),
                                     makeSymbolicNumber(2.0));
    testSymbolicExpression("x^2", std::move(expr3));
    
    // Test 6: Complex expression: x^2 + 2*x + 1
    auto xSquared = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::POWER,
                                        makeSymbolicVariable("x"),
                                        makeSymbolicNumber(2.0));
    auto twoX = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                    makeSymbolicNumber(2.0),
                                    makeSymbolicVariable("x"));
    auto firstTerm = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::ADD,
                                        std::move(xSquared),
                                        std::move(twoX));
    auto complexExpr = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::ADD,
                                           std::move(firstTerm),
                                           makeSymbolicNumber(1.0));
    testSymbolicExpression("x^2 + 2*x + 1", std::move(complexExpr));
    
    // Test 7: Trigonometric function
    auto sinExpr = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::SIN,
                                      makeSymbolicVariable("x"));
    testSymbolicExpression("sin(x)", std::move(sinExpr));
    
    // Test 8: Natural logarithm
    auto lnExpr = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::LN,
                                     makeSymbolicVariable("x"));
    testSymbolicExpression("ln(x)", std::move(lnExpr));
    
    // Test 9: Product rule: x * sin(x)
    auto x = makeSymbolicVariable("x");
    auto sinX = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::SIN,
                                   makeSymbolicVariable("x"));
    auto productExpr = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                           std::move(x),
                                           std::move(sinX));
    testSymbolicExpression("x * sin(x)", std::move(productExpr));
    
    // Test 10: Quotient rule: x / (x + 1)
    auto numerator = makeSymbolicVariable("x");
    auto denominator = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::ADD,
                                          makeSymbolicVariable("x"),
                                          makeSymbolicNumber(1.0));
    auto quotientExpr = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::DIVIDE,
                                            std::move(numerator),
                                            std::move(denominator));
    testSymbolicExpression("x / (x + 1)", std::move(quotientExpr));
    
    // Test 11: Chain rule: sin(x^2)
    auto xSquared2 = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::POWER,
                                         makeSymbolicVariable("x"),
                                         makeSymbolicNumber(2.0));
    auto sinXSquared = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::SIN,
                                          std::move(xSquared2));
    testSymbolicExpression("sin(x^2)", std::move(sinXSquared));
    
    // Test 12: Square root
    auto sqrtExpr = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::SQRT,
                                       makeSymbolicVariable("x"));
    testSymbolicExpression("sqrt(x)", std::move(sqrtExpr));
    
    // Test 13: Negative expression
    auto negExpr = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::NEGATIVE,
                                      makeSymbolicVariable("x"));
    testSymbolicExpression("-x", std::move(negExpr));
    
    // Test 14: Double negative
    auto doubleNegExpr = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::NEGATIVE,
                                            makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::NEGATIVE,
                                                               makeSymbolicVariable("x")));
    testSymbolicExpression("-(-x)", std::move(doubleNegExpr));
    
    return 0;
}
