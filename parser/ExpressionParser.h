#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

// Forward declarations
class ASTNode;
class BinaryOpNode;
class UnaryOpNode;
class NumberNode;
class VariableNode;
class FunctionNode;

// Token types
enum class TokenType {
    NUMBER,
    VARIABLE,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    POWER,
    LEFT_PAREN,
    RIGHT_PAREN,
    FUNCTION,
    COMMA,
    END_OF_FILE,
    INVALID
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    size_t position;
    
    Token(TokenType t, const std::string& v = "", size_t pos = 0) 
        : type(t), value(v), position(pos) {}
};

// Abstract Syntax Tree Node base class
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
    virtual double evaluate(const std::map<std::string, double>& variables = {}) const = 0;
    virtual std::unique_ptr<ASTNode> clone() const = 0;
};

// Binary operation node (+, -, *, /, ^)
class BinaryOpNode : public ASTNode {
public:
    enum class OpType {
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        POWER
    };
    
    OpType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    BinaryOpNode(OpType operation, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(operation), left(std::move(l)), right(std::move(r)) {}
    
    std::string toString() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    std::unique_ptr<ASTNode> clone() const override;
};

// Unary operation node (+, -, functions)
class UnaryOpNode : public ASTNode {
public:
    enum class OpType {
        POSITIVE,
        NEGATIVE,
        SIN,
        COS,
        TAN,
        LOG,
        LN,
        SQRT,
        ABS
    };
    
    OpType op;
    std::unique_ptr<ASTNode> operand;
    
    UnaryOpNode(OpType operation, std::unique_ptr<ASTNode> op)
        : op(operation), operand(std::move(op)) {}
    
    std::string toString() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    std::unique_ptr<ASTNode> clone() const override;
};

// Number literal node
class NumberNode : public ASTNode {
public:
    double value;
    
    NumberNode(double val) : value(val) {}
    
    std::string toString() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    std::unique_ptr<ASTNode> clone() const override;
};

// Variable node
class VariableNode : public ASTNode {
public:
    std::string name;
    
    VariableNode(const std::string& varName) : name(varName) {}
    
    std::string toString() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    std::unique_ptr<ASTNode> clone() const override;
};

// Function call node
class FunctionNode : public ASTNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    FunctionNode(const std::string& funcName, std::vector<std::unique_ptr<ASTNode>> args)
        : functionName(funcName), arguments(std::move(args)) {}
    
    std::string toString() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    std::unique_ptr<ASTNode> clone() const override;
};

// Lexer class for tokenization
class Lexer {
private:
    std::string input;
    size_t position;
    size_t length;
    
    void skipWhitespace();
    Token readNumber();
    Token readIdentifier();
    bool isFunction(const std::string& identifier);
    
public:
    Lexer(const std::string& expression);
    Token getNextToken();
    void reset();
    size_t getPosition() const { return position; }
};

// Parser class for building AST
class Parser {
private:
    Lexer lexer;
    Token currentToken;
    
    void advance();
    void expect(TokenType type, const std::string& message = "");
    
    // Recursive descent parsing methods
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parsePower();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseFunction();
    
    // Operator precedence helpers
    int getPrecedence(TokenType type);
    bool isRightAssociative(TokenType type);
    
public:
    Parser(const std::string& expression);
    
    // Main parsing method
    std::unique_ptr<ASTNode> parse();
    
    // Error handling
    void throwError(const std::string& message);
    
    // Utility methods
    bool hasError() const;
    std::string getErrorMessage() const;
};

// ExpressionParser - main interface
class ExpressionParser {
private:
    std::unique_ptr<ASTNode> ast;
    std::string lastError;
    bool hasParsingError;
    
public:
    ExpressionParser();
    
    // Parse an expression string into an AST
    bool parse(const std::string& expression);
    
    // Evaluate the parsed expression
    double evaluate(const std::map<std::string, double>& variables = {}) const;
    
    // Get string representation of the AST
    std::string toString() const;
    
    // Get the AST for further manipulation
    const ASTNode* getAST() const;
    
    // Error handling
    bool hasError() const { return hasParsingError; }
    std::string getError() const { return lastError; }
    
    // Utility methods
    std::unique_ptr<ASTNode> cloneAST() const;
};

#endif // EXPRESSION_PARSER_H
