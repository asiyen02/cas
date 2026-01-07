# CAS Calculator - Computer Algebra System

A C++ implementation of a Computer Algebra System (CAS) with a focus on expression parsing, symbolic computation, and graphing capabilities.

## Project Structure

```
cas/
├── main.cpp                  # CLI interface and dispatcher
├── parser/                  
│   ├── ExpressionParser.h    # Parser interface and AST definitions
│   └── ExpressionParser.cpp  # Tokenization + AST building implementation
├── cas/
│   └── SymbolicEngine.h/.cpp # Symbolic computation, differentiation, integration
├── evaluator/
│   └── Evaluator.h/.cpp     # Evaluate parsed expressions numerically (future)
├── grapher/
│   ├── Grapher.h/.cpp       # SFML-based GUI grapher (optional)
│   └── ConsoleGrapher.h/.cpp # Console-based ASCII grapher
├── commands/
│   └── CommandDispatcher.h/.cpp # Interprets CLI commands (future)
└── test_parser.cpp          # Test program for the expression parser
```

## Features

### Expression Parser (Implemented)
- **Tokenization**: Converts mathematical expressions into tokens
- **AST Building**: Creates Abstract Syntax Trees for expressions
- **Operator Precedence**: Handles proper operator precedence and associativity
- **Functions**: Supports mathematical functions (sin, cos, tan, log, ln, sqrt, abs)
- **Variables**: Supports variable substitution and evaluation
- **Error Handling**: Comprehensive error reporting for invalid expressions

### Symbolic Engine (Implemented)
- **Symbolic Expressions**: Represents mathematical expressions symbolically
- **Differentiation**: Automatic differentiation with product rule, quotient rule, chain rule
- **Integration**: Basic integration rules for polynomials, trigonometric, and logarithmic functions
- **Simplification**: Expression simplification and constant folding
- **AST Conversion**: Converts parsed AST nodes to symbolic expressions
- **Equation Solving**: Basic linear equation solving
- **Factoring**: Basic polynomial factoring

### Grapher Module (Implemented)
- **Console Grapher**: ASCII-based plotting in terminal (no external dependencies)
- **SFML Grapher**: GUI-based plotting with SFML (optional, requires SFML installation)
- **Multiple Functions**: Plot multiple functions simultaneously with different colors/symbols
- **Customizable Ranges**: Set custom x and y axis ranges
- **Grid and Axes**: Optional grid lines and coordinate axes
- **Function Labels**: Display function names and legends
- **Integration**: Seamless integration with parser and symbolic engine

### Supported Operations
- **Arithmetic**: `+`, `-`, `*`, `/`, `^` (power)
- **Functions**: `sin()`, `cos()`, `tan()`, `log()`, `ln()`, `sqrt()`, `abs()`
- **Parentheses**: Full support for grouping expressions
- **Variables**: Any alphanumeric identifier (e.g., `x`, `y`, `theta`)

### Example Expressions
```
2 + 3 * 4           # Result: 14 (respects precedence)
(2 + 3) * 4         # Result: 20 (parentheses override precedence)
x^2 + y^2           # Variables with power
sin(3.14159)        # Trigonometric functions
sqrt(x^2 + y^2)     # Nested functions
log(100)            # Logarithm base 10
ln(2.718)           # Natural logarithm
abs(-5)             # Absolute value
```

## Building the Project

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, or MSVC)

### Build Instructions

1. **Clone and navigate to the project directory:**
   ```bash
   cd cas
   ```

2. **Create a build directory:**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure and build:**
   ```bash
   cmake ..
   cmake --build .
   ```

4. **Run the test program:**
   ```bash
   ./bin/test_parser
   ```

5. **Run the test programs:**
   ```bash
   # Test the parser
   ./bin/test_parser
   
   # Test the symbolic engine
   ./bin/test_symbolic
   
   # Test the console grapher
   ./bin/test_console_grapher
   
   # Run the integrated CAS demo
   ./bin/cas_grapher_demo
   
   # Run the comprehensive CAS demo
   ./bin/cas_demo
   ```

## Usage

### Console Grapher Demo
Run `./bin/test_console_grapher` to see the console-based plotting:
```
=== Console Grapher Demo ===
Creating a console grapher with multiple functions...
Adding functions to plot...
Functions added successfully!
Plotting functions...

5: y = x³ - 2x.........................+.....................................11
.       .       .      2.       .       +       .       .2      .       .  111
.       .       .       .       .       +       .       .       .       .11
........................2...............+.6.............5.............111.......
```

### Integrated CAS Demo
Run `./bin/cas_grapher_demo` to see the full integration:
```
=== Integrated CAS Grapher Demo ===
1. Original function: x^2 + 2*x + 1
   Symbolic representation: (((x ^ 2) + (2 * x)) + 1)
   Derivative: ((((x ^ 1) * (2 * 1)) + ((2 * 1) + (x * 0))) + 0)
   Integral: ((((x ^ 3) / 3) + (2 * ((x ^ 2) / 2))) + (1 * x))
```

### Interactive Mode
Run `./bin/cas_main` to start the interactive expression evaluator:
```
> 2 + 3 * 4
  AST: (2 + (3 * 4))
  Result: 14

> x^2 + y^2
  AST: ((x ^ 2) + (y ^ 2))
  Evaluation error: Undefined variable: x
```

### Testing
Run `./bin/test_parser` to see comprehensive tests of the parser functionality.

## Architecture

### Expression Parser Components

1. **Lexer (`Lexer` class)**
   - Tokenizes input strings into tokens
   - Handles numbers, operators, functions, variables, and parentheses
   - Supports scientific notation and whitespace

2. **Parser (`Parser` class)**
   - Implements recursive descent parsing
   - Builds Abstract Syntax Trees (AST)
   - Handles operator precedence and associativity

3. **AST Nodes**
   - `BinaryOpNode`: Binary operations (+, -, *, /, ^)
   - `UnaryOpNode`: Unary operations (+, -, functions)
   - `NumberNode`: Numeric literals
   - `VariableNode`: Variable references
   - `FunctionNode`: Function calls

4. **ExpressionParser (Main Interface)**
   - High-level interface for parsing and evaluating expressions
   - Error handling and AST management
   - Variable substitution support

### Design Principles

- **Extensibility**: Easy to add new operators and functions
- **Error Handling**: Comprehensive error reporting with position information
- **Performance**: Efficient parsing and evaluation
- **Memory Safety**: Uses smart pointers for automatic memory management
- **Type Safety**: Strong typing throughout the codebase

## Grapher Features

### Console Grapher
- **ASCII Plotting**: Displays graphs using ASCII characters in the terminal
- **Multiple Functions**: Plot up to 10 different functions simultaneously
- **Custom Symbols**: Each function uses a different character for identification
- **Grid and Axes**: Optional coordinate grid and axis lines
- **Customizable Ranges**: Set custom x and y axis ranges
- **Function Labels**: Display function names at the top of the plot
- **No Dependencies**: Works without any external libraries

### SFML Grapher (Optional)
- **GUI Interface**: Modern graphical plotting interface
- **Color Coding**: Different colors for each function
- **Interactive**: Zoom, pan, and explore graphs
- **High Resolution**: Smooth, high-quality rendering
- **Real-time Updates**: Dynamic plotting and updates

### Integration Examples
```cpp
// Create a console grapher
ConsoleGrapher::PlotSettings settings;
settings.width = 80;
settings.height = 30;
settings.xMin = -5.0;
settings.xMax = 5.0;
ConsoleGrapher grapher(settings);

// Add functions to plot
grapher.addFunction("x^2", "y = x²", '1');
grapher.addFunction("sin(x)", "y = sin(x)", '2');
grapher.addFunction("cos(x)", "y = cos(x)", '3');

// Plot the functions
grapher.plot();
```

## Future Development

### Planned Components

1. **Enhanced Symbolic Engine**
   - More advanced integration techniques
   - Systems of equations solving
   - Complex number support
   - Matrix operations

2. **Evaluator** (`evaluator/Evaluator`)
   - Numerical evaluation with high precision
   - Complex number support
   - Matrix operations

3. **Enhanced Grapher**
   - 3D plotting capabilities
   - Parametric equations
   - Polar coordinates
   - Export to image files

4. **Command Dispatcher** (`commands/CommandDispatcher`)
   - CLI command interpretation
   - Batch processing
   - Script execution

5. **GUI Interface**
   - Modern desktop application
   - Real-time plotting
   - Interactive mathematical input

## Contributing

This is a learning project for building a Computer Algebra System. Feel free to contribute by:

1. Adding new mathematical functions
2. Implementing symbolic computation features
3. Improving error handling and user experience
4. Adding tests and documentation

## License

This project is open source and available under the MIT License.
