# Kobo Tweaks

A beginner-friendly addon for Kobo eReaders.

## Features
- Customize various UI elements and features of Kobo eReaders (similar to [kobopatch](https://github.com/pgaskin/kobopatch))
- Add Battery and Clock widgets while reading a book (similar to [NickelClock](https://github.com/shermp/NickelClock))
- More to come...

## Installation

> [!IMPORTANT]
> This addon conflicts with NickelClock, please [uninstall](https://github.com/shermp/NickelClock?tab=readme-ov-file#uninstall-nickelclock) it first

Follow these steps to install Kobo Tweaks:  
  1. Connect your Kobo eReader to your computer
  2. Download the latest [KoboRoot.tgz](https://github.com/redphx/kobo-tweaks/releases/latest) file and place it inside the hidden `.kobo` folder on your Kobo eReader
  > If you're using macOS and don't see the `.kobo` folder in Finder, press the combination `Cmd + Shift + .`
  3. Eject the device safely

After it installs and reboots, open a book and you'll see new Clock and Battery widgets on the header. To customize Kobo Tweaks, check the **Customization** section below.

## Customization

Settings can be customized by editing the `.adds/tweaks/settings.ini` file.

### [Reading]
> Applies after reopening the book

| Setting | Description | Values |
|-|-|-|
| `BookmarkImage` | Custom bookmark image file name, including extension, located in the `.adds/tweaks/images`.<br>If an additional image exists with the same base name and the `_dark` suffix, that image is used when Dark mode is active.| String |
| `HeaderFooterHeightScale` | Percentage-based scaling factor applied to the original header and footer height.<br>For example, a value of 66 means the header and footer are rendered at 66 percent of their original height. | Unit: %<br>Range: 50-100<br>Default: 100|

#### Bookmark image

- There is a bookmark template at [`resources/bongo_cat.svg`](https://github.com/redphx/kobo-tweaks/blob/main/resources/bongo_cat.svg)
- Bookmark image must be in PNG format with a transparent background.
- There are no restrictions on the dimension, but it's recommended to use dimensions similar to the original. Below are the dimensions of Kobo's default bookmark image.

> [!NOTE]
> This info needs to be verified

| Device/Dimension | 57x54 | 64x61 | 102x97 | 116x110 | 133x126 |
|-|:-:|:-:|:-:|:-:|:-:|
| Mini<br>Touch 2.0 | x |
| Aura<br>Aura Edition 2<br>Glo Nia | | x |
| Aura H2O<br>Aura HD<br>Clara 2E<br>Clara BW<br>Clara Colour<br>Clara HD<br>Glo HD | | | x |
| Libra 2<br>Libra Colour<br>Libra H2O | | | | x |
| Aura One<br>Elipsa<br>Elipsa 2E<br>Forma<br>Sage | | | | | x |


## [Reading.Widget]
> Applies after reopening the book

| Setting | Description | Values |
|-|-|-|
| `HeaderLeft`<br>`HeaderRight`<br>`FooterLeft`<br>`FooterRight` | Defines where the widget is shown and which widget it is | `Battery`, `Clock`<br>or leave it blank to show nothing |
| `BatteryStyle`<br>`BatteryStyleCharging` | Specifies the battery style for normal and charging states | `IconLevel`, `LevelIcon`, `Icon`, `Level` |
| `BatteryShowWhenBelow` | Shows the battery widget only when the battery level is less than or equal to this value | Unit: %<br>Range: 10-100<br>Default: 100 (always visible) |
| `Clock24hFormat` | Enables or disables 24-hour time format | `true`, `false`, `on`, `off` |

- The Battery widget updates only when you turn a page or unlock the device.

- The Clock widget updates when you turn a page, when the device is unlocked, and every two minutes after the last update.
