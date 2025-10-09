# Big File Cleaner

## PURPOSE
“Big File Cleaner” is a Qt 6.4+ desktop application that finds large files in a chosen directory tree and lets you delete them in bulk. Enter a minimum size (MB/GB/TB, decimals allowed), pick a search root, and review the results sorted from largest to smallest.

## DESCRIPTION
The application provides two inputs: **Minimum File Size** and **Search Directory** for the root folder to scan. You can type a path directly or click **Browse**… to open a native folder chooser. Press **Search** to recursively scan the directory and all subdirectories; matches appear in the **Found** table with two columns — **File** and **Size** (human-readable). Select one or more rows and click **Delete Selected**; a confirmation dialog lists all files to be removed, and any failures are reported after the operation.

Built with C++20 and Qt Widgets, BigFileList keeps the UI responsive by scanning on a worker `QThread` using `std::filesystem::recursive_directory_iterator` (permission errors are skipped). The result is a fast, focused tool for identifying and cleaning up oversized files across local disks and mounted volumes.

## REQUIRED FILES
Due to the way the Qt works, the files must be in a very specific directory structure.
### On Windows:
Make sure you have the Visual C++ Redistributable installed.  You can download it from the Microsoft website:
[Microsoft Visual C++ Redistributable packages for Visual Studio 2015–2022](https://aka.ms/vs/17/release/vc_redist.x64.exe)

Directory structure:
* `.\icons\BigFileCleaner.ico`
* `.\icons\BigFileCleaner.png`
* `.\imageformats\qico.dll`
* `.\platforms\qwindows.dll`
* `.\styles\qmodernwindowsstyle.dll`
* `.\BigFileCleaner.exe`
* `.\Qt6Core.dll`
* `.\Qt6Gui.dll`
* `.\Qt6Widgets.dll`

### On Linux:
On Linux, I am using the distro's Qt6 libraries.  Here are the commands to install them:
* Fedora / RHEL / Rocky / Alma (x86_64, aarch64):<br>
  `sudo dnf install -y qt6-qtbase-gui xorg-x11-server-Xwayland libxkbcommon-x11 libX11 libXext libXrender xcb-util-cursor`
* Debian / LinuxMint / Ubuntu (amd64, arm64, etc.)<br>
  `sudo apt-get install -y libqt6widgets6 xwayland libxcb1 libxcb-render0 libxcb-shape0 libxcb-xfixes0 libx11-6 libxext6 libxrender1 fontconfig libxkbcommon-x11-0 libxcb-cursor0`
* openSUSE<br>
  `sudo zypper install -y libqt6-qtbase-platformtheme libqt6-qtbase-gui xwayland xcb-util-cursor`

Directory structure:
* `./icons/BigFileCleaner.ico`
* `./icons/BigFileCleaner.png`
* `./BigFileCleaner`