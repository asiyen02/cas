#ifndef SYMBOLIC_ENGINE_H
#define SYMBOLIC_ENGINE_H

#include "../parser/ExpressionParser.h"
#include <memory>
#include <string>
#include <map>
#include <vector>

// Forward declarations
class SymbolicExpression;
class SymbolicBinaryOp;
class SymbolicUnaryOp;
class SymbolicNumber;
class SymbolicVariable;
class SymbolicFunction;

// Symbolic expression base class
class SymbolicExpression {
public:
    virtual ~SymbolicExpression() = default;
    virtual std::string toString() const = 0;
    virtual std::unique_ptr<SymbolicExpression> differentiate(const std::string& variable) const = 0;
    virtual std::unique_ptr<SymbolicExpression> integrate(const std::string& variable) const = 0;
    virtual std::unique_ptr<SymbolicExpression> simplify() const = 0;
    virtual std::unique_ptr<SymbolicExpression> clone() const = 0;
    virtual double evaluate(const std::map<std::string, double>& variables = {}) const = 0;
    virtual bool isConstant() const = 0;
    virtual bool isZero() const = 0;
    virtual bool isOne() const = 0;
};

// Symbolic binary operation
class SymbolicBinaryOp : public SymbolicExpression {
public:
    enum class OpType {
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        POWER
    };
    
    OpType op;
    std::unique_ptr<SymbolicExpression> left;
    std::unique_ptr<SymbolicExpression> right;
    
    SymbolicBinaryOp(OpType operation, std::unique_ptr<SymbolicExpression> l, std::unique_ptr<SymbolicExpression> r)
        : op(operation), left(std::move(l)), right(std::move(r)) {}
    
    std::string toString() const override;
    std::unique_ptr<SymbolicExpression> differentiate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> integrate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> simplify() const override;
    std::unique_ptr<SymbolicExpression> clone() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    bool isConstant() const override;
    bool isZero() const override;
    bool isOne() const override;
};

// Symbolic unary operation
class SymbolicUnaryOp : public SymbolicExpression {
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
    std::unique_ptr<SymbolicExpression> operand;
    
    SymbolicUnaryOp(OpType operation, std::unique_ptr<SymbolicExpression> op)
        : op(operation), operand(std::move(op)) {}
    
    std::string toString() const override;
    std::unique_ptr<SymbolicExpression> differentiate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> integrate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> simplify() const override;
    std::unique_ptr<SymbolicExpression> clone() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    bool isConstant() const override;
    bool isZero() const override;
    bool isOne() const override;
};

// Symbolic number
class SymbolicNumber : public SymbolicExpression {
public:
    double value;
    
    SymbolicNumber(double val) : value(val) {}
    
    std::string toString() const override;
    std::unique_ptr<SymbolicExpression> differentiate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> integrate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> simplify() const override;
    std::unique_ptr<SymbolicExpression> clone() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    bool isConstant() const override { return true; }
    bool isZero() const override { return value == 0.0; }
    bool isOne() const override { return value == 1.0; }
};

// Symbolic variable
class SymbolicVariable : public SymbolicExpression {
public:
    std::string name;
    
    SymbolicVariable(const std::string& varName) : name(varName) {}
    
    std::string toString() const override;
    std::unique_ptr<SymbolicExpression> differentiate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> integrate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> simplify() const override;
    std::unique_ptr<SymbolicExpression> clone() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    bool isConstant() const override { return false; }
    bool isZero() const override { return false; }
    bool isOne() const override { return false; }
};

// Symbolic function
class SymbolicFunction : public SymbolicExpression {
public:
    std::string functionName;
    std::vector<std::unique_ptr<SymbolicExpression>> arguments;
    
    SymbolicFunction(const std::string& funcName, std::vector<std::unique_ptr<SymbolicExpression>> args)
        : functionName(funcName), arguments(std::move(args)) {}
    
    std::string toString() const override;
    std::unique_ptr<SymbolicExpression> differentiate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> integrate(const std::string& variable) const override;
    std::unique_ptr<SymbolicExpression> simplify() const override;
    std::unique_ptr<SymbolicExpression> clone() const override;
    double evaluate(const std::map<std::string, double>& variables = {}) const override;
    bool isConstant() const override;
    bool isZero() const override;
    bool isOne() const override;
};

// Main Symbolic Engine class
class SymbolicEngine {
private:
    std::unique_ptr<SymbolicExpression> expression;
    
    // Helper functions for simplification
    std::unique_ptr<SymbolicExpression> simplifyBinaryOp(SymbolicBinaryOp::OpType op, 
                                                        std::unique_ptr<SymbolicExpression> left,
                                                        std::unique_ptr<SymbolicExpression> right) const;
    std::unique_ptr<SymbolicExpression> simplifyUnaryOp(SymbolicUnaryOp::OpType op,
                                                       std::unique_ptr<SymbolicExpression> operand) const;
    
    // AST to symbolic conversion
    std::unique_ptr<SymbolicExpression> convertASTToSymbolic(const ASTNode* ast);
    

    
public:
    SymbolicEngine();
    
    // Convert from AST to symbolic expression
    bool parseFromAST(const ASTNode* ast);
    
    // Parse from string
    bool parseFromString(const std::string& expression);
    
    // Differentiation
    std::unique_ptr<SymbolicExpression> differentiate(const std::string& variable) const;
    
    // Simplification
    std::unique_ptr<SymbolicExpression> simplify() const;
    
    // Evaluation
    double evaluate(const std::map<std::string, double>& variables = {}) const;
    
    // String representation
    std::string toString() const;
    
    // Utility functions
    bool hasExpression() const { return expression != nullptr; }
    const SymbolicExpression* getExpression() const { return expression.get(); }
    
    // Advanced operations (future)
    std::unique_ptr<SymbolicExpression> integrate(const std::string& variable) const;
    std::unique_ptr<SymbolicExpression> solve(const std::string& variable) const;
    std::vector<std::unique_ptr<SymbolicExpression>> factor() const;
};

// Utility functions for creating symbolic expressions
std::unique_ptr<SymbolicExpression> makeSymbolicNumber(double value);
std::unique_ptr<SymbolicExpression> makeSymbolicVariable(const std::string& name);
std::unique_ptr<SymbolicExpression> makeSymbolicBinaryOp(SymbolicBinaryOp::OpType op,
                                                        std::unique_ptr<SymbolicExpression> left,
                                                        std::unique_ptr<SymbolicExpression> right);
std::unique_ptr<SymbolicExpression> makeSymbolicUnaryOp(SymbolicUnaryOp::OpType op,
                                                       std::unique_ptr<SymbolicExpression> operand);
std::unique_ptr<SymbolicExpression> makeSymbolicFunction(const std::string& funcName,
                                                        std::vector<std::unique_ptr<SymbolicExpression>> args);

#endif // SYMBOLIC_ENGINE_H
