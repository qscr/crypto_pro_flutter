-dontwarn
-dontoptimize
-dontpreverify

-keep class ru.CryptoPro.**
-keepclassmembers class ru.CryptoPro.** {
    *;
}

-keep class ru.cprocsp.**
-keepclassmembers class ru.cprocsp.** {
    *;
}

-keep class org.ini4j.**
-keepclassmembers class org.ini4j.** {
    *;
}