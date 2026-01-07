#include "SymbolicEngine.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

// ============================================================================
// Symbolic Expression Implementations
// ============================================================================

// SymbolicNumber implementation
std::string SymbolicNumber::toString() const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::unique_ptr<SymbolicExpression> SymbolicNumber::differentiate(const std::string& variable) const {
    (void)variable; // Suppress unused parameter warning
    return std::make_unique<SymbolicNumber>(0.0); // Derivative of constant is 0
}

std::unique_ptr<SymbolicExpression> SymbolicNumber::integrate(const std::string& variable) const {
    (void)variable; // Suppress unused parameter warning
    return std::make_unique<SymbolicBinaryOp>(SymbolicBinaryOp::OpType::MULTIPLY,
                                             std::make_unique<SymbolicNumber>(value),
                                             std::make_unique<SymbolicVariable>(variable));
}

std::unique_ptr<SymbolicExpression> SymbolicNumber::simplify() const {
    return std::make_unique<SymbolicNumber>(value);
}

std::unique_ptr<SymbolicExpression> SymbolicNumber::clone() const {
    return std::make_unique<SymbolicNumber>(value);
}

double SymbolicNumber::evaluate(const std::map<std::string, double>& variables) const {
    (void)variables; // Suppress unused parameter warning
    return value;
}

// SymbolicVariable implementation
std::string SymbolicVariable::toString() const {
    return name;
}

std::unique_ptr<SymbolicExpression> SymbolicVariable::differentiate(const std::string& variable) const {
    if (name == variable) {
        return std::make_unique<SymbolicNumber>(1.0); // d/dx(x) = 1
    } else {
        return std::make_unique<SymbolicNumber>(0.0); // d/dx(y) = 0 for y != x
    }
}

std::unique_ptr<SymbolicExpression> SymbolicVariable::integrate(const std::string& variable) const {
    if (name == variable) {
        // ∫x dx = x²/2
        return std::make_unique<SymbolicBinaryOp>(SymbolicBinaryOp::OpType::DIVIDE,
                                                 std::make_unique<SymbolicBinaryOp>(SymbolicBinaryOp::OpType::POWER,
                                                                                   std::make_unique<SymbolicVariable>(variable),
                                                                                   std::make_unique<SymbolicNumber>(2.0)),
                                                 std::make_unique<SymbolicNumber>(2.0));
    } else {
        // ∫y dx = y*x (treating y as constant)
        return std::make_unique<SymbolicBinaryOp>(SymbolicBinaryOp::OpType::MULTIPLY,
                                                 std::make_unique<SymbolicVariable>(name),
                                                 std::make_unique<SymbolicVariable>(variable));
    }
}

std::unique_ptr<SymbolicExpression> SymbolicVariable::simplify() const {
    return std::make_unique<SymbolicVariable>(name);
}

std::unique_ptr<SymbolicExpression> SymbolicVariable::clone() const {
    return std::make_unique<SymbolicVariable>(name);
}

double SymbolicVariable::evaluate(const std::map<std::string, double>& variables) const {
    auto it = variables.find(name);
    if (it == variables.end()) {
        throw std::runtime_error("Undefined variable: " + name);
    }
    return it->second;
}

// SymbolicBinaryOp implementation
std::string SymbolicBinaryOp::toString() const {
    // Pretty-printing with special handling for multiplication
    if (op == OpType::MULTIPLY) {
        // If both sides are constant numbers, multiply and return a single number
        if (left->isConstant() && right->isConstant()) {
            const SymbolicNumber* lnum = dynamic_cast<const SymbolicNumber*>(left.get());
            const SymbolicNumber* rnum = dynamic_cast<const SymbolicNumber*>(right.get());
            if (lnum && rnum) {
                double prod = lnum->value * rnum->value;
                std::ostringstream oss;
                if (std::floor(prod) == prod) oss << (long long)prod; else oss << prod;
                return oss.str();
            }
        }

        // If one side is constant, print coefficient first (e.g., 2*x -> 2x)
        if (left->isConstant() && !right->isConstant()) {
            const SymbolicNumber* lnum = dynamic_cast<const SymbolicNumber*>(left.get());
            if (lnum) {
                double coeff = lnum->value;
                std::string rhs = right->toString();
                if (coeff == 1.0) return rhs;
                if (coeff == -1.0) return std::string("-") + rhs;
                std::ostringstream oss;
                if (std::floor(coeff) == coeff) oss << (long long)coeff; else oss << coeff;
                // If rhs is a simple variable or function call, omit the *
                bool needParens = (dynamic_cast<const SymbolicVariable*>(right.get()) == nullptr &&
                                   dynamic_cast<const SymbolicFunction*>(right.get()) == nullptr &&
                                   dynamic_cast<const SymbolicUnaryOp*>(right.get()) == nullptr);
                return oss.str() + (needParens ? "(" + rhs + ")" : rhs);
            }
        }

        if (!left->isConstant() && right->isConstant()) {
            const SymbolicNumber* rnum = dynamic_cast<const SymbolicNumber*>(right.get());
            if (rnum) {
                double coeff = rnum->value;
                std::string lhs = left->toString();
                if (coeff == 1.0) return lhs;
                if (coeff == -1.0) return std::string("-") + lhs;
                std::ostringstream oss;
                if (std::floor(coeff) == coeff) oss << (long long)coeff; else oss << coeff;
                bool needParens = (dynamic_cast<const SymbolicVariable*>(left.get()) == nullptr &&
                                   dynamic_cast<const SymbolicFunction*>(left.get()) == nullptr &&
                                   dynamic_cast<const SymbolicUnaryOp*>(left.get()) == nullptr);
                return oss.str() + (needParens ? "(" + lhs + ")" : lhs);
            }
        }

        // Fallback: use explicit * with parentheses
        return "(" + left->toString() + " * " + right->toString() + ")";
    }

    std::string opStr;
    switch (op) {
        case OpType::ADD: opStr = "+"; break;
        case OpType::SUBTRACT: opStr = "-"; break;
        case OpType::DIVIDE: opStr = "/"; break;
        case OpType::POWER: opStr = "^"; break;
        default: opStr = "?"; break;
    }
    return "(" + left->toString() + " " + opStr + " " + right->toString() + ")";
}

std::unique_ptr<SymbolicExpression> SymbolicBinaryOp::differentiate(const std::string& variable) const {
    switch (op) {
        case OpType::ADD:
        case OpType::SUBTRACT: {
            auto leftDeriv = left->differentiate(variable);
            auto rightDeriv = right->differentiate(variable);
            if (op == OpType::ADD) {
                return makeSymbolicBinaryOp(OpType::ADD, std::move(leftDeriv), std::move(rightDeriv));
            } else {
                return makeSymbolicBinaryOp(OpType::SUBTRACT, std::move(leftDeriv), std::move(rightDeriv));
            }
        }
        case OpType::MULTIPLY: {
            // Product rule: d/dx(u*v) = u*dv/dx + v*du/dx
            auto leftDeriv = left->differentiate(variable);
            auto rightDeriv = right->differentiate(variable);
            
            auto term1 = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY, 
                                             left->clone(), std::move(rightDeriv));
            auto term2 = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY, 
                                             right->clone(), std::move(leftDeriv));
            
            return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::ADD, 
                                       std::move(term1), std::move(term2));
        }
        case OpType::DIVIDE: {
            // Quotient rule: d/dx(u/v) = (v*du/dx - u*dv/dx) / v^2
            auto leftDeriv = left->differentiate(variable);
            auto rightDeriv = right->differentiate(variable);
            
            auto numerator1 = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY, 
                                                  right->clone(), std::move(leftDeriv));
            auto numerator2 = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY, 
                                                  left->clone(), std::move(rightDeriv));
            auto numerator = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::SUBTRACT, 
                                                 std::move(numerator1), std::move(numerator2));
            
            auto denominator = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::POWER, 
                                                  right->clone(), 
                                                  std::make_unique<SymbolicNumber>(2.0));
            
            return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::DIVIDE, 
                                       std::move(numerator), std::move(denominator));
        }
        case OpType::POWER: {
            // Power rule for constant exponents: d/dx(x^n) = n*x^(n-1)
            if (right->isConstant()) {
                double expVal = right->evaluate();
                auto newExponent = std::make_unique<SymbolicNumber>(expVal - 1.0);
                auto powerTerm = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::POWER, 
                                                    left->clone(), std::move(newExponent));
                auto baseDeriv = left->differentiate(variable);
                auto constTerm = std::make_unique<SymbolicNumber>(expVal);
                
                return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY, 
                                          std::move(powerTerm), 
                                          makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                                              std::move(constTerm), std::move(baseDeriv)));
            }
            throw std::runtime_error("Differentiation of variable exponents not implemented");
        }
        default:
            throw std::runtime_error("Unknown binary operation in differentiation");
    }
}

std::unique_ptr<SymbolicExpression> SymbolicBinaryOp::simplify() const {
    auto simplifiedLeft = left->simplify();
    auto simplifiedRight = right->simplify();
    
    // Handle special cases
    if (op == OpType::ADD) {
        if (simplifiedLeft->isZero()) return simplifiedRight;
        if (simplifiedRight->isZero()) return simplifiedLeft;
        if (simplifiedLeft->isConstant() && simplifiedRight->isConstant()) {
            return std::make_unique<SymbolicNumber>(simplifiedLeft->evaluate() + simplifiedRight->evaluate());
        }
    } else if (op == OpType::SUBTRACT) {
        if (simplifiedRight->isZero()) return simplifiedLeft;
        if (simplifiedLeft->isZero()) {
            return makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::NEGATIVE, std::move(simplifiedRight));
        }
        if (simplifiedLeft->isConstant() && simplifiedRight->isConstant()) {
            return std::make_unique<SymbolicNumber>(simplifiedLeft->evaluate() - simplifiedRight->evaluate());
        }
    } else if (op == OpType::MULTIPLY) {
        if (simplifiedLeft->isZero() || simplifiedRight->isZero()) {
            return std::make_unique<SymbolicNumber>(0.0);
        }
        if (simplifiedLeft->isOne()) return simplifiedRight;
        if (simplifiedRight->isOne()) return simplifiedLeft;
        if (simplifiedLeft->isConstant() && simplifiedRight->isConstant()) {
            return std::make_unique<SymbolicNumber>(simplifiedLeft->evaluate() * simplifiedRight->evaluate());
        }
    } else if (op == OpType::DIVIDE) {
        if (simplifiedRight->isZero()) {
            throw std::runtime_error("Division by zero");
        }
        if (simplifiedLeft->isZero()) {
            return std::make_unique<SymbolicNumber>(0.0);
        }
        if (simplifiedRight->isOne()) return simplifiedLeft;
        if (simplifiedLeft->isConstant() && simplifiedRight->isConstant()) {
            return std::make_unique<SymbolicNumber>(simplifiedLeft->evaluate() / simplifiedRight->evaluate());
        }
    } else if (op == OpType::POWER) {
        if (simplifiedRight->isZero()) {
            return std::make_unique<SymbolicNumber>(1.0);
        }
        if (simplifiedRight->isOne()) return simplifiedLeft;
        if (simplifiedLeft->isZero()) {
            return std::make_unique<SymbolicNumber>(0.0);
        }
        if (simplifiedLeft->isOne()) {
            return std::make_unique<SymbolicNumber>(1.0);
        }
        if (simplifiedLeft->isConstant() && simplifiedRight->isConstant()) {
            return std::make_unique<SymbolicNumber>(std::pow(simplifiedLeft->evaluate(), simplifiedRight->evaluate()));
        }
    }
    
    return makeSymbolicBinaryOp(op, std::move(simplifiedLeft), std::move(simplifiedRight));
}

std::unique_ptr<SymbolicExpression> SymbolicBinaryOp::integrate(const std::string& variable) const {
    switch (op) {
        case OpType::ADD:
        case OpType::SUBTRACT: {
            auto leftIntegral = left->integrate(variable);
            auto rightIntegral = right->integrate(variable);
            if (op == OpType::ADD) {
                return makeSymbolicBinaryOp(OpType::ADD, std::move(leftIntegral), std::move(rightIntegral));
            } else {
                return makeSymbolicBinaryOp(OpType::SUBTRACT, std::move(leftIntegral), std::move(rightIntegral));
            }
        }
        case OpType::MULTIPLY: {
            // For simple cases like ∫x dx, ∫2x dx
            if (left->isConstant() && !right->isConstant()) {
                // ∫c*f(x) dx = c*∫f(x) dx
                auto rightIntegral = right->integrate(variable);
                return makeSymbolicBinaryOp(OpType::MULTIPLY, left->clone(), std::move(rightIntegral));
            } else if (!left->isConstant() && right->isConstant()) {
                // ∫f(x)*c dx = c*∫f(x) dx
                auto leftIntegral = left->integrate(variable);
                return makeSymbolicBinaryOp(OpType::MULTIPLY, std::move(leftIntegral), right->clone());
            }
            // For more complex cases, we'd need integration by parts
            throw std::runtime_error("Integration by parts not implemented for general multiplication");
        }
        case OpType::DIVIDE: {
            // For simple cases like ∫1/x dx
            if (left->isConstant() && right->toString() == variable) {
                // ∫c/x dx = c*ln(x)
                return makeSymbolicBinaryOp(OpType::MULTIPLY,
                                          left->clone(),
                                          makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::LN,
                                                             std::make_unique<SymbolicVariable>(variable)));
            }
            throw std::runtime_error("Complex division integration not implemented");
        }
        case OpType::POWER: {
            // For simple cases like ∫x^n dx where n is constant
            if (left->toString() == variable && right->isConstant()) {
                double expVal = right->evaluate();
                if (expVal == -1) {
                    // ∫1/x dx = ln(x)
                    return makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::LN,
                                             std::make_unique<SymbolicVariable>(variable));
                } else if (expVal != -1) {
                    // ∫x^n dx = x^(n+1)/(n+1) for n != -1
                    auto newExponent = std::make_unique<SymbolicNumber>(expVal + 1.0);
                    auto powerTerm = makeSymbolicBinaryOp(OpType::POWER,
                                                        std::make_unique<SymbolicVariable>(variable),
                                                        std::move(newExponent));
                    auto denominator = std::make_unique<SymbolicNumber>(expVal + 1.0);
                    return makeSymbolicBinaryOp(OpType::DIVIDE, std::move(powerTerm), std::move(denominator));
                }
            }
            throw std::runtime_error("Complex power integration not implemented");
        }
        default:
            throw std::runtime_error("Integration not implemented for this binary operation");
    }
}

std::unique_ptr<SymbolicExpression> SymbolicBinaryOp::clone() const {
    return makeSymbolicBinaryOp(op, left->clone(), right->clone());
}

double SymbolicBinaryOp::evaluate(const std::map<std::string, double>& variables) const {
    double leftVal = left->evaluate(variables);
    double rightVal = right->evaluate(variables);
    
    switch (op) {
        case OpType::ADD: return leftVal + rightVal;
        case OpType::SUBTRACT: return leftVal - rightVal;
        case OpType::MULTIPLY: return leftVal * rightVal;
        case OpType::DIVIDE: 
            if (rightVal == 0) throw std::runtime_error("Division by zero");
            return leftVal / rightVal;
        case OpType::POWER: return std::pow(leftVal, rightVal);
        default: throw std::runtime_error("Unknown binary operation");
    }
}

bool SymbolicBinaryOp::isConstant() const {
    return left->isConstant() && right->isConstant();
}

bool SymbolicBinaryOp::isZero() const {
    return false; // Would need more complex logic for this
}

bool SymbolicBinaryOp::isOne() const {
    return false; // Would need more complex logic for this
}

// SymbolicUnaryOp implementation
std::string SymbolicUnaryOp::toString() const {
    std::string opStr;
    switch (op) {
        case OpType::POSITIVE: return "+" + operand->toString();
        case OpType::NEGATIVE: return "-" + operand->toString();
        case OpType::SIN: return "sin(" + operand->toString() + ")";
        case OpType::COS: return "cos(" + operand->toString() + ")";
        case OpType::TAN: return "tan(" + operand->toString() + ")";
        case OpType::LOG: return "log(" + operand->toString() + ")";
        case OpType::LN: return "ln(" + operand->toString() + ")";
        case OpType::SQRT: return "sqrt(" + operand->toString() + ")";
        case OpType::ABS: return "abs(" + operand->toString() + ")";
        default: return "unknown(" + operand->toString() + ")";
    }
}

std::unique_ptr<SymbolicExpression> SymbolicUnaryOp::differentiate(const std::string& variable) const {
    auto operandDeriv = operand->differentiate(variable);
    
    switch (op) {
        case OpType::POSITIVE:
            return operandDeriv;
        case OpType::NEGATIVE:
            return makeSymbolicUnaryOp(OpType::NEGATIVE, std::move(operandDeriv));
        case OpType::SIN: {
            // d/dx(sin(u)) = cos(u) * du/dx
            auto cosExpr = makeSymbolicUnaryOp(OpType::COS, operand->clone());
            return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY, 
                                      std::move(cosExpr), std::move(operandDeriv));
        }
        case OpType::COS: {
            // d/dx(cos(u)) = -sin(u) * du/dx
            auto sinExpr = makeSymbolicUnaryOp(OpType::SIN, operand->clone());
            auto negSin = makeSymbolicUnaryOp(OpType::NEGATIVE, std::move(sinExpr));
            return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY, 
                                      std::move(negSin), std::move(operandDeriv));
        }
        case OpType::TAN: {
            // d/dx(tan(u)) = sec^2(u) * du/dx = (1/cos^2(u)) * du/dx
            auto cosExpr = makeSymbolicUnaryOp(OpType::COS, operand->clone());
            auto cosSquared = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::POWER, 
                                                  std::move(cosExpr), 
                                                  std::make_unique<SymbolicNumber>(2.0));
            auto secSquared = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::DIVIDE,
                                                  std::make_unique<SymbolicNumber>(1.0),
                                                  std::move(cosSquared));
            return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                      std::move(secSquared), std::move(operandDeriv));
        }
        case OpType::LN: {
            // d/dx(ln(u)) = (1/u) * du/dx
            auto oneOverU = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::DIVIDE,
                                                std::make_unique<SymbolicNumber>(1.0),
                                                operand->clone());
            return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                      std::move(oneOverU), std::move(operandDeriv));
        }
        case OpType::SQRT: {
            // d/dx(sqrt(u)) = (1/(2*sqrt(u))) * du/dx
            auto sqrtU = makeSymbolicUnaryOp(OpType::SQRT, operand->clone());
            auto twoSqrtU = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                                std::make_unique<SymbolicNumber>(2.0),
                                                std::move(sqrtU));
            auto oneOverTwoSqrtU = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::DIVIDE,
                                                       std::make_unique<SymbolicNumber>(1.0),
                                                       std::move(twoSqrtU));
            return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                      std::move(oneOverTwoSqrtU), std::move(operandDeriv));
        }
        default:
            throw std::runtime_error("Differentiation not implemented for this unary operation");
    }
}

std::unique_ptr<SymbolicExpression> SymbolicUnaryOp::simplify() const {
    auto simplifiedOperand = operand->simplify();
    
    // Handle special cases
    if (op == OpType::POSITIVE) {
        return simplifiedOperand;
    } else if (op == OpType::NEGATIVE) {
        if (simplifiedOperand->isZero()) {
            return std::make_unique<SymbolicNumber>(0.0);
        }
        // Double negative
        if (auto unaryOp = dynamic_cast<SymbolicUnaryOp*>(simplifiedOperand.get())) {
            if (unaryOp->op == OpType::NEGATIVE) {
                return unaryOp->operand->clone();
            }
        }
    }
    
    if (simplifiedOperand->isConstant()) {
        return std::make_unique<SymbolicNumber>(evaluate());
    }
    
    return makeSymbolicUnaryOp(op, std::move(simplifiedOperand));
}

std::unique_ptr<SymbolicExpression> SymbolicUnaryOp::integrate(const std::string& variable) const {
    switch (op) {
        case OpType::POSITIVE:
            return operand->integrate(variable);
        case OpType::NEGATIVE: {
            auto integral = operand->integrate(variable);
            return makeSymbolicUnaryOp(OpType::NEGATIVE, std::move(integral));
        }
        case OpType::SIN: {
            // ∫sin(x) dx = -cos(x)
            if (operand->toString() == variable) {
                return makeSymbolicUnaryOp(OpType::NEGATIVE,
                                         makeSymbolicUnaryOp(OpType::COS,
                                                            std::make_unique<SymbolicVariable>(variable)));
            }
            throw std::runtime_error("Complex sine integration not implemented");
        }
        case OpType::COS: {
            // ∫cos(x) dx = sin(x)
            if (operand->toString() == variable) {
                return makeSymbolicUnaryOp(OpType::SIN,
                                         std::make_unique<SymbolicVariable>(variable));
            }
            throw std::runtime_error("Complex cosine integration not implemented");
        }
        case OpType::LN: {
            // ∫ln(x) dx = x*ln(x) - x
            if (operand->toString() == variable) {
                auto x = std::make_unique<SymbolicVariable>(variable);
                auto xLnX = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                                x->clone(),
                                                makeSymbolicUnaryOp(OpType::LN, std::move(x)));
                return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::SUBTRACT,
                                          std::move(xLnX),
                                          std::make_unique<SymbolicVariable>(variable));
            }
            throw std::runtime_error("Complex logarithm integration not implemented");
        }
        default:
            throw std::runtime_error("Integration not implemented for this unary operation");
    }
}

std::unique_ptr<SymbolicExpression> SymbolicUnaryOp::clone() const {
    return makeSymbolicUnaryOp(op, operand->clone());
}

double SymbolicUnaryOp::evaluate(const std::map<std::string, double>& variables) const {
    double val = operand->evaluate(variables);
    
    switch (op) {
        case OpType::POSITIVE: return val;
        case OpType::NEGATIVE: return -val;
        case OpType::SIN: return std::sin(val);
        case OpType::COS: return std::cos(val);
        case OpType::TAN: return std::tan(val);
        case OpType::LOG: 
            if (val <= 0) throw std::runtime_error("Log of non-positive number");
            return std::log10(val);
        case OpType::LN: 
            if (val <= 0) throw std::runtime_error("Natural log of non-positive number");
            return std::log(val);
        case OpType::SQRT: 
            if (val < 0) throw std::runtime_error("Square root of negative number");
            return std::sqrt(val);
        case OpType::ABS: return std::abs(val);
        default: throw std::runtime_error("Unknown unary operation");
    }
}

bool SymbolicUnaryOp::isConstant() const {
    return operand->isConstant();
}

bool SymbolicUnaryOp::isZero() const {
    return operand->isZero();
}

bool SymbolicUnaryOp::isOne() const {
    return false; // Would need more complex logic for this
}

// SymbolicFunction implementation
std::string SymbolicFunction::toString() const {
    std::string result = functionName + "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) result += ", ";
        result += arguments[i]->toString();
    }
    result += ")";
    return result;
}

std::unique_ptr<SymbolicExpression> SymbolicFunction::differentiate(const std::string& variable) const {
    // For now, we'll handle basic functions
    if (arguments.size() != 1) {
        throw std::runtime_error("Differentiation not implemented for multi-argument functions");
    }
    
    auto argDeriv = arguments[0]->differentiate(variable);
    
    if (functionName == "sin") {
        auto cosExpr = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::COS, arguments[0]->clone());
        return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY, 
                                  std::move(cosExpr), std::move(argDeriv));
    } else if (functionName == "cos") {
        auto sinExpr = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::SIN, arguments[0]->clone());
        auto negSin = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::NEGATIVE, std::move(sinExpr));
        return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY, 
                                  std::move(negSin), std::move(argDeriv));
    } else if (functionName == "ln") {
        auto oneOverArg = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::DIVIDE,
                                              std::make_unique<SymbolicNumber>(1.0),
                                              arguments[0]->clone());
        return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                  std::move(oneOverArg), std::move(argDeriv));
    }
    
    throw std::runtime_error("Differentiation not implemented for function: " + functionName);
}

std::unique_ptr<SymbolicExpression> SymbolicFunction::simplify() const {
    std::vector<std::unique_ptr<SymbolicExpression>> simplifiedArgs;
    for (const auto& arg : arguments) {
        simplifiedArgs.push_back(arg->simplify());
    }
    
    // Check if all arguments are constant
    bool allConstant = true;
    for (const auto& arg : simplifiedArgs) {
        if (!arg->isConstant()) {
            allConstant = false;
            break;
        }
    }
    
    if (allConstant) {
        return std::make_unique<SymbolicNumber>(evaluate());
    }
    
    return makeSymbolicFunction(functionName, std::move(simplifiedArgs));
}

std::unique_ptr<SymbolicExpression> SymbolicFunction::integrate(const std::string& variable) const {
    // For now, we'll handle basic functions
    if (arguments.size() != 1) {
        throw std::runtime_error("Integration not implemented for multi-argument functions");
    }
    
    if (functionName == "sin" && arguments[0]->toString() == variable) {
        // ∫sin(x) dx = -cos(x)
        return makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::NEGATIVE,
                                 makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::COS,
                                                    std::make_unique<SymbolicVariable>(variable)));
    } else if (functionName == "cos" && arguments[0]->toString() == variable) {
        // ∫cos(x) dx = sin(x)
        return makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::SIN,
                                 std::make_unique<SymbolicVariable>(variable));
    } else if (functionName == "ln" && arguments[0]->toString() == variable) {
        // ∫ln(x) dx = x*ln(x) - x
        auto x = std::make_unique<SymbolicVariable>(variable);
        auto xLnX = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::MULTIPLY,
                                        x->clone(),
                                        makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::LN, std::move(x)));
        return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::SUBTRACT,
                                  std::move(xLnX),
                                  std::make_unique<SymbolicVariable>(variable));
    }
    
    throw std::runtime_error("Integration not implemented for function: " + functionName);
}

std::unique_ptr<SymbolicExpression> SymbolicFunction::clone() const {
    std::vector<std::unique_ptr<SymbolicExpression>> clonedArgs;
    for (const auto& arg : arguments) {
        clonedArgs.push_back(arg->clone());
    }
    return makeSymbolicFunction(functionName, std::move(clonedArgs));
}

double SymbolicFunction::evaluate(const std::map<std::string, double>& variables) const {
    if (arguments.size() != 1) {
        throw std::runtime_error("Function " + functionName + " expects 1 argument");
    }
    
    double arg = arguments[0]->evaluate(variables);
    
    if (functionName == "sin") return std::sin(arg);
    if (functionName == "cos") return std::cos(arg);
    if (functionName == "tan") return std::tan(arg);
    if (functionName == "log") {
        if (arg <= 0) throw std::runtime_error("Log of non-positive number");
        return std::log10(arg);
    }
    if (functionName == "ln") {
        if (arg <= 0) throw std::runtime_error("Natural log of non-positive number");
        return std::log(arg);
    }
    if (functionName == "sqrt") {
        if (arg < 0) throw std::runtime_error("Square root of negative number");
        return std::sqrt(arg);
    }
    if (functionName == "abs") return std::abs(arg);
    
    throw std::runtime_error("Unknown function: " + functionName);
}

bool SymbolicFunction::isConstant() const {
    for (const auto& arg : arguments) {
        if (!arg->isConstant()) return false;
    }
    return true;
}

bool SymbolicFunction::isZero() const {
    return false; // Would need more complex logic for this
}

bool SymbolicFunction::isOne() const {
    return false; // Would need more complex logic for this
}

// ============================================================================
// SymbolicEngine Implementation
// ============================================================================

SymbolicEngine::SymbolicEngine() {}

bool SymbolicEngine::parseFromAST(const ASTNode* ast) {
    if (!ast) return false;
    
    try {
        expression = convertASTToSymbolic(ast);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "AST conversion error: " << e.what() << std::endl;
        return false;
    }
}

std::unique_ptr<SymbolicExpression> SymbolicEngine::convertASTToSymbolic(const ASTNode* ast) {
    if (!ast) {
        throw std::runtime_error("Null AST node");
    }
    
    // Dynamic cast to determine the type of AST node
    if (auto numberNode = dynamic_cast<const NumberNode*>(ast)) {
        return std::make_unique<SymbolicNumber>(numberNode->value);
    }
    else if (auto variableNode = dynamic_cast<const VariableNode*>(ast)) {
        return std::make_unique<SymbolicVariable>(variableNode->name);
    }
    else if (auto binaryOpNode = dynamic_cast<const BinaryOpNode*>(ast)) {
        auto left = convertASTToSymbolic(binaryOpNode->left.get());
        auto right = convertASTToSymbolic(binaryOpNode->right.get());
        
        SymbolicBinaryOp::OpType opType;
        switch (binaryOpNode->op) {
            case BinaryOpNode::OpType::ADD: opType = SymbolicBinaryOp::OpType::ADD; break;
            case BinaryOpNode::OpType::SUBTRACT: opType = SymbolicBinaryOp::OpType::SUBTRACT; break;
            case BinaryOpNode::OpType::MULTIPLY: opType = SymbolicBinaryOp::OpType::MULTIPLY; break;
            case BinaryOpNode::OpType::DIVIDE: opType = SymbolicBinaryOp::OpType::DIVIDE; break;
            case BinaryOpNode::OpType::POWER: opType = SymbolicBinaryOp::OpType::POWER; break;
            default: throw std::runtime_error("Unknown binary operation");
        }
        
        return makeSymbolicBinaryOp(opType, std::move(left), std::move(right));
    }
    else if (auto unaryOpNode = dynamic_cast<const UnaryOpNode*>(ast)) {
        auto operand = convertASTToSymbolic(unaryOpNode->operand.get());
        
        SymbolicUnaryOp::OpType opType;
        switch (unaryOpNode->op) {
            case UnaryOpNode::OpType::POSITIVE: opType = SymbolicUnaryOp::OpType::POSITIVE; break;
            case UnaryOpNode::OpType::NEGATIVE: opType = SymbolicUnaryOp::OpType::NEGATIVE; break;
            case UnaryOpNode::OpType::SIN: opType = SymbolicUnaryOp::OpType::SIN; break;
            case UnaryOpNode::OpType::COS: opType = SymbolicUnaryOp::OpType::COS; break;
            case UnaryOpNode::OpType::TAN: opType = SymbolicUnaryOp::OpType::TAN; break;
            case UnaryOpNode::OpType::LOG: opType = SymbolicUnaryOp::OpType::LOG; break;
            case UnaryOpNode::OpType::LN: opType = SymbolicUnaryOp::OpType::LN; break;
            case UnaryOpNode::OpType::SQRT: opType = SymbolicUnaryOp::OpType::SQRT; break;
            case UnaryOpNode::OpType::ABS: opType = SymbolicUnaryOp::OpType::ABS; break;
            default: throw std::runtime_error("Unknown unary operation");
        }
        
        return makeSymbolicUnaryOp(opType, std::move(operand));
    }
    else if (auto functionNode = dynamic_cast<const FunctionNode*>(ast)) {
        std::vector<std::unique_ptr<SymbolicExpression>> args;
        for (const auto& arg : functionNode->arguments) {
            args.push_back(convertASTToSymbolic(arg.get()));
        }
        
        return makeSymbolicFunction(functionNode->functionName, std::move(args));
    }
    else {
        throw std::runtime_error("Unknown AST node type");
    }
}

bool SymbolicEngine::parseFromString(const std::string& expressionStr) {
    ExpressionParser parser;
    if (parser.parse(expressionStr)) {
        return parseFromAST(parser.getAST());
    }
    return false;
}

std::unique_ptr<SymbolicExpression> SymbolicEngine::differentiate(const std::string& variable) const {
    if (!expression) {
        throw std::runtime_error("No expression to differentiate");
    }
    return expression->differentiate(variable);
}

std::unique_ptr<SymbolicExpression> SymbolicEngine::simplify() const {
    if (!expression) {
        throw std::runtime_error("No expression to simplify");
    }
    return expression->simplify();
}

double SymbolicEngine::evaluate(const std::map<std::string, double>& variables) const {
    if (!expression) {
        throw std::runtime_error("No expression to evaluate");
    }
    return expression->evaluate(variables);
}

std::string SymbolicEngine::toString() const {
    if (!expression) {
        return "No expression";
    }
    return expression->toString();
}



// Integration implementation
std::unique_ptr<SymbolicExpression> SymbolicEngine::integrate(const std::string& variable) const {
    if (!expression) {
        throw std::runtime_error("No expression to integrate");
    }
    return expression->integrate(variable);
}

std::unique_ptr<SymbolicExpression> SymbolicEngine::solve(const std::string& variable) const {
    if (!expression) {
        throw std::runtime_error("No expression to solve");
    }
    
    // For now, we'll implement simple linear equation solving
    // In a full CAS, you'd want more sophisticated equation solving
    
    // Try to solve linear equations of the form ax + b = 0
    try {
        auto simplified = expression->simplify();
        
        // Check if it's a linear equation in the variable
        if (auto binaryOp = dynamic_cast<const SymbolicBinaryOp*>(simplified.get())) {
            if (binaryOp->op == SymbolicBinaryOp::OpType::ADD || 
                binaryOp->op == SymbolicBinaryOp::OpType::SUBTRACT) {
                
                // Try to isolate the variable
                auto left = binaryOp->left->clone();
                auto right = binaryOp->right->clone();
                
                // Move constant terms to the right side
                if (left->isConstant()) {
                    // ax + b = 0 -> x = -b/a
                    if (binaryOp->op == SymbolicBinaryOp::OpType::ADD) {
                        auto negB = makeSymbolicUnaryOp(SymbolicUnaryOp::OpType::NEGATIVE, std::move(left));
                        return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::DIVIDE, std::move(negB), std::move(right));
                    } else {
                        return makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::DIVIDE, std::move(left), std::move(right));
                    }
                }
            }
        }
        
        throw std::runtime_error("Complex equation solving not implemented");
    } catch (const std::exception& e) {
        throw std::runtime_error("Equation solving failed: " + std::string(e.what()));
    }
}

std::vector<std::unique_ptr<SymbolicExpression>> SymbolicEngine::factor() const {
    if (!expression) {
        throw std::runtime_error("No expression to factor");
    }
    
    std::vector<std::unique_ptr<SymbolicExpression>> factors;
    
    try {
        auto simplified = expression->simplify();
        
        // For now, implement basic factoring
        if (auto binaryOp = dynamic_cast<const SymbolicBinaryOp*>(simplified.get())) {
            if (binaryOp->op == SymbolicBinaryOp::OpType::MULTIPLY) {
                // Already factored
                factors.push_back(binaryOp->left->clone());
                factors.push_back(binaryOp->right->clone());
                return factors;
            } else if (binaryOp->op == SymbolicBinaryOp::OpType::ADD) {
                // Try to factor out common terms
                // For example: x^2 + x = x(x + 1)
                if (auto leftPower = dynamic_cast<const SymbolicBinaryOp*>(binaryOp->left.get())) {
                    if (leftPower->op == SymbolicBinaryOp::OpType::POWER && 
                        leftPower->left->toString() == "x" && leftPower->right->toString() == "2") {
                        
                        if (binaryOp->right->toString() == "x") {
                            // x^2 + x = x(x + 1)
                            auto x = std::make_unique<SymbolicVariable>("x");
                            auto xPlusOne = makeSymbolicBinaryOp(SymbolicBinaryOp::OpType::ADD,
                                                                std::make_unique<SymbolicVariable>("x"),
                                                                std::make_unique<SymbolicNumber>(1.0));
                            factors.push_back(std::move(x));
                            factors.push_back(std::move(xPlusOne));
                            return factors;
                        }
                    }
                }
            }
        }
        
        // If no factoring is possible, return the original expression
        factors.push_back(simplified->clone());
        return factors;
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Factoring failed: " + std::string(e.what()));
    }
}

// ============================================================================
// Utility Functions
// ============================================================================

std::unique_ptr<SymbolicExpression> makeSymbolicNumber(double value) {
    return std::make_unique<SymbolicNumber>(value);
}

std::unique_ptr<SymbolicExpression> makeSymbolicVariable(const std::string& name) {
    return std::make_unique<SymbolicVariable>(name);
}

std::unique_ptr<SymbolicExpression> makeSymbolicBinaryOp(SymbolicBinaryOp::OpType op,
                                                        std::unique_ptr<SymbolicExpression> left,
                                                        std::unique_ptr<SymbolicExpression> right) {
    return std::make_unique<SymbolicBinaryOp>(op, std::move(left), std::move(right));
}

std::unique_ptr<SymbolicExpression> makeSymbolicUnaryOp(SymbolicUnaryOp::OpType op,
                                                       std::unique_ptr<SymbolicExpression> operand) {
    return std::make_unique<SymbolicUnaryOp>(op, std::move(operand));
}

std::unique_ptr<SymbolicExpression> makeSymbolicFunction(const std::string& funcName,
                                                        std::vector<std::unique_ptr<SymbolicExpression>> args) {
    return std::make_unique<SymbolicFunction>(funcName, std::move(args));
}
