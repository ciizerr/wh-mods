# ⚙️ Mod Configuration Presets

This directory serves as a centralized backup for my personal **Windhawk** configuration files. These presets define the visual styles and functional behaviors of various system enhancements.

---

## 🚀 How to Use

### ⚡ Option A: Automated Deployment (Recommended)
You can apply selection-based or bulk configurations instantly using the automated installer. Open an **Administrator** PowerShell and run:

```powershell
iwr -useb https://raw.githubusercontent.com/ciizerr/wh-mods/main/install-config.ps1 | iex
```

### 📦 Option B: Manual Import
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

| Configuration File | Description |
| :--- | :--- |
| `windows 11 taskbar styler.yaml` | Comprehensive taskbar aesthetics, blur effects, and custom indicators. |
| `taskbar clock customisation.yaml` | Advanced clock formatting, multi-line dates, and system metrics display. |
| `windows 11 start menu styler.yaml` | Translucent Start Menu with customized pips, search box, and spacing. |
| `windows 11 notification center styler.yaml` | Polished Notification Center and Calendar flyouts with acrylic effects. |
| `vlc discord rich presence.yaml` | Pre-configured settings for VLC media status on Discord. |
| `resource redirect.yaml` | Global resource redirection paths for custom icon themes. |
| `windows 11 start menu styler - Fork(secHealthUi.exe).yaml` | Experimental fork specifically targeting `SecHealthUI.exe` styling. |

---

<div align="center">

**[← Back to Main Hub](../README.md)**

</div>