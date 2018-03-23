# Multi-touch Server

## Quick Start

- Build the apk of the Multi-Touch Server with [Android Studio][android-studio] or Gradle
- Install the apk on Android OS
- Access terminal of Android OS and grant access to UInput: `$ chmod 666 /dev/uinput`
- Run the installed app
- The app will create a background service that constantly listens to 9999 UDP port for Multi-Touch events from the [QML Client](../QmlClient).

[android-studio]: https://developer.android.com/studio/index.html
