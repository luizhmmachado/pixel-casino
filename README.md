## (PT-BR)
Recria os jogos do cassino (https://github.com/bdwat/casino) com interface gráfica Qt (qml)

## (EN)
Recreate the casino minigames (https://github.com/bdwat/casino) with the Qt GUI (qml)

## Android APK install note

If Android shows "App not installed", the most common causes are:

1. Signature mismatch with a previously installed APK of the same package.
2. Device ABI incompatibility (current CI artifact is `arm64-v8a`).

Quick checks:

1. Uninstall the previous app and install again.
2. Ensure you are using the APK that matches your device ABI.

Release now publishes two Android APKs:

1. `pixel-casino-android-arm64-v8a.apk` (64-bit ARM)
2. `pixel-casino-android-armeabi-v7a.apk` (32-bit ARM)

To allow in-place updates between CI releases, configure these GitHub Secrets so the workflow signs APKs with a stable key:

1. `ANDROID_KEYSTORE_B64` (base64 of `.jks`/`.keystore` file)
2. `ANDROID_KEY_ALIAS`
3. `ANDROID_KEYSTORE_PASSWORD`
4. `ANDROID_KEY_PASSWORD`
