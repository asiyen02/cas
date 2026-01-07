#include "Grapher.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

// Function constructor
Grapher::Function::Function(const std::string& expr, const std::string& funcName, const sf::Color& funcColor)
    : expression(expr), name(funcName), color(funcColor), ast(nullptr) {
    if (!expr.empty()) {
        ExpressionParser parser;
        if (parser.parse(expr)) {
            ast = parser.cloneAST();
        }
    }
}

// Grapher default constructor
Grapher::Grapher() : settings(PlotSettings()) {
    window.create(sf::VideoMode(settings.width, settings.height), settings.title);
    window.setFramerateLimit(60);
    
    if (!loadFont()) {
        std::cerr << "Warning: Could not load font. Text rendering will be disabled." << std::endl;
    }
}

// Grapher constructor with settings
Grapher::Grapher(const PlotSettings& settings) : settings(settings) {
    window.create(sf::VideoMode(settings.width, settings.height), settings.title);
    window.setFramerateLimit(60);
    
    if (!loadFont()) {
        std::cerr << "Warning: Could not load font. Text rendering will be disabled." << std::endl;
    }
}

Grapher::~Grapher() {
    if (window.isOpen()) {
        window.close();
    }
}

bool Grapher::addFunction(const std::string& expression, const std::string& name, const sf::Color& color) {
    try {
        ExpressionParser parser;
        if (parser.parse(expression)) {
            functions.emplace_back(expression, name, color);
            functions.back().ast = parser.cloneAST();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing function '" << expression << "': " << e.what() << std::endl;
    }
    return false;
}

void Grapher::removeFunction(size_t index) {
    if (index < functions.size()) {
        functions.erase(functions.begin() + index);
    }
}

void Grapher::clearFunctions() {
    functions.clear();
}

void Grapher::setPlotRange(double xMin, double xMax, double yMin, double yMax) {
    settings.xMin = xMin;
    settings.xMax = xMax;
    settings.yMin = yMin;
    settings.yMax = yMax;
}

void Grapher::setWindowSize(int width, int height) {
    settings.width = width;
    settings.height = height;
    if (window.isOpen()) {
        window.setSize(sf::Vector2u(width, height));
    }
}

void Grapher::setTitle(const std::string& title) {
    settings.title = title;
    if (window.isOpen()) {
        window.setTitle(title);
    }
}

void Grapher::setGridVisible(bool visible) {
    settings.showGrid = visible;
}

void Grapher::setAxesVisible(bool visible) {
    settings.showAxes = visible;
}

void Grapher::plot() {
    if (!window.isOpen()) {
        return;
    }

    window.clear(settings.backgroundColor);

    if (settings.showGrid) {
        drawGrid();
    }
    
    if (settings.showAxes) {
        drawAxes();
    }
    
    drawFunctions();
    drawLabels();

    window.display();
}

void Grapher::run() {
    if (!window.isOpen()) {
        return;
    }

    while (window.isOpen()) {
        handleEvents();
        plot();
    }
}

sf::Vector2f Grapher::worldToScreen(double x, double y) const {
    int screenX = worldXToScreen(x);
    int screenY = worldYToScreen(y);
    return sf::Vector2f(static_cast<float>(screenX), static_cast<float>(screenY));
}

sf::Vector2f Grapher::screenToWorld(int screenX, int screenY) const {
    double worldX = screenXToWorld(screenX);
    double worldY = screenYToWorld(screenY);
    return sf::Vector2f(static_cast<float>(worldX), static_cast<float>(worldY));
}

void Grapher::drawGrid() {
    // Draw vertical grid lines
    for (int x = static_cast<int>(settings.xMin); x <= static_cast<int>(settings.xMax); ++x) {
        if (x == 0) continue; // Skip the y-axis
        
        int screenX = worldXToScreen(x);
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(static_cast<float>(screenX), 0), settings.gridColor),
            sf::Vertex(sf::Vector2f(static_cast<float>(screenX), static_cast<float>(settings.height)), settings.gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }

    // Draw horizontal grid lines
    for (int y = static_cast<int>(settings.yMin); y <= static_cast<int>(settings.yMax); ++y) {
        if (y == 0) continue; // Skip the x-axis
        
        int screenY = worldYToScreen(y);
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(0, static_cast<float>(screenY)), settings.gridColor),
            sf::Vertex(sf::Vector2f(static_cast<float>(settings.width), static_cast<float>(screenY)), settings.gridColor)
        };
        window.draw(line, 2, sf::Lines);
    }
}

void Grapher::drawAxes() {
    // Draw x-axis
    int yAxisScreenY = worldYToScreen(0);
    if (yAxisScreenY >= 0 && yAxisScreenY < settings.height) {
        sf::Vertex xAxis[] = {
            sf::Vertex(sf::Vector2f(0, static_cast<float>(yAxisScreenY)), settings.axesColor),
            sf::Vertex(sf::Vector2f(static_cast<float>(settings.width), static_cast<float>(yAxisScreenY)), settings.axesColor)
        };
        window.draw(xAxis, 2, sf::Lines);
    }

    // Draw y-axis
    int xAxisScreenX = worldXToScreen(0);
    if (xAxisScreenX >= 0 && xAxisScreenX < settings.width) {
        sf::Vertex yAxis[] = {
            sf::Vertex(sf::Vector2f(static_cast<float>(xAxisScreenX), 0), settings.axesColor),
            sf::Vertex(sf::Vector2f(static_cast<float>(xAxisScreenX), static_cast<float>(settings.height)), settings.axesColor)
        };
        window.draw(yAxis, 2, sf::Lines);
    }
}

void Grapher::drawFunctions() {
    for (const auto& func : functions) {
        drawFunction(func);
    }
}

void Grapher::drawFunction(const Function& func) {
    if (!func.ast) return;

    std::vector<sf::Vertex> points;
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
                points.emplace_back(sf::Vector2f(static_cast<float>(screenX), static_cast<float>(screenY)), func.color);
            }
        } catch (const std::exception&) {
            // Skip points that cause evaluation errors
            continue;
        }
    }

    if (points.size() > 1) {
        window.draw(points.data(), points.size(), sf::LineStrip);
    }
}

void Grapher::drawLabels() {
    if (!font.getInfo().family.empty()) {
        // Draw axis labels
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(12);
        text.setFillColor(settings.axesColor);

        // X-axis label
        text.setString("x");
        text.setPosition(static_cast<float>(settings.width - 20), static_cast<float>(worldYToScreen(0) - 20));
        window.draw(text);

        // Y-axis label
        text.setString("y");
        text.setPosition(static_cast<float>(worldXToScreen(0) + 5), 10.0f);
        window.draw(text);

        // Draw function names if available
        text.setCharacterSize(10);
        for (size_t i = 0; i < functions.size(); ++i) {
            if (!functions[i].name.empty()) {
                text.setString(functions[i].name);
                text.setFillColor(functions[i].color);
                text.setPosition(10.0f, static_cast<float>(20 + i * 20));
                window.draw(text);
            }
        }
    }
}

bool Grapher::loadFont() {
    // Try to load a system font
    if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        return true;
    }
    if (font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
        return true;
    }
    if (font.loadFromFile("C:/Windows/Fonts/tahoma.ttf")) {
        return true;
    }
    return false;
}

std::string Grapher::formatNumber(double value) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

void Grapher::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                break;
            case sf::Event::Resized:
                settings.width = event.size.width;
                settings.height = event.size.height;
                window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(settings.width), static_cast<float>(settings.height))));
                break;
            default:
                break;
        }
    }
}

void Grapher::updateView() {
    window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(settings.width), static_cast<float>(settings.height))));
}

double Grapher::screenXToWorld(int screenX) const {
    return settings.xMin + (screenX * (settings.xMax - settings.xMin)) / settings.width;
}

double Grapher::screenYToWorld(int screenY) const {
    return settings.yMax - (screenY * (settings.yMax - settings.yMin)) / settings.height;
}

int Grapher::worldXToScreen(double worldX) const {
    return static_cast<int>(((worldX - settings.xMin) * settings.width) / (settings.xMax - settings.xMin));
}

int Grapher::worldYToScreen(double worldY) const {
    return static_cast<int>(((settings.yMax - worldY) * settings.height) / (settings.yMax - settings.yMin));
}
