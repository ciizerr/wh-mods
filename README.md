# Neko Cat Windhawk Mod

This mod brings the classic Neko desktop pet to Windows right on your desktop! By injecting into your shell (`explorer.exe`), Neko roams your actual screen dynamically over all your windows. It provides full configuration, interactivity, and custom audio from the Windhawk Mod Settings.

## How it works

The Neko Cat operates on a dual-framework:
- **State Machine Engine (`5 FPS`)**: Internally, Neko tracks his logic exactly like the classic 90s application. He decides when to sleep, wash, run, or scratch at a calculated 5 ticks per second. 
- **Smooth Renderer (`60 FPS`)**: The mod smoothly interpolates the cat's travel path to match your screen's refresh rate using GDI+ rendering. The pixel-art frames are automatically upscaled cleanly via Nearest-Neighbor interpolation based on your preferred scale.

Because the window uses Alpha-mapping, your mouse perfectly passes right through the "transparent" portions of the cat to click your desktop icons underneath, but clicking exactly on the cat registers an interaction!

## Cat Behaviors

You can cycle through Neko's different behavior patterns by **Left-Clicking** on him! Every time you click, he will meow and shift to the next behavior in this list:

1. **Chase Mouse**: (Default) The cat lovingly chases your mouse cursor. If you stop moving the mouse, he stops, washes himself, and eventually falls asleep.
2. **Run Away**: Neko gets shy! Whenever your mouse gets too close to him, he will bolt in the opposite direction.
3. **Random**: Neko gets the zoomies and randomly picks different spots on the screen to sprint towards.
4. **Pace**: Neko continuously patrols around the extreme edges of your monitor, running corner to corner.
5. **Run Around**: Neko chases an invisible bouncing ball around the screen (very similar to a bouncing DVD logo)!

### Special Feature: Forced Sleep
If you **Click and Drag** the cat and drop him somewhere, he will immediately yawn, curl up, and enter `FORCED_SLEEP` mode. While in this mode, he ignores your mouse movements completely and will just snore peacefully. 

To wake him up from this deep slumber, simply **Right-Click** him! He will wake up and instantly resume chasing your mouse.

## Mod Settings
Open the Windhawk GUI to configure:
- **Asset Path**: The folder containing your `.png` sprites and `.wav` sounds.
- **Cat Scale**: Set to `2` for a 64x64 cat or shrink/grow him.
- **Speed**: How quickly he sprints across your screen.
- **Enable Sound / Interval**: Turn on `.wav` meows and adjust how frequently he snores while sleeping!
