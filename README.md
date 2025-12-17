# Kobo Tweaks

A beginner-friendly addon for customizing Kobo eReaders.

## 🔥 1. Features
- Customize various UI elements and features of Kobo eReaders (similar to [KoboPatch](https://github.com/pgaskin/kobopatch))
  - Reduces header and footer's heights
- Add Battery and Clock widgets while reading a book (similar to [NickelClock](https://github.com/shermp/NickelClock))
- Customize bookmark image
- More to come...

#### Difference from KoboPatch:
  - Beginner-friendly, "patches" can be customized without reinstalling firmware (just edit and reboot)
  - No need to wait for patch updates when a new firmware is released (most of the time)
  - Downside: not all patches can be implemented, and it requires more work to add a new tweak

#### Difference from NickekClock:  
  - Uses its own widgets for clock & battery instead of Kobo's built-in ones (reduce the likelihood of encountering unwanted bugs)
  - Clock & battery only update after a page turn
  - Supports Dark mode
  - Downside: it conflicts with NickelClock. You can [uninstall NickelClock](https://github.com/shermp/NickelClock#uninstall-nickelclock) manually, or let Kobo Tweaks handle the uninstallation automatically.

<table>
  <tbody>
    <tr>
      <td>
        <img height="600" alt="image" src="https://github.com/user-attachments/assets/99f571d7-607b-486b-9136-3f2b5cbf887e" />
      </td>
      <td>
        Settings used<br><br>
        <pre>
[Reading]
BookmarkImage=bongo_cat.png
HeaderFooterHeightScale=60
HeaderFooterMargins=50
&nbsp;
[Reading.Widget]
BatteryShowWhenBelow=100
BatteryStyle=Icon
BatteryStyleCharging=LevelIcon
Clock24hFormat=true
FooterLeft=
FooterRight=Battery
HeaderLeft=Clock
HeaderRight=
        </pre>
      </td>
    </tr>
  </tbody>
</table>

## 🛠️ 2. Installation

Kobo Tweaks should be compatible with any Kobo eReader running a recent 4.x firmware.

Follow these steps to install Kobo Tweaks:  
  1. Connect your Kobo eReader to your computer
  2. Download the latest [KoboRoot.tgz](https://github.com/redphx/kobo-tweaks/releases/latest) file and place it inside the hidden `.kobo` folder on your Kobo eReader
  > If you're using macOS and don't see the `.kobo` folder in Finder, press the combination `Cmd + Shift + .`

  <pre>
.kobo/
├─ KoboRoot.tgz
</pre>
  3. Eject the device safely to avoid data corruption

After it installs and reboots, open a book and you'll see new Clock and Battery widgets on the header. To customize Kobo Tweaks, check the **Customization** section below.

> [!IMPORTANT]
> <table><tr><td><img height="300" alt="image" src="https://github.com/user-attachments/assets/03b362c1-06c0-43bf-9c20-1629a74ef990" /></td><td>Since Kobo Tweaks cannot be used with NickelClock, it automatically detects and uninstalls NickelClock. If you see this dialog while reading a book, restart the device once again before continuing.</td></tr></table>

There are a few new files and folders in your Kobo eRreader:
<pre>
.adds/
├─ tweaks/
│  ├─ images/
│  ├─ DELETE_TO_UNINSTALL.txt
│  ├─ settings.ini
</pre>

### Uninstallation
To uninstall Kobo Tweaks, delete the `DELETE_TO_UNINSTALL.txt` file and reboot the device. The file also contains the currently installed Kobo Tweaks version. Please include that information when reporting a bug.

## 💃 3. Customization

Settings can be customized by editing the `.adds/tweaks/settings.ini` file.

### [Reading]
> Applies after reopening the book

| Setting | Description | Values |
|-|-|-|
| `BookmarkImage` | Custom bookmark image file name, including extension, located in the `.adds/tweaks/images`| String |
| `HeaderFooterHeightScale` | Percentage-based scaling factor applied to the original header and footer height.<br>For example, a value of 66 means the header and footer are rendered at 66 percent of their original height. | <b>Unit:</b> %<br><b>Range:</b> 50-100<br><b>Default:</b> 100 |
| `HeaderFooterMargins` | Sets the left and right margins for both header and footer | <b>Range:</b> 0-100<br><b>Default:</b> 50 |

#### Bookmark image

- There is a bookmark template at [`resources/bongo_cat.svg`](https://github.com/redphx/kobo-tweaks/blob/main/resources/bongo_cat.svg)
- Bookmark image must be in PNG format with a transparent background
- If an additional image exists with the same base name and the `_dark` suffix, that image is used when Dark mode is active. For example, if `BookmarkImage` is set to `bongo_cat.png`, then `bongo_cat_dark.png` will be used in Dark mode, if it exists.
- There are no restrictions on the dimension, but it's recommended to use dimensions similar to the original. Below are the dimensions of Kobo's default bookmark image.

> [!NOTE]
> This info needs to be verified

| Device/Dimensions | 57x54 | 64x61 | 102x97 | 116x110 | 133x126 |
|-|:-:|:-:|:-:|:-:|:-:|
| Mini<br>Touch 2.0 | x |
| Aura, Aura Edition 2<br>Glo Nia | | x |
| Aura H2O, Aura HD<br>Clara 2E, Clara BW, Clara Colour, Clara HD<br>Glo HD | | | x |
| Libra 2, Libra Colour, Libra H2O | | | | x |
| Aura One<br>Elipsa, Elipsa 2E<br>Forma<br>Sage | | | | | x |


### [Reading.Widget]
> Applies after reopening the book

| Setting | Description | Values |
|-|-|-|
| `HeaderLeft`<br>`HeaderRight`<br>`FooterLeft`<br>`FooterRight` | Defines where the widget is shown and which widget it has | `Battery`, `Clock`<br>or leave it blank to show nothing |
| `BatteryStyle`<br>`BatteryStyleCharging` | Specifies the battery style for normal and charging states | `IconLevel`, `LevelIcon`, `Icon`, `Level` |
| `BatteryShowWhenBelow` | Shows the battery widget only when the battery level is less than or equal to this value | <b>Unit:</b> %<br><b>Range:</b> 10-100<br><b>Default:</b> 100 (always visible) |
| `Clock24hFormat` | Enables or disables 24-hour time format | `true`, `false`, `on`, `off` |

- All widgets support Dark mode
- The Battery widget updates only when you turn a page or unlock the device.
- The Clock widget updates when you turn a page, when the device is unlocked, and every two minutes after the last update.
- ⚠️ If you place a widget in the `HeaderRight` position, you may see a visual glitch when opening a book if the first page is already bookmarked. Turn the page forward and back to fix it.

## 🤝 4. Acknowledgements

- Thanks to [**@pgaskin**](https://github.com/pgaskin) and [**@shermp**](https://github.com/shermp) for reviewing and improving the code
- Thanks to the [shermp/NickelClock](https://github.com/shermp/NickelClock) project for giving me the idea of how to add widgets to the Reading view.
- And thank you for using!
