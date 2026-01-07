#include "ExpressionParser.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <algorithm>

// ============================================================================
// AST Node Implementations
// ============================================================================

// BinaryOpNode implementation
std::string BinaryOpNode::toString() const {
    std::string opStr;
    switch (op) {
        case OpType::ADD: opStr = "+"; break;
        case OpType::SUBTRACT: opStr = "-"; break;
        case OpType::MULTIPLY: opStr = "*"; break;
        case OpType::DIVIDE: opStr = "/"; break;
        case OpType::POWER: opStr = "^"; break;
    }
    return "(" + left->toString() + " " + opStr + " " + right->toString() + ")";
}

double BinaryOpNode::evaluate(const std::map<std::string, double>& variables) const {
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

std::unique_ptr<ASTNode> BinaryOpNode::clone() const {
    return std::make_unique<BinaryOpNode>(op, left->clone(), right->clone());
}

// UnaryOpNode implementation
std::string UnaryOpNode::toString() const {
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

double UnaryOpNode::evaluate(const std::map<std::string, double>& variables) const {
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

std::unique_ptr<ASTNode> UnaryOpNode::clone() const {
    return std::make_unique<UnaryOpNode>(op, operand->clone());
}

// NumberNode implementation
std::string NumberNode::toString() const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

double NumberNode::evaluate(const std::map<std::string, double>& variables) const {
    (void)variables; // Suppress unused parameter warning
    return value;
}

std::unique_ptr<ASTNode> NumberNode::clone() const {
    return std::make_unique<NumberNode>(value);
}

// VariableNode implementation
std::string VariableNode::toString() const {
    return name;
}

double VariableNode::evaluate(const std::map<std::string, double>& variables) const {
    auto it = variables.find(name);
    if (it == variables.end()) {
        throw std::runtime_error("Undefined variable: " + name);
    }
    return it->second;
}

std::unique_ptr<ASTNode> VariableNode::clone() const {
    return std::make_unique<VariableNode>(name);
}

// FunctionNode implementation
std::string FunctionNode::toString() const {
    std::string result = functionName + "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) result += ", ";
        result += arguments[i]->toString();
    }
    result += ")";
    return result;
}

double FunctionNode::evaluate(const std::map<std::string, double>& variables) const {
    // For now, we'll handle basic functions here
    // In a more sophisticated CAS, this would be handled by the symbolic engine
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

std::unique_ptr<ASTNode> FunctionNode::clone() const {
    std::vector<std::unique_ptr<ASTNode>> clonedArgs;
    for (const auto& arg : arguments) {
        clonedArgs.push_back(arg->clone());
    }
    return std::make_unique<FunctionNode>(functionName, std::move(clonedArgs));
}

// ============================================================================
// Lexer Implementation
// ============================================================================

Lexer::Lexer(const std::string& expression) 
    : input(expression), position(0), length(expression.length()) {}

void Lexer::skipWhitespace() {
    while (position < length && std::isspace(input[position])) {
        position++;
    }
}

Token Lexer::readNumber() {
    size_t start = position;
    bool hasDecimal = false;
    
    while (position < length) {
        char c = input[position];
        if (std::isdigit(c)) {
            position++;
        } else if (c == '.' && !hasDecimal) {
            hasDecimal = true;
            position++;
        } else if (c == 'e' || c == 'E') {
            // Handle scientific notation
            position++;
            if (position < length && (input[position] == '+' || input[position] == '-')) {
                position++;
            }
            while (position < length && std::isdigit(input[position])) {
                position++;
            }
            break;
        } else {
            break;
        }
    }
    
    std::string numberStr = input.substr(start, position - start);
    return Token(TokenType::NUMBER, numberStr, start);
}

Token Lexer::readIdentifier() {
    size_t start = position;
    while (position < length && (std::isalnum(input[position]) || input[position] == '_')) {
        position++;
    }
    
    std::string identifier = input.substr(start, position - start);
    TokenType type = isFunction(identifier) ? TokenType::FUNCTION : TokenType::VARIABLE;
    return Token(type, identifier, start);
}

bool Lexer::isFunction(const std::string& identifier) {
    static const std::vector<std::string> functions = {
        "sin", "cos", "tan", "log", "ln", "sqrt", "abs"
    };
    return std::find(functions.begin(), functions.end(), identifier) != functions.end();
}

Token Lexer::getNextToken() {
    skipWhitespace();
    
    if (position >= length) {
        return Token(TokenType::END_OF_FILE, "", position);
    }
    
    char current = input[position];
    size_t currentPos = position;
    
    // Numbers
    if (std::isdigit(current) || current == '.') {
        return readNumber();
    }
    
    // Identifiers and functions
    if (std::isalpha(current) || current == '_') {
        return readIdentifier();
    }
    
    // Single character tokens
    switch (current) {
        case '+': position++; return Token(TokenType::PLUS, "+", currentPos);
        case '-': position++; return Token(TokenType::MINUS, "-", currentPos);
        case '*': position++; return Token(TokenType::MULTIPLY, "*", currentPos);
        case '/': position++; return Token(TokenType::DIVIDE, "/", currentPos);
        case '^': position++; return Token(TokenType::POWER, "^", currentPos);
        case '(': position++; return Token(TokenType::LEFT_PAREN, "(", currentPos);
        case ')': position++; return Token(TokenType::RIGHT_PAREN, ")", currentPos);
        case ',': position++; return Token(TokenType::COMMA, ",", currentPos);
        default:
            position++;
            return Token(TokenType::INVALID, std::string(1, current), currentPos);
    }
}

void Lexer::reset() {
    position = 0;
}

// ============================================================================
// Parser Implementation
// ============================================================================

Parser::Parser(const std::string& expression) : lexer(expression), currentToken(TokenType::INVALID, "", 0) {
    advance();
}

void Parser::advance() {
    currentToken = lexer.getNextToken();
}

void Parser::expect(TokenType type, const std::string& message) {
    if (currentToken.type != type) {
        std::string expected = message.empty() ? "Unexpected token" : message;
        throwError(expected + " at position " + std::to_string(currentToken.position));
    }
}

int Parser::getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 1;
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
            return 2;
        case TokenType::POWER:
            return 3;
        default:
            return 0;
    }
}

bool Parser::isRightAssociative(TokenType type) {
    return type == TokenType::POWER;
}

std::unique_ptr<ASTNode> Parser::parse() {
    try {
        auto result = parseExpression();
        expect(TokenType::END_OF_FILE, "Expected end of expression");
        return result;
    } catch (const std::exception& e) {
        throwError(e.what());
        return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseTerm();
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();
    
    while (currentToken.type == TokenType::PLUS || currentToken.type == TokenType::MINUS) {
        BinaryOpNode::OpType op = (currentToken.type == TokenType::PLUS) ? 
            BinaryOpNode::OpType::ADD : BinaryOpNode::OpType::SUBTRACT;
        
        advance();
        auto right = parseFactor();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    auto left = parsePower();
    
    while (currentToken.type == TokenType::MULTIPLY || currentToken.type == TokenType::DIVIDE ||
           currentToken.type == TokenType::NUMBER || currentToken.type == TokenType::VARIABLE || 
           currentToken.type == TokenType::FUNCTION || currentToken.type == TokenType::LEFT_PAREN) {
        
        BinaryOpNode::OpType op;
        
        if (currentToken.type == TokenType::MULTIPLY || currentToken.type == TokenType::DIVIDE) {
            op = (currentToken.type == TokenType::MULTIPLY) ? 
                BinaryOpNode::OpType::MULTIPLY : BinaryOpNode::OpType::DIVIDE;
            advance();
        } else {
            // Implicit multiplication detected
            op = BinaryOpNode::OpType::MULTIPLY;
        }
        
        auto right = parsePower();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePower() {
    auto left = parsePrimary();
    
    if (currentToken.type == TokenType::POWER) {
        advance();
        auto right = parsePower(); // Right associative
        left = std::make_unique<BinaryOpNode>(BinaryOpNode::OpType::POWER, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    switch (currentToken.type) {
        case TokenType::NUMBER: {
            double value = std::stod(currentToken.value);
            advance();
            return std::make_unique<NumberNode>(value);
        }
        
        case TokenType::VARIABLE: {
            std::string varName = currentToken.value;
            advance();
            return std::make_unique<VariableNode>(varName);
        }
        
        case TokenType::FUNCTION: {
            return parseFunction();
        }
        
        case TokenType::LEFT_PAREN: {
            advance();
            auto expr = parseExpression();
            expect(TokenType::RIGHT_PAREN, "Expected closing parenthesis");
            advance();
            return expr;
        }
        
        case TokenType::PLUS:
        case TokenType::MINUS: {
            UnaryOpNode::OpType op = (currentToken.type == TokenType::PLUS) ? 
                UnaryOpNode::OpType::POSITIVE : UnaryOpNode::OpType::NEGATIVE;
            advance();
            auto operand = parsePrimary();
            return std::make_unique<UnaryOpNode>(op, std::move(operand));
        }
        
        default:
            throwError("Unexpected token: " + currentToken.value);
            return nullptr;
    }
}

std::unique_ptr<ASTNode> Parser::parseFunction() {
    std::string funcName = currentToken.value;
    advance();
    
    expect(TokenType::LEFT_PAREN, "Expected opening parenthesis after function name");
    advance();
    
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    if (currentToken.type != TokenType::RIGHT_PAREN) {
        arguments.push_back(parseExpression());
        
        while (currentToken.type == TokenType::COMMA) {
            advance();
            arguments.push_back(parseExpression());
        }
    }
    
    expect(TokenType::RIGHT_PAREN, "Expected closing parenthesis");
    advance();
    
    return std::make_unique<FunctionNode>(funcName, std::move(arguments));
}

void Parser::throwError(const std::string& message) {
    throw std::runtime_error(message);
}

bool Parser::hasError() const {
    return false; // Errors are thrown as exceptions
}

std::string Parser::getErrorMessage() const {
    return ""; // Errors are thrown as exceptions
}

// ============================================================================
// ExpressionParser Implementation
// ============================================================================

ExpressionParser::ExpressionParser() : hasParsingError(false) {}

bool ExpressionParser::parse(const std::string& expression) {
    try {
        Parser parser(expression);
        ast = parser.parse();
        hasParsingError = false;
        lastError.clear();
        return true;
    } catch (const std::exception& e) {
        hasParsingError = true;
        lastError = e.what();
        ast.reset();
        return false;
    }
}

double ExpressionParser::evaluate(const std::map<std::string, double>& variables) const {
    if (!ast) {
        throw std::runtime_error("No expression parsed");
    }
    return ast->evaluate(variables);
}

std::string ExpressionParser::toString() const {
    if (!ast) {
        return "No expression parsed";
    }
    return ast->toString();
}

const ASTNode* ExpressionParser::getAST() const {
    return ast.get();
}

std::unique_ptr<ASTNode> ExpressionParser::cloneAST() const {
    if (!ast) {
        return nullptr;
    }
    return ast->clone();
}
