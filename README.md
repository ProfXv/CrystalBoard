# 🔮 CrystalBoard 🎨

A crystal-clear, transparent canvas that floats above your desktop. CrystalBoard is a minimalist tool for quick sketches, notes, and annotations directly on your screen, designed to be intuitive and integrate seamlessly into your workflow with mouse-only controls.

## ✨ Features

- **Transparent Overlay**: Draw on a borderless, transparent window that floats over your other applications.
- **Mouse-Only Operation**: Designed from the ground up to be controlled entirely with a 3-button mouse and scroll wheel.
- **Scroll-Wheel Mode System**: Effortlessly cycle through tools and settings without moving your cursor.
- **On-the-Fly Adjustments**: Dynamically change color (Hue, Saturation, Value) and brush size.
- **Live Cursor Preview**: The cursor instantly reflects the current brush size and color.
- **Mode Indicator**: A temporary text indicator appears next to the cursor to show the current scroll mode and action.
- **Minimalist Settings Panel**: A clean, centered help panel that provides keybindings and stays out of the way.

## 🖱️ Controls

CrystalBoard is designed for efficiency. Here are all the controls:

| Action                 | Left Button        | Middle Button          | Right Button         | Scroll Wheel                               |
| ---------------------- | ------------------ | ---------------------- | -------------------- | ------------------------------------------ |
| **Click**              | *Drag* to draw     | Cycle Scroll Mode      | Clear entire canvas  | Use active mode                            |
| **Double-Click**       | Toggle Help Panel  | -                      | **Exit Application** | -                                          |

**Scroll Modes (Cycled by Middle-Click):**
1.  **History**: Undo / Redo
2.  **Hue**: Adjust color's hue
3.  **Saturation**: Adjust color's saturation
4.  **Brightness**: Adjust color's brightness
5.  **Size**: Adjust brush/eraser/font size
6.  **Tool**: Switch between Pen, Eraser, Text, Line, Rectangle, and Circle

**Keyboard:**
- `ESC`: **Exit Application**

## 🛠️ Building from Source

### Dependencies
- C++ Compiler (g++, clang++)
- CMake (version 3.5 or higher)
- Qt (version 6.x)

### Build Steps

1.  Clone the repository:
    ```bash
    git clone https://github.com/your-username/CrystalBoard.git
    cd CrystalBoard
    ```

2.  Create a build directory:
    ```bash
    mkdir build && cd build
    ```

3.  Run CMake and Make:
    ```bash
    cmake ..
    make
    ```

4.  Run the application:
    ```bash
    ./CrystalBoard
    ```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.