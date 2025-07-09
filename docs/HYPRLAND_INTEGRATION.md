# Hyprland Integration Guide for CrystalBoard

This guide provides a structured approach to integrating CrystalBoard seamlessly into your Hyprland desktop environment. The goal is to make CrystalBoard behave like a native feature of your desktop—a quick, accessible, and clean annotation layer.

This guide is for users with a basic understanding of Hyprland's configuration files.

## The Three Pillars of Integration

We will approach this in three distinct steps: setting up a keybinding, defining window rules, and implementing optional dynamic styling for a truly polished experience.

---

### Part 1: The Toggle Shortcut

**Principle:** The most crucial step is to have a keyboard shortcut that can instantly launch or close CrystalBoard. We can achieve this with a simple shell command that checks if the process is running and acts accordingly.

The command logic is: `pkill CrystalBoard || CrystalBoard`
- `pkill CrystalBoard`: Attempts to terminate the CrystalBoard process. If it succeeds (i.e., the process existed), the command chain stops.
- `|| CrystalBoard`: If the `pkill` command fails (i.e., the process did not exist), this part of the command is executed, launching the application.

**Example:**
You can bind this command to any key combination you prefer in your `hyprland.conf` or a related configuration file.

```ini
# This is an example binding. Change the key combination to your preference.
bind = SHIFT_ALT_CTRL, L, exec, pkill CrystalBoard || CrystalBoard
```

---

### Part 2: Window Rules for a Canvas-like Feel

**Principle:** By default, Hyprland will tile CrystalBoard like any other application. To make it function as an overlay canvas, we need to tell Hyprland to treat it differently. This is done via `windowrule`.

Key rules for CrystalBoard include:
- `float`: Allows the window to float above the tiled layout.
- `size <width> <height>`: Forces the window to a specific size, ideally your monitor's resolution.
- `noborder` (Optional): Removes the window border.
- `noshadow` (Optional): Removes the window shadow.

For a complete list of rules, refer to the **[Official Hyprland Window Rules Documentation](https://wiki.hyprland.org/Configuring/Window-Rules/)**.

**Example:**
Add the following lines to your `hyprland.conf`.

```ini
# Make CrystalBoard float and set its size.
# IMPORTANT: Change 1920 1080 to your monitor's resolution.
windowrule = float, class:^(CrystalBoard)$
windowrule = size 1920 1080, class:^(CrystalBoard)$

# Optional: Uncomment the lines below to remove the border and shadow for a cleaner look.
# windowrule = noborder, class:^(CrystalBoard)$
# windowrule = noshadow, class:^(CrystalBoard)$
```

---

### Part 3: Advanced Dynamic Styling

**Principle:** For the ultimate integration, you might want to temporarily disable desktop effects like blur and rounded corners when CrystalBoard is active. This creates a distraction-free, pure canvas. This advanced technique requires a script that listens to Hyprland's `activewindow` event and uses the `hyprctl` command to change settings on the fly.

The core of this method is the `hyprctl keyword` command, which can dynamically alter most settings you'd normally put in your config file.

- To learn about listening to events, see the **[Official Hyprland IPC Documentation](https://wiki.hyprland.org/IPC/)**.
- To see what settings can be changed, consult the **[Official Hyprland Keywords Documentation](https://wiki.hyprland.org/Configuring/Keywords/)**.

**Example:**
The following is a conceptual example within a shell script that is assumed to be triggered by the `activewindow` event. Your implementation may vary.

```sh
#!/bin/sh

# This is a conceptual script. Your actual implementation will depend on your setup.

# Get the class of the currently active window
active_class=$(hyprctl activewindow -j | jq -r ".class")

if [ "$active_class" = "CrystalBoard" ]; then
    # When CrystalBoard is active, disable decorations
    hyprctl keyword decoration:rounding 0
    hyprctl keyword decoration:blur:enabled false
else
    # When another window is active, restore default decorations
    hyprctl keyword decoration:rounding 10
    hyprctl keyword decoration:blur:enabled true
fi
```
This example demonstrates how to check the active window and use `hyprctl` to toggle settings, providing a highly integrated and professional feel.