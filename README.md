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

<div align='center'>
<table style='width:100%; margin-top: 15px;'>
  <thead>
    <tr>
      <th style='padding: 10px; text-align: center;'></th>
      <th style='padding: 10px; text-align: center;'>Left Button</th>
      <th style='padding: 10px; text-align: center;'>Right Button</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th style='padding: 10px; text-align: right;'>Single-click</th>
      <td style='padding: 10px; text-align: center;'><u>Drag</u> to draw</td>
      <td style='padding: 10px; text-align: center;'>clear canvas</td>
    </tr>
    <tr>
      <th style='padding: 10px; text-align: right;'>Double-click</th>
      <td style='padding: 10px; text-align: center;'>toggle view</td>
      <td style='padding: 10px; text-align: center;'>reset config</td>
    </tr>
  </tbody>
</table>
<p style='text-align: center; margin-top: 15px; margin-bottom: 0; padding: 0;'><b>Scroll:</b> use active mode</p>
<p style='text-align: center; margin-top: 5px; margin-bottom: 0; padding: 0;'><b>Scroll when <u>Hold</u>:</b> cycle mode</p>
<p style='text-align: center; margin-top: 5px; margin-bottom: 0; padding: 0;'><b>Left Button + Right Button:</b> exit app</p>
</div>

**Scroll Modes:**
1.  **History**: Undo / Redo
2.  **Hue**: Adjust color's hue
3.  **Saturation**: Adjust color's saturation
4.  **Brightness**: Adjust color's brightness
5.  **Opacity**: Adjust color's opacity
6.  **Size**: Adjust brush/eraser/font size
7.  **Tool**: Switch between Pen, Eraser, Text, Line, Arrow, Rectangle, and Circle

**Keyboard:**
- `ESC`: **Exit Application**

## ⚙️ Configuration

CrystalBoard automatically saves your settings upon exit and reloads them the next time you start the application. This includes your last used tool, color (hue, saturation, value, opacity), and sizes (general and text).

The configuration is stored in a simple INI-style file in the standard location for your operating system:

-   **Linux**: `~/.config/CrystalBoard/CrystalBoard.conf`
-   **macOS**: `~/Library/Preferences/com.crystalboard.CrystalBoard.plist`
-   **Windows**: In the system registry under `HKEY_CURRENT_USER\Software\CrystalBoard\CrystalBoard`

You can manually edit this file to change settings, or delete it to reset the application to its default state.

## 🛠️ Building from Source

### Dependencies
- C++ Compiler (g++, clang++)
- CMake (version 3.5 or higher)
- Qt (version 6.x)

### Build Steps

1.  Clone the repository:
    ```bash
    git clone https://github.com/ProfXv/CrystalBoard.git
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