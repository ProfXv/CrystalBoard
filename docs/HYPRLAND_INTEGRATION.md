# Hyprland Integration Guide for CrystalBoard

This guide provides tips for integrating CrystalBoard seamlessly into your Hyprland desktop environment, making it feel like a native feature.

---

### Tip 1: Create a Toggle Shortcut (Recommended)

The most convenient way to use CrystalBoard is to have a single keyboard shortcut that can instantly launch or close it. This can be achieved with a simple shell command in your `hyprland.conf`.

The command logic is: `pkill CrystalBoard || CrystalBoard`
- `pkill CrystalBoard`: Attempts to terminate the CrystalBoard process. If it succeeds (the process existed), the command stops.
- `|| CrystalBoard`: If `pkill` fails (the process did not exist), this part executes, launching the application.

**Example `hyprland.conf` binding:**
```ini
# This is an example. Change SHIFT_ALT_CTRL, L to your preferred key combination.
bind = SHIFT_ALT_CTRL, L, exec, pkill CrystalBoard || CrystalBoard
```

---

### Tip 2: Advanced Dynamic Styling (Optional)

For the ultimate integrated experience, you can make your desktop effects (like blur and rounded corners) automatically disable when CrystalBoard is active, creating a distraction-free canvas.

This advanced technique requires a script that listens to Hyprland's `activewindow` event and uses the `hyprctl` command to change settings on the fly.

- To learn about listening to events, see the **[Official Hyprland IPC Documentation](https://wiki.hyprland.org/IPC/)**.
- To see what settings can be changed, consult the **[Official Hyprland Keywords Documentation](https://wiki.hyprland.org/Configuring/Keywords/)**.

**Conceptual Example Script:**
The following demonstrates how to check the active window and use `hyprctl` to toggle settings. Your actual implementation may vary.

```sh
#!/bin/sh
# This is a conceptual script.

active_class=$(hyprctl activewindow -j | jq -r ".class")

if [ "$active_class" = "CrystalBoard" ]; then
    # When CrystalBoard is active, disable decorations
    hyprctl keyword decoration:rounding 0
    hyprctl keyword decoration:blur:enabled false
else
    # When another window is active, restore your default decorations
    hyprctl keyword decoration:rounding 10
    hyprctl keyword decoration:blur:enabled true
fi
```

**Note:** As of the latest version, CrystalBoard handles its own window sizing and floating behavior directly within the code. You **no longer need** to set `windowrule` for `float` or `size` in your `hyprland.conf` for this application.
