# CrystalBoard v0.1.0

A simple, transparent desktop canvas application for basic drawing.

## Features

- **Transparent Window**: Draw directly over your desktop and other applications.
- **Basic Drawing**: Left-click and drag to draw smooth lines.
- **Color Picker**: Right-click to toggle a color picker with RGB sliders.
- **Dynamic Color Tracks**: Slider tracks dynamically show the pure color channel (e.g., the red slider's track is pure red).
- **Undo/Redo**: Use the mouse wheel to undo or redo strokes.
- **Custom Cursor**: A circular cursor shows the current brush color.
- **Quit**: Press the `Esc` key to close the application.

## How to Build and Run

### Prerequisites

- A C++ compiler (like GCC or Clang)
- CMake (version 3.5 or higher)
- Qt (version 6)

### Building

1.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

2.  **Run CMake and Make:**
    ```bash
    cmake ..
    make
    ```

### Running

The executable `CrystalBoard` will be created in the project's root directory.

```bash
./CrystalBoard
```
