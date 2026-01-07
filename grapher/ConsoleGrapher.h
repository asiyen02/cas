#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../parser/ExpressionParser.h"
#include "../cas/SymbolicEngine.h"

class ConsoleGrapher {
public:
    struct PlotSettings {
        double xMin = -10.0;
        double xMax = 10.0;
        double yMin = -10.0;
        double yMax = 10.0;
        int width = 80;
        int height = 24;
        bool showGrid = true;
        bool showAxes = true;
        char gridChar = '.';
        char axesChar = '+';
        char functionChar = '*';
    };

    struct Function {
        std::string expression;
        std::string name;
        char symbol;
        std::unique_ptr<ASTNode> ast;
        
        Function(const std::string& expr, const std::string& funcName = "", char funcSymbol = '*');
    };

    ConsoleGrapher();
    ConsoleGrapher(const PlotSettings& settings);

    // Add functions to plot
    bool addFunction(const std::string& expression, const std::string& name = "", char symbol = '*');
    void removeFunction(size_t index);
    void clearFunctions();

    // Plot settings
    void setPlotRange(double xMin, double xMax, double yMin, double yMax);
    void setDisplaySize(int width, int height);

    // Main plotting functions
    void plot();
    void plotToFile(const std::string& filename);

    // Utility functions
    double screenXToWorld(int screenX) const;
    double screenYToWorld(int screenY) const;
    int worldXToScreen(double worldX) const;
    int worldYToScreen(double worldY) const;

private:
    PlotSettings settings;
    std::vector<Function> functions;
    std::vector<std::vector<char>> displayBuffer;
    
    // Drawing functions
    void clearBuffer();
    void drawGrid();
    void drawAxes();
    void drawFunctions();
    void drawLabels();
    void drawFunction(const Function& func);
    
    // Helper functions
    std::string formatNumber(double value) const;
    void printBuffer();
    void printBufferToFile(const std::string& filename);
    
    // Coordinate transformation
    bool isValidPoint(int x, int y) const;
};
