# CrystalBoard 🔮

**A crystal-clear, transparent canvas that floats above your desktop.**

CrystalBoard is a simple yet elegant desktop application that provides a transparent drawing surface, allowing you to sketch, annotate, or doodle over any of your open windows. It's built with C++ and the Qt framework for a lightweight and responsive experience.

## Features

### Core Functionality
- **Transparent Canvas**: The main window is fully transparent, letting you see everything on your desktop underneath.
- **Smooth Drawing**: Use your mouse to draw fluid lines with ease.
- **Versatile Color Picker**: A dedicated widget with RGB sliders allows for precise color selection. The slider tracks dynamically update to show a pure color gradient.

### User Interaction
- **Simple Controls**:
    - `Left-click + Drag`: Draw on the canvas.
    - `Right-click`: Toggle the color picker.
    - `Scroll Wheel`: Undo/Redo your strokes.
    - `Middle-click + Scroll Wheel`: Adjust brush size.
    - `Esc Key`: Instantly close the application.
- **Custom Brush Cursor**: A circular cursor provides a real-time preview of your selected brush color and size.

## Tech Stack
- **C++20**
- **Qt 6**
- **CMake**

## How to Build and Run

### Prerequisites
- A modern C++ compiler (GCC, Clang, MSVC)
- CMake (>= 3.16)
- Qt 6 (Core, GUI, Widgets)

### Build Steps
```bash
# 1. Clone the repository
# git clone <repository-url>
# cd CrystalBoard

# 2. Create and navigate to the build directory
mkdir -p build && cd build

# 3. Configure the project with CMake
cmake ..

# 4. Compile the source code
make
```

### Installation (Optional)
To make the `CrystalBoard` command available system-wide, you can copy the compiled binary to a directory in your system's `PATH`. A common choice is `~/.local/bin`.

```bash
# This command assumes you are in the project's root directory
cp CrystalBoard ~/.local/bin/
```

### Run the Application
After a successful build, the executable will be located in the project's root directory. If you've performed the installation step, you can run it from anywhere.
```bash
# From the project root
./CrystalBoard

# Or from anywhere, if installed
CrystalBoard
```

## Contributing
This project is shared as-is, and active maintenance is not guaranteed. However, you are welcome to fork the repository and experiment. If you find a bug, you can open an issue to report it.

## Usage Example: Hyprland Integration
For users of the Hyprland window manager, a detailed guide on how to integrate CrystalBoard with keyboard shortcuts and window rules is available.

➡️ **[View Hyprland Integration Guide](./docs/HYPRLAND_INTEGRATION.md)**

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
