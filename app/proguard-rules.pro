-keep class com.nvidia.devtech.* { *; }
-keep class com.rockstargames.gtasa.* { *; }
-keep class com.wardrumstudios.utils.* { *; }

-keep class ro.alynsampmobile.game.* { *; }
-keep class ro.alynsampmobile.game.ui.* { *; }
-keep class ro.alynsampmobile.game.ui.widgets.* { *; }
-keep class ro.alynsampmobile.game.ui.widgets.adapter.* { *; }

-keep class ro.alynsampmobile.launcher.utils.SignatureChecker { *; }

# for minify
-dontwarn javax.servlet.**
-dontwarn org.conscrypt.**
-dontwarn org.bouncycastle.**
-dontwarn org.openjsse.**