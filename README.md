## LVGL Smulator based on GTK4

Easy to build and modify the demo App for developing your own UI.
- Note: this branch and following instructions work for Debian based distros supporting libgtk-4, ex: Debian 12, Ubuntu 22.04, etc.
- For GTK3 use the gtk3 branch and instruction in there.
- Windows and Mac: while easily possible to port this simulator to those platforms I do not like promoting the enemies. Fill free to fork this and add support in your own repo. I will not merge it in here.

### Checkout this repo
```
git clone https://github.com/Varanda-Labs/gtk-lvgl-sim.git
```

### Install Dependencies
```
sudo apt install build-essential cmake libgtk-3-dev glade
```

To Build
```
cd gtk-lvgl-sim
./build.sh

```
### To run
```
./build/lvgl-sim
```

### To clean
```
cd gtk-lvgl-sim
./clean.sh
```

