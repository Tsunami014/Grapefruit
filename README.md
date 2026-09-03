# Grapefruit
> Currently builds for: Only Android (and will probably stay that way).

Grapefruit is a virtual friend who can try help you do your work! They have *non-ML based* conversations with you (made with a **LOT** of handcrafted sentences by me <3) and try to *intrinsically* motivate you to complete tasks, basically by reassuring you, suggesting different things based on your emotions (such as taking a break or doing a different task) and affirming that even starting work is ok, you don't have to finish if you don't want to (and many other different things similar in vibe). And never by pressuring you to do anything.
Grapefruit will be chilling in their house all day, stop by whenever you don't feel like doing anything or when you don't know what to do! There is zero pressure, Grapefruit won't judge you or berate you for not getting stuff done. You can even have overdue tasks and it won't change anything.

Tasks contain an importance rating (out of 5), a list of reasons why you want to do the task (with a guide on how to make them intrinsic, boiling down to "not why you want to do the task but why doing it is fun/enjoyable for you"), a list of qualities from a preset bank (e.g. "tiring") which also are used when recommending you tasks (e.g. it is less likely to recommend you a tiring task when you said you were tired) and a list of 'items', basically sub things that need doing for that task. Each item can have a *rough* estimated time and/or a due date.
Tasks are sorted into categories for convenience too.
Tasks can also be starred, which makes it more likely to be recommended and also allows you to see all the 'starred tasks' separately for more organisation.
It uses a task recommendation system which uses complex maths to determine which tasks are best to do now based on many qualities of the task, your emotions and the conversation so far.

It also contains an *extremely efficient* text editor for mobile, containing buttons in a bar at the bottom (directly above the keyboard) for e.g. moving whole lines of text around and inserting new lines. It also hides other elements when selecting one to make it easier to see while editing.

**Also please note it autosaves constantly, don't worry about losing your work**.

## Installing
- Go to the [releases page](https://github.com/Tsunami014/Grapefruit/releases) and download an apk file of the latest (or any, but the latest is always best) release onto your phone.
- Go to the file manager and press on the app.
    - If it says "the file manager doesn't have permission to install this app" then you need to press the button in that popup to open the settings and change the file manager to be able to install apps and try again.
    - If it says "this app has not been verified", do not worry you can still install it by pressing "don't scan" (which may be under "more options"). You can scan it if you want but I promise I didn't put any viruses in this so it's kinda a waste of time.
- The app should be installed! Yay! Now go enjoy it!

## Developing
### Building
There are a bunch of requirements needed in order to build, but there's a `shell.nix` file for those using Nix!

Run `make debug` for a debug desktop version or `make release` for a final mobile version. `make clean` deletes all build files.

Building for release should result in an apk file being created. For quick testing, ensure USB debugging is turned on and the phone is connected and use the `run.sh` file! (executing `./run.sh --help` will list options)

A convenient command that builds, installs, runs and displays logs for the mobile app is:
```
make debug && ./run.sh i r l
```

### Debugging
Including `-DENABLE_DEBUG=ON` when building (automatically included when running `make debug`), will include the following debug features in the build:
- Allows dragging objects in the main scene
- Press `p` in the main scene to debug log all the positions of objects in the main scene
- Changes the task texts to state the task's score
- Sorts tasks in task list instead by task score
- Add a redo button to conversations to rerun with the same purpose and context variables
- Logs the location of the data on the drive whenever it loads

### The app icon
`assets/icon.svg` is the app icon, but NOT DIRECTLY the apk app icon. For that, the file needs to be converted to an android xml file and then placed in `android/res/drawable/icon.xml`. This can be done via e.g. [a website](https://svg2vector.com/).
