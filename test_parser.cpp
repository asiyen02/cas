#include "parser/ExpressionParser.h"
#include <iostream>
#include <map>

void testExpression(const std::string& expr, const std::map<std::string, double>& vars = {}) {
    std::cout << "Testing: " << expr << std::endl;
    
    ExpressionParser parser;
    if (parser.parse(expr)) {
        std::cout << "  AST: " << parser.toString() << std::endl;
        
        try {
            double result = parser.evaluate(vars);
            std::cout << "  Result: " << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  Evaluation error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "  Parse error: " << parser.getError() << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Expression Parser Test ===\n\n";
    
    // Test basic arithmetic
    testExpression("2 + 3");
    testExpression("10 - 4");
    testExpression("6 * 7");
    testExpression("15 / 3");
    testExpression("2 ^ 3");
    
    // Test complex expressions
    testExpression("2 + 3 * 4");
    testExpression("(2 + 3) * 4");
    testExpression("2 ^ 3 ^ 2");
    testExpression("10 - 3 * 2 + 1");
    
    // Test variables
    std::map<std::string, double> vars = {{"x", 5.0}, {"y", 3.0}};
    testExpression("x + y", vars);
    testExpression("x * y - 2", vars);
    testExpression("x ^ 2 + y ^ 2", vars);
    
    // Test functions
    testExpression("sin(3.14159)");
    testExpression("cos(0)");
    testExpression("sqrt(16)");
    testExpression("log(100)");
    testExpression("ln(2.718)");
    testExpression("abs(-5)");
    
    // Test function with variables
    testExpression("sin(x)", vars);
    testExpression("sqrt(x * x + y * y)", vars);
    
    // Test complex expressions with functions
    testExpression("sin(x) + cos(y)", vars);
    testExpression("sqrt(x^2 + y^2)", vars);
    testExpression("log(x * y) + ln(x / y)", vars);
    
    // Test error cases
    testExpression("2 + ");  // Incomplete expression
    testExpression("2 / 0"); // Division by zero
    testExpression("sqrt(-1)"); // Invalid operation
    testExpression("unknown(5)"); // Unknown function
    
    return 0;
}
