#!/bin/sh
APK=build/android-build/build/outputs/apk/debug/android-build-debug.apk
PKG=com.example.androidapp

any=false
for arg in "$@"; do
    case "$arg" in
        i|install) any=true
            echo "Installing built apk..."
            adb install -r $APK
            ;;
        u|uninstall) any=true
            echo "Installing built apk..."
            adb uninstall $PKG
            ;;
        r|run) any=true
            adb shell monkey -p $PKG -c android.intent.category.LAUNCHER 1
            ;;
        -h|--help|-help) ;;
        *) echo "Unknown argument: $arg"; exit 1 ;;
    esac
done

if ! $any; then
    echo "Usage: $0 i[nstall]|u[ninstall]|r[un]"
    echo "Can chain commands e.g. \`$0 i r\`"
fi
