# Grapefruit
An *intrinsically* motivating *non-AI* digital friend for organisation and help with procrastination

## Building
There are a bunch of requirements needed in order to build, but there's a `shell.nix` file for those using Nix!

Run `make debug` for a debug version or `make release` for a final version. `make clean` cleans old build files.

The building should result in an apk file being created. For quick testing, ensure USB debugging is turned on and the phone is connected and use the `run.sh` file! (executing `./run.sh --help` will list options)

A convenient command that does everything is
```
make debug && ./run.sh i r l
```
