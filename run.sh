#!/bin/sh
APK=build/android-build/build/outputs/apk/debug/android-build-debug.apk
PKG=com.example.androidapp

any=false
for arg in "$@"; do
    case "$arg" in
        i|install) any=true
            echo "Installing built apk..."
            adb install -r $APK
            sleep 1 ;;
        u|uninstall) any=true
            echo "Uninstalling application..."
            adb uninstall $PKG
            sleep 1 ;;
        r|run) any=true
            echo "Running application..."
            adb shell monkey -p $PKG -c android.intent.category.LAUNCHER 1
            sleep 1 ;;
        l|log) any=true
            PID=$(adb shell pidof $PKG | tr -d '\r')
            if [ -z $PID ]; then
                echo "PID not found - app may not be running"; exit 1
            fi
            adb logcat --pid=$PID
            ;;
        -h|--help|-help) ;;
        *) echo "Unknown argument: $arg"; exit 1 ;;
    esac
done

if ! $any; then
    echo "Usage: $0 i[nstall]|u[ninstall]|r[un]"
    echo "Can chain commands e.g. \`$0 i r\`"
fi
