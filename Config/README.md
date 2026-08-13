# ⚙️ Mod Configuration Presets

This directory serves as a centralized backup for my personal **Windhawk** configuration files. These presets define the visual styles and functional behaviors of various system enhancements.

---

## 🚀 How to Use

### 📦 Option A: Native Full Backup Import (Windhawk 2.0+)
If you want to restore my complete Windhawk setup, use Windhawk's built-in **Import / Export Backup** feature:
1. **Open Windhawk**.
2. Click the top-right menu icon (**⋮** / **Settings**) and select **Backup / Restore** (or **Import / Export Backup**).
3. Select **Import Backup** and choose my backup file (`2026-08-13-10h11m50-windhawk-backup.json`) from this folder.
4. Confirm to restore all mod configuration presets and global settings.

### 📄 Option B: Per-Mod Manual Import (YAML)
1. **Open Windhawk** and navigate to the specific mod you want to configure.
2. In the mod's **Settings** tab, look for the **Import Settings** (or "Import from YAML/JSON") button.
3. Select the corresponding `.yaml` file from this folder.
4. **Save** the settings to apply the changes.

---

## ⚠️ Important Requirements

### 🧩 Dependency Mods
> [!IMPORTANT]
> The **Windows 11 Taskbar Styler** requires the **Taskbar Content Presenter Injector** mod to be installed and active. Without this background dependency, taskbar visual customizations will not render.

### 🔍 Process Inclusions
Some advanced styler mods require specific system processes to be included in the mod's target list. 

**Typically required processes:**
- `SearchHost.exe` (Search Menu)
- `ShellExperienceHost.exe` (Notification Center)
- `StartMenuExperienceHost.exe` (Start Menu)
- `SecHealthUI.exe` (Windows Security/Health UI)
- `TextInputHost.exe` (Touch Keyboard/Emoji Panel)
- `LockApp.exe` (Lock Screen)

Check the mod's **Target Processes** setting in Windhawk to ensure these are included.

---

## 📂 Available Configurations

### 📦 Full Backup (`2026-08-13-10h11m50-windhawk-backup.json`)
Contains complete settings for all **13 mods** in my personal Windhawk setup:

| Mod ID | Mod Name | Description |
| :--- | :--- | :--- |
| `desktop-live-overlay` | Desktop Live Overlay | Live clock and weather overlay configuration. |
| `explorer-details-better-file-sizes` | Better file sizes in Explorer details | Improved file size formatting in Explorer details pane. |
| `icon-resource-redirect` | Resource Redirect | Global resource redirection paths for custom icon themes. |
| `neko-cat` | Desktop Companions | Interactive desktop pet (Neko cat, dog, and anime characters). |
| `nilesoft-shell-animator` | Nilesoft Shell Animator | Context menu animations and transition effects. |
| `notifications-placement` | Customize Windows notifications placement | Custom notification popup location and screen placement. |
| `taskbar-content-presenter-injector` | Taskbar ContentPresenter Injector | Required background injector dependency for taskbar styling. |
| `taskbar-dock-animation` | Taskbar Dock Animation | Smooth dock animations for taskbar items. |
| `vlc-discord-rpc` | VLC Discord Rich Presence | VLC media playback status integration for Discord. |
| `windows-11-notification-center-styler` | Windows 11 Notification Center Styler | Polished Notification Center and Calendar flyouts with acrylic effects. |
| `windows-11-start-menu-styler` | Windows 11 Start Menu Styler | Translucent Start Menu with customized pips, search box, and spacing. |
| `windows-11-taskbar-styler` | Windows 11 Taskbar Styler | Comprehensive taskbar aesthetics, blur effects, and custom indicators. |
| `local@windows-11-start-menu-styler-fork` | Windows 11 Start Menu Styler - Fork | Custom fork specifically targeting `SecHealthUI.exe` styling. |

### 📄 Standalone `.yaml` Preset Files

| Preset File | Description |
| :--- | :--- |
| `windows 11 taskbar styler.yaml` | Comprehensive taskbar aesthetics, blur effects, and custom indicators. |
| `taskbar clock customisation.yaml` | Advanced clock formatting, multi-line dates, and system metrics display. |
| `windows 11 start menu styler.yaml` | Translucent Start Menu with customized pips, search box, and spacing. |
| `windows 11 notification center styler.yaml` | Polished Notification Center and Calendar flyouts with acrylic effects. |
| `vlc discord rich presence.yaml` | Pre-configured settings for VLC media status on Discord. |
| `resource redirect.yaml` | Global resource redirection paths for custom icon themes. |
| `desktop-live-overlay.yaml` | Preset configurations for desktop live clock and weather overlay. |
| `windows 11 start menu styler - Fork(secHealthUi.exe).yaml` | Experimental fork specifically targeting `SecHealthUI.exe` styling. |

---

<div align="center">

**[← Back to Main Hub](../README.md)**

</div>