# GEMINI.md

## Project Overview

This project is for "icsograf", a designer for knotian knotworks. It's a C++ application built with the Qt framework. The project uses CMake for its build system. The application allows users to create, edit, and save Celtic knot designs. It also supports plugins to extend its functionality.

The codebase is organized into the following main directories:

*   `src/`: Contains the main source code for the application.
    *   `dialogs/`: UI-related classes, including the main window.
    *   `graph/`: Core logic for representing and manipulating the knot graph.
    *   `io/`: Code for loading and saving knots, as well as exporting to other formats.
    *   `scripting/`: Scripting engine integration and plugin management.
    *   `widgets/`: Custom Qt widgets used throughout the application.
*   `data/`: Application data, such as icons, translations, and plugins.
*   `cmake/`: Custom CMake modules.

## Building and Running

### Dependencies

*   Qt Framework (version 4.7 or later)
*   CMake (version 3.5 or later)

### Build Steps

To build the project, run the following commands from the root directory:

```bash
mkdir build
cd build
cmake ..
make
```

The executable will be located in the `build/bin` directory.

## Development Conventions

*   The code follows the Qt coding style.
*   The project uses a resource manager to handle application-wide resources like cusp shapes and edge types.
*   The UI is designed using Qt Designer, and the `.ui` files are compiled into headers.
*   The application supports plugins, which can be written in C++ or a scripting language.
*   Internationalization is supported through Qt's translation system.

## phase 1 :

*at each step, give assesment to the user  and then build to check for errors*

- [x] assess refactoring edge_normal, edge_2strand and edge_3strand ; for example :
 - create a class for 'QLineF Edge_2Strand::handle' logic
 - create a class for inverted logic







