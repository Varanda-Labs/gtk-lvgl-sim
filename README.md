## LVGL Smulator based on GTK4

Easy to build and modify the demo App for developing your own UI.

- Windows and Mac: while easily portable to these platforms I do not want to promote spywares.

### Checkout this repo
```
git clone https://github.com/Varanda-Labs/gtk-lvgl-sim.git
```

### Install Dependencies
```
sudo apt install build-essential cmake libgtk-3-dev glade
```

### To Build your custom App
```
cd gtk-lvgl-sim
./clean.sh
./mk.sh

```
Subsequent rebuilds can skip the clean.sh command

### To Build your LVGL Music Demo
```
cd gtk-lvgl-sim
./clean.sh
./mk_demo_music.sh

```
Subsequent rebuilds can skip the clean.sh command

### To run
```
./run.sh
```

### To clean
```
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

### License
MIT, read [LICENSE](LICENSE) file

