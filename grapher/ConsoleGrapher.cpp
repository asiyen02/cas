#include "ConsoleGrapher.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

// Function constructor
ConsoleGrapher::Function::Function(const std::string& expr, const std::string& funcName, char funcSymbol)
    : expression(expr), name(funcName), symbol(funcSymbol), ast(nullptr) {
    if (!expr.empty()) {
        ExpressionParser parser;
        if (parser.parse(expr)) {
            ast = parser.cloneAST();
        }
    }
}

// ConsoleGrapher default constructor
ConsoleGrapher::ConsoleGrapher() : settings(PlotSettings()) {
    clearBuffer();
}

// ConsoleGrapher constructor with settings
ConsoleGrapher::ConsoleGrapher(const PlotSettings& settings) : settings(settings) {
    clearBuffer();
}

bool ConsoleGrapher::addFunction(const std::string& expression, const std::string& name, char symbol) {
    try {
        ExpressionParser parser;
        if (parser.parse(expression)) {
            functions.emplace_back(expression, name, symbol);
            functions.back().ast = parser.cloneAST();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing function '" << expression << "': " << e.what() << std::endl;
    }
    return false;
}

void ConsoleGrapher::removeFunction(size_t index) {
    if (index < functions.size()) {
        functions.erase(functions.begin() + index);
    }
}

void ConsoleGrapher::clearFunctions() {
    functions.clear();
}

void ConsoleGrapher::setPlotRange(double xMin, double xMax, double yMin, double yMax) {
    settings.xMin = xMin;
    settings.xMax = xMax;
    settings.yMin = yMin;
    settings.yMax = yMax;
}

void ConsoleGrapher::setDisplaySize(int width, int height) {
    settings.width = width;
    settings.height = height;
    clearBuffer();
}

void ConsoleGrapher::plot() {
    clearBuffer();
    
    if (settings.showGrid) {
        drawGrid();
    }
    
    if (settings.showAxes) {
        drawAxes();
    }
    
    drawFunctions();
    drawLabels();
    
    printBuffer();
}

void ConsoleGrapher::plotToFile(const std::string& filename) {
    clearBuffer();
    
    if (settings.showGrid) {
        drawGrid();
    }
    
    if (settings.showAxes) {
        drawAxes();
    }
    
    drawFunctions();
    drawLabels();
    
    printBufferToFile(filename);
}

double ConsoleGrapher::screenXToWorld(int screenX) const {
    return settings.xMin + (screenX * (settings.xMax - settings.xMin)) / settings.width;
}

double ConsoleGrapher::screenYToWorld(int screenY) const {
    return settings.yMax - (screenY * (settings.yMax - settings.yMin)) / settings.height;
}

int ConsoleGrapher::worldXToScreen(double worldX) const {
    return static_cast<int>(((worldX - settings.xMin) * settings.width) / (settings.xMax - settings.xMin));
}

int ConsoleGrapher::worldYToScreen(double worldY) const {
    return static_cast<int>(((settings.yMax - worldY) * settings.height) / (settings.yMax - settings.yMin));
}

void ConsoleGrapher::clearBuffer() {
    displayBuffer.resize(settings.height);
    for (auto& row : displayBuffer) {
        row.resize(settings.width, ' ');
    }
}

void ConsoleGrapher::drawGrid() {
    // Draw vertical grid lines
    for (int x = static_cast<int>(settings.xMin); x <= static_cast<int>(settings.xMax); ++x) {
        if (x == 0) continue; // Skip the y-axis
        
        int screenX = worldXToScreen(x);
        if (screenX >= 0 && screenX < settings.width) {
            for (int y = 0; y < settings.height; ++y) {
                displayBuffer[y][screenX] = settings.gridChar;
            }
        }
    }

    // Draw horizontal grid lines
    for (int y = static_cast<int>(settings.yMin); y <= static_cast<int>(settings.yMax); ++y) {
        if (y == 0) continue; // Skip the x-axis
        
        int screenY = worldYToScreen(y);
        if (screenY >= 0 && screenY < settings.height) {
            for (int x = 0; x < settings.width; ++x) {
                displayBuffer[screenY][x] = settings.gridChar;
            }
        }
    }
}

void ConsoleGrapher::drawAxes() {
    // Draw x-axis
    int yAxisScreenY = worldYToScreen(0);
    if (yAxisScreenY >= 0 && yAxisScreenY < settings.height) {
        for (int x = 0; x < settings.width; ++x) {
            displayBuffer[yAxisScreenY][x] = settings.axesChar;
        }
    }

    // Draw y-axis
    int xAxisScreenX = worldXToScreen(0);
    if (xAxisScreenX >= 0 && xAxisScreenX < settings.width) {
        for (int y = 0; y < settings.height; ++y) {
            displayBuffer[y][xAxisScreenX] = settings.axesChar;
        }
    }
}

void ConsoleGrapher::drawFunctions() {
    for (const auto& func : functions) {
        drawFunction(func);
    }
}

void ConsoleGrapher::drawFunction(const Function& func) {
    if (!func.ast) return;

    const int numPoints = settings.width;
    const double step = (settings.xMax - settings.xMin) / numPoints;

    for (int i = 0; i <= numPoints; ++i) {
        double x = settings.xMin + i * step;
        
        try {
            std::map<std::string, double> variables = {{"x", x}};
            double y = func.ast->evaluate(variables);
            
            // Check if y is within the plot range
            if (y >= settings.yMin && y <= settings.yMax && 
                std::isfinite(y) && !std::isnan(y)) {
                int screenX = worldXToScreen(x);
                int screenY = worldYToScreen(y);
                
                if (isValidPoint(screenX, screenY)) {
                    displayBuffer[screenY][screenX] = func.symbol;
                }
            }
        } catch (const std::exception&) {
            // Skip points that cause evaluation errors
            continue;
        }
    }
}

void ConsoleGrapher::drawLabels() {
    // Draw function names at the top
    for (size_t i = 0; i < functions.size() && i < 5; ++i) {
        if (!functions[i].name.empty()) {
            std::string label = functions[i].symbol + std::string(": ") + functions[i].name;
            if (label.length() < settings.width) {
                for (size_t j = 0; j < label.length(); ++j) {
                    if (j < settings.width) {
                        displayBuffer[0][j] = label[j];
                    }
                }
            }
        }
    }
}

std::string ConsoleGrapher::formatNumber(double value) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

void ConsoleGrapher::printBuffer() {
    std::cout << "\n";
    for (const auto& row : displayBuffer) {
        for (char c : row) {
            std::cout << c;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void ConsoleGrapher::printBufferToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& row : displayBuffer) {
            for (char c : row) {
                file << c;
            }
            file << "\n";
        }
        file.close();
        std::cout << "Graph saved to " << filename << std::endl;
    } else {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
    }
}

bool ConsoleGrapher::isValidPoint(int x, int y) const {
    return x >= 0 && x < settings.width && y >= 0 && y < settings.height;
}
