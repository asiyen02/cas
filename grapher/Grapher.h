#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "../parser/ExpressionParser.h"
#include "../cas/SymbolicEngine.h"

class Grapher {
public:
    struct PlotSettings {
        double xMin = -10.0;
        double xMax = 10.0;
        double yMin = -10.0;
        double yMax = 10.0;
        int width = 800;
        int height = 600;
        std::string title = "CAS Grapher";
        bool showGrid = true;
        bool showAxes = true;
        sf::Color backgroundColor = sf::Color::White;
        sf::Color gridColor = sf::Color(200, 200, 200);
        sf::Color axesColor = sf::Color::Black;
        sf::Color plotColor = sf::Color::Blue;
    };

    struct Function {
        std::string expression;
        std::string name;
        sf::Color color;
        std::unique_ptr<ASTNode> ast;
        
        Function(const std::string& expr, const std::string& funcName = "", 
                const sf::Color& funcColor = sf::Color::Blue);
    };

    Grapher();
    Grapher(const PlotSettings& settings);
    ~Grapher();

    // Add functions to plot
    bool addFunction(const std::string& expression, const std::string& name = "", 
                    const sf::Color& color = sf::Color::Blue);
    void removeFunction(size_t index);
    void clearFunctions();
    size_t getFunctionCount() const { return functions.size(); }

    // Plot settings
    void setPlotRange(double xMin, double xMax, double yMin, double yMax);
    void setWindowSize(int width, int height);
    void setTitle(const std::string& title);
    void setGridVisible(bool visible);
    void setAxesVisible(bool visible);

    // Main plotting functions
    void plot();
    void run();

    // Utility functions
    sf::Vector2f worldToScreen(double x, double y) const;
    sf::Vector2f screenToWorld(int screenX, int screenY) const;

private:
    PlotSettings settings;
    std::vector<Function> functions;
    sf::RenderWindow window;
    sf::Font font;
    
    // Drawing functions
    void drawGrid();
    void drawAxes();
    void drawFunctions();
    void drawLabels();
    void drawFunction(const Function& func);
    
    // Helper functions
    bool loadFont();
    std::string formatNumber(double value) const;
    void handleEvents();
    void updateView();
    
    // Coordinate transformation
    double screenXToWorld(int screenX) const;
    double screenYToWorld(int screenY) const;
    int worldXToScreen(double worldX) const;
    int worldYToScreen(double worldY) const;
};
