# WH Mods Hub

<div align="center">

![License](https://img.shields.io/github/license/ciizerr/wh-mods?style=for-the-badge&color=blue)
![Repo Size](https://img.shields.io/github/repo-size/ciizerr/wh-mods?style=for-the-badge&color=green)
![Windhawk Version](https://img.shields.io/badge/Windhawk-Compatible-orange?style=for-the-badge&logo=windows)

**A collection of custom mods and configurations for Windhawk.**

[Mods](#featured-mods) • [Installation](#installation) • [Report Issue](https://github.com/ciizerr/wh-mods/issues)

</div>

---

## Overview

WH Mods Hub is a personal repository where I store my custom mods, configurations, and assets for the Windhawk platform.

## Repository Structure

```text
.
├── assets/             # Media and audio files
├── Config/             # My Windhawk backup (.json) & configuration presets
├── mods/               # Windhawk source files (.wh.cpp)
└── previews/           # Visual documentation
```

## Configuration

Mod settings and presets are managed in the [`Config/`](./Config) directory, including my complete **Windhawk Backup** (`.json`) and individual mod presets (`.yaml`). You can restore my full setup using Windhawk's native backup feature or import individual settings per mod. For details on each preset and required process inclusions, see the [Configuration README](./Config/README.md).

## Previews

<details>
<summary><b>Desktop Pets Previews</b></summary>

### 🏃 Character Options
| Neko Cat | Neko Dog | Sakura | Tomoyo |
| :---: | :---: | :---: | :---: |
| ![Neko Cat](./previews/Neko-mod/Neko-cat.gif) | ![Neko Dog](./previews/Neko-mod/neko-dog.gif) | ![Sakura](./previews/Neko-mod/sakura-icon.gif) | ![Tomoyo](./previews/Neko-mod/tomoyo-icon.gif) |

### 🎮 Mod Interaction Preview
![Neko Mod Interaction](https://raw.githubusercontent.com/ciizerr/wh-mods/2c1ecbf9ba9d0964e1a764a090cb2b7df729dc5c/previews/Neko-cat.gif)

</details>

## Installation

> [!NOTE]
> [Windhawk](https://windhawk.net/) must be installed and running.

1. Go to the [`mods/`](./mods) directory.
2. Open the source file you want to install (e.g., `neko-cat.wh.cpp`).
3. Copy the file contents.
4. In Windhawk, click the arrow next to the "Mods" tab and select **Create New Mod**.
5. Paste the code into the editor and click **Save and Compile**.

> [!NOTE]
> If a mod requires assets, it will download them from this repository automatically. An active internet connection is required during the first initialization.

## Featured Mods

<details>
<summary><b>Neko Cat (Virtual Pet)</b></summary>

A port of the classic "Neko" desktop pet, optimized for modern Windows environments via Windhawk.

- **Behaviors:** Pathfinding, scratching, and sleeping.
- **Customization:** Toggle sound, adjust speed, and customize trigger zones.
- **Performance:** Written in C++ for minimal system impact.

</details>

## Assets and Safety

- **External Assets:** Most mods fetch assets from this repository to keep the installation size small. This requires a network connection.
- **Offline Support:** To use mods offline, download the [`assets/`](./assets) folder and update the asset URLs in the source code to point to your local paths.
- **Integrity:** Assets are served directly from GitHub using HTTPS. No third-party binaries or trackers are included.

## Contact

- **Issues and Suggestions:** [GitHub Issues](https://github.com/ciizerr/wh-mods/issues)

---

<div align="center">

By [ciizerr](https://github.com/ciizerr)  
Licensed under the [MIT License](./LICENSE).

</div>
