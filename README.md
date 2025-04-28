# Плагин для формирования электронной подписи формата CADES-BES с помощью нативных SDK Crypto Pro

## **Описание**

Плагин принимает ключевые контейнеры в формате **PKCS12** `.pfx`. Есть возможность интеграции с внешними ключевыми контейнерами (например, Рутокен)

Приватный ключ должен быть помечен как экспортируемый

Пока Android Only

## **Установка**

### **Подключение плагина к Android проекту**

1. Скопировать `.aar` библиотеки из `android/libs` плагина к себе в проект в `android\app\libs`

2. Добавить в `build.gradle`

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
        keepDebugSymbols += ['*/arm64-v8a/*.so', '*/armeabi-v7a/*.so', '*/x86_64/*.so', '*/x86/*.so']
    }
}

dependencies {
    implementation fileTree(dir: 'libs', include: '*.aar')
}
```

3. Создать файл `proguard-rules.pro` в `android/app`

```pro
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
```

Библиотеки .aar указаны в плагине как compile-only, так как невозможно к .aar (коим является этот плагин) подключать другие .aar, для этого требуется скопировать их к себе в проект и подключить как implementation. Proguard используется, чтобы запретить обфускацию кода, которая происходить при выполнении релизной сборки. Подробнее можно прочитать [тут](https://docs.cryptopro.ru/android/samples/ACSPClientApp/obfuscation_and_code_optimization)

## **Использование**

- Инициализировать провайдер
  ```dart
  CryptoProFlutter.initCSP()
  ```
- Добавить Pfx-контейнер в хранилище
  ```dart
  CryptoProFlutter.addPfxCertificate(File file, String password, String newPassword)
  ```
- Получить список сертификатов, добавленных пользователем
  ```dart
  CryptoProFlutter.getInstalledCertificates()
  ```
- Удалить добавленный сертификат
  ```dart
  CryptoProFlutter.deleteCertificate(Certificate certificate)
  ```
- Подписать файл
  ```dart
  CryptoProFlutter.signFile(
      required File file,
      required Certificate certificate,
      required String password,
      required CAdESFormat format,
      bool isDetached = true,
      bool disableOnlineValidation = false,
      String? tsaUrl,
  )
  ```
- Подписать сообщение
  ```dart
  CryptoProFlutter.signMessage({
      required String message,
      required Certificate certificate,
      required String password,
      required CAdESFormat format,
      bool isDetached = true,
      bool signHash = false,
      bool disableOnlineValidation = false,
      String? tsaUrl,
  })
  ```
- Добавить внешний контейнер в хранилище
  ```dart
  CryptoProFlutter.addContainerFromExternalStorage({
      required String storageName,
      required String password,
      String? newPassword,
  })
  ```
- Добавить сертификаты в хранилище доверенных приложения
  ```dart
  CryptoProFlutter.addCertificatesToTrustedStorage({
      required List<String> paths,
  })
  ```

## Todo

- [x] Поддержка iOS
