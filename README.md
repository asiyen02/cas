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

# CAS (Computer Algebra System)

Lightweight C++ CAS providing an expression parser, a symbolic engine (differentiate/integrate),
and two grapher backends: an ASCII console grapher and an SFML GUI grapher.

## Quick start

- Build:
```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```
- Run interactive REPL:
```bash
./interactive_cas
```
Type commands such as: `parse <expr>`, `eval <expr>`, `diff <expr>`, `integrate <expr>`, `simplify <expr>`, `graph <expr>`.

## Graphing
- `graph <expr>` opens the SFML GUI grapher when SFML is available; otherwise it automatically uses the console grapher.
- Options: `xmin:<v> xmax:<v> ymin:<v> ymax:<v> width:<cols> height:<rows>`
- The REPL auto-samples the function to adjust `y` range unless `ymin`/`ymax` are provided.

## Notable files
- `parser/` — expression parser and AST
- `cas/` — symbolic engine (differentiate, integrate, simplify, pretty-print)
- `grapher/ConsoleGrapher.*` — ASCII plotting
- `grapher/Grapher.*` — SFML GUI plotting
- `interactive_cas.cpp` — REPL that integrates parser, symbolic engine, and grapher

## Recent changes
- `interactive_cas` REPL with `graph` command that opens GUI when available.
- Improved symbolic pretty-printer: shows `2x` instead of `x * 2` and collapses numeric products.
- Auto-scaling of y-range for graphs.
- Added `.gitignore` to exclude build artifacts.


