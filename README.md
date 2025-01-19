## LVGL Smulator based on GTK4

Easy to build and modify the demo App for developing your own UI.

- Windows and Mac: while easily portable to these platforms I refuse to promote spywares. Fill free to fork this repo and support those in your own. I will not merge ports for those in here.

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

### To debug (vscode or codium)
```
cd gtk-lvgl-sim
mkdir .vscode
cp launch.json .vscode/
```
- if using codium (free of MS spyware) use the first configuration
Native-Debug extension:  https://github.com/WebFreak001/code-debug 
- if using MS spyware use the second configuration
extension C/C++ from Microsoft

