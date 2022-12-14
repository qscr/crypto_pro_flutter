# Плагин для формирования электронной подписи формата CADES-BES с помощью нативных SDK Crypto Pro

## __Описание__
Плагин принимает сертификаты в формате __PKCS12__ ```.pfx```

Приватный ключ должен быть помечен как экспортируемый

Пока Android Only


## __Установка__
### __Подключение плагина к Android проекту__
1. Скопировать ```.aar``` библиотеки из ```android/libs``` плагина к себе в проект в ```android\app\libs```

2. Добавить в ```build.gradle```
```gradle
minSdkVersion 24

buildTypes {
        release {
            shrinkResources false
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
        }
    }

packagingOptions {
    jniLibs {
        useLegacyPackaging = true
    }
}

dependencies {
    implementation fileTree(dir: 'libs', include: '*.aar')
}
```

3. Создать файл ```proguard-rules.pro``` в ```android/app```
```pro
-keep public class ru.CryptoPro.*
```

Библиотеки .aar указаны в плагине как compile-only, так как невозможно к .aar (коим является этот плагин) подключать другие .aar, для этого требуется скопировать их к себе в проект и подключить как implementation. Proguard используется, чтобы запретить обфускацию кода, которая происходить при выполнении релизной сборки.

## __Использование__

* Инициализировать провайдер
    ```dart
    CryptoProFlutter.initCSP()
    ```
* Добавить Pfx-сертификат в хранилище
    ```dart
    CryptoProFlutter.addPfxCertificate(File file, String password)
    ```
* Получить список сертификатов, добавленных пользователем
    ```dart
    CryptoProFlutter.getInstalledCertificates()
    ```
* Удалить добавленный сертификат
    ```dart
    CryptoProFlutter.deleteCertificate(Certificate certificate)
    ```
* Подписать файл
    ```dart
    CryptoProFlutter.signFile(
        required File file,
        required Certificate certificate,
        required String password,
        bool isDetached = true,
        bool disableOnlineValidation = false,
    )
    ```
* Подписать сообщение
    ```dart
    CryptoProFlutter.signMessage({
        required String message,
        required Certificate certificate,
        required String password,
        bool isDetached = true,
        bool signHash = false,
        bool signHash = false,
        bool disableOnlineValidation = false,
    })
    ```

## Todo
 - [x] Поддержка iOS