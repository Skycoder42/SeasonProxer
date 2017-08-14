#!/bin/bash
set -e

if [[ $PLATFORM == "android_"* ]]; then
	mv install build-$PLATFORM/android-build
	/opt/qt/$QT_VER/$PLATFORM/bin/androiddeployqt --input "build-$PLATFORM/Mobile/android-libAniRem.so-deployment-settings.json" --output "build-$PLATFORM/android-build" --deployment bundled --gradle --no-gdbserver #--release --sign <path> <alias> --storepass <pass>
fi