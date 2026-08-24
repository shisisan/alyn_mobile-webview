# Alyn SA-MP Mobile — v17.x (old source, released officially)

This is a snapshot of the **last 17.x-era source** of Alyn SA-MP Mobile (March 2025):
the classic Java launcher plus the native C++ SA-MP client (`app/src/main/cpp/Alyn_SAMPMOBILE`)
that hooks `libGTASA.so` (GTA:SA Android **2.10** only).

Released publicly and for free by the author. If you paid someone for this source, you got scammed.

## What was removed before release

- The signing keystore (`key.jks`) and its signing config — release builds are **unsigned**; set up your own signing.
- `google-services.json` (Firebase/Crashlytics config) — add your own, or remove the
  `com.google.gms.google-services` and `com.google.firebase.crashlytics` plugins from `app/build.gradle`.
- The AppLovin SDK key and ad unit id — now `YOUR_APPLOVIN_SDK_KEY` / `YOUR_AD_UNIT_ID` placeholders.

## Building

- Android Studio, JDK 17, Android SDK 35, NDK **25.1.8937393**, CMake.
- ABIs: `armeabi-v7a` + `arm64-v8a`.
- Requires the GTA: San Andreas Android **2.10** game data on the device; the client only works with that exact build of `libGTASA.so`.

## Notes

- `app/build.gradle` still says `versionName "17.0"` — the 17.1 bump was never committed; this is the final state of the 17.x line before the launcher was rewritten.
- Study it, learn from it, build something better.
