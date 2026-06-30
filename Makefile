.PHONY: all debug release clean setup-qt apk install

QT_VERSION = 6.10.1
QT_ANDROID_DIR = $(CURDIR)/.qt/$(QT_VERSION)/android_arm64_v8a
QT_HOST_DIR = $(CURDIR)/.qt/$(QT_VERSION)/gcc_64
SYSROOT = $(ANDROID_SDK_ROOT)/ndk-bundle/toolchains/llvm/prebuilt/linux-x86_64/sysroot

ANDRIOD_FLAGS = \
	-GNinja \
	-DCMAKE_SYSTEM_NAME=Android \
	-DANDROID_ABI="arm64-v8a" \
	-DANDROID_PLATFORM=android-33 \
	-DANDROID_NDK_ROOT="$(ANDROID_SDK_ROOT)/ndk-bundle" \
	-DQT_HOST_PATH="$(QT_HOST_DIR)" \
	-DEGL_INCLUDE_DIR="$(SYSROOT)/usr/include" \
	-DGLESv2_INCLUDE_DIR="$(SYSROOT)/usr/include"

DESKTOP_FLAGS = \
	-GNinja \
	-DCMAKE_EXE_LINKER_FLAGS="-Wl,--allow-shlib-undefined" \
	-DCMAKE_SHARED_LINKER_FLAGS="-Wl,--allow-shlib-undefined"


setup-qt:
	@if [ ! -d "$(QT_HOST_DIR)" ]; then \
		echo "Installing linux Qt stuff for Qt v$(QT_VERSION)..."; \
		aqt install-qt linux desktop $(QT_VERSION) linux_gcc_64 --outputdir .qt; \
	fi
	@if [ ! -d "$(QT_ANDROID_DIR)" ]; then \
		echo "Installing android Qt stuff for Qt v$(QT_VERSION)..."; \
		aqt install-qt linux android $(QT_VERSION) android_arm64_v8a --outputdir .qt; \
	fi

debug: setup-qt
	$(QT_ANDROID_DIR)/bin/qt-cmake -B build -S . $(ANDRIOD_FLAGS) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
	cmake --build build --target apk --parallel
all: release
release: setup-qt
	$(QT_ANDROID_DIR)/bin/qt-cmake -B release -S . $(ANDRIOD_FLAGS) -DCMAKE_BUILD_TYPE=Release
	cmake --build release --target apk --parallel

# Use regular cmake
desktop: setup-qt
	$(QT_HOST_DIR)/bin/qt-cmake -B build-desktop -S . $(DESKTOP_FLAGS) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
	cmake --build build-desktop --parallel

clean:
	rm -rf ./build ./build-desktop ./release
