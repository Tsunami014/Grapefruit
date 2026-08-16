# Grapefruit
An *intrinsically* motivating *non-ML* digital friend for organisation and help with procrastination

## Building
There are a bunch of requirements needed in order to build, but there's a `shell.nix` file for those using Nix!

Run `make debug` for a debug version or `make release` for a final version. `make clean` cleans old build files.

The building should result in an apk file being created. For quick testing, ensure USB debugging is turned on and the phone is connected and use the `run.sh` file! (executing `./run.sh --help` will list options)

A convenient command that does everything is
```
make debug && ./run.sh i r l
```

### Debugging
Including `-DENABLE_DEBUG=ON` when building (or running `make debug`) will include the following debug features:
- Allows dragging objects in the main scene
- Press `p` in the main scene to debug log all the positions of objects in the main scene
- Changes the bottom task text to state the task's score
- Sorts tasks in task list instead by task end score
- Add a redo button to conversations
- Logs the location of the data on the drive whenever it loads it

### The app icon
`assets/icon.svg` is the app icon, but NOT DIRECTLY the apk app icon. For that, the file needs to be converted to an android xml file and then placed in `android/res/drawable/icon.xml`. This can be done via e.g. [a website](https://svg2vector.com/).
