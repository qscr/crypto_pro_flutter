import 'dart:async';
import 'dart:developer';
import 'dart:io';

import 'package:crypto_pro_flutter/crypto_pro_flutter.dart';
import 'package:crypto_pro_flutter/crypto_pro_flutter_platform_interface.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:path_provider/path_provider.dart';
import 'package:permission_handler/permission_handler.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  bool _initCSPResult = false;
  String? _errorMessage;
  final _cryptoProFlutterPlugin = CryptoProFlutter();

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    final result = await Permission.bluetooth.request();
    final result2 = await Permission.bluetoothConnect.request();
    bool? initCSPResult;
    String? errorMessage;
    // Platform messages may fail, so we use a try/catch PlatformException.
    // We also handle the message potentially returning null.
    try {
      initCSPResult = await _cryptoProFlutterPlugin.initCSP();
    } on PlatformException {
      errorMessage = 'Failed to init providers';
    }

    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      if (initCSPResult != null) {
        _initCSPResult = initCSPResult;
      } else {
        _errorMessage = errorMessage;
      }
    });
  }

  Future<String> getAssetFilePath(String assetName) async {
    Directory directory = await getTemporaryDirectory();
    final tempPath = "${directory.path}/$assetName";
    ByteData data = await rootBundle.load("assets/$assetName");
    List<int> bytes =
        data.buffer.asUint8List(data.offsetInBytes, data.lengthInBytes);
    await File(tempPath).writeAsBytes(bytes);
    return tempPath;
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Padding(
            padding: const EdgeInsets.symmetric(horizontal: 16.0),
            child: Column(
              children: [
                ElevatedButton(
                  onPressed: () async {
                    FilePickerResult? result =
                        await FilePicker.platform.pickFiles();
                    if (result?.isSinglePick ?? false) {
                      final file = File(result!.files.single.path!);
                      await _cryptoProFlutterPlugin.addPfxCertificate(
                        file: file,
                        password: "11111111",
                      );
                    }
                  },
                  child: const Text(
                    "Добавить PFX-сертификат",
                    textAlign: TextAlign.center,
                  ),
                ),
                ElevatedButton(
                  onPressed: () async {
                    await _cryptoProFlutterPlugin
                        .addContainerFromExternalStorage(
                      storageName: 'Aktiv Rutoken ECP BT 1',
                      password: "11111111",
                    );
                  },
                  child: const Text(
                    "Добавить сертификат из внешнего хранилища",
                    textAlign: TextAlign.center,
                  ),
                ),
                ElevatedButton(
                  onPressed: () async {
                    // final cert = await getAssetFilePath('root_unep_2024.crt');
                    // await _cryptoProFlutterPlugin
                    //     .addCertificatesToTrustedStorage(paths: [cert]);
                  },
                  child: const Text(
                    "Добавить корневые сертификаты во хранилище доверенных",
                    textAlign: TextAlign.center,
                  ),
                ),
                ElevatedButton(
                  onPressed: () async {
                    final certs = await _cryptoProFlutterPlugin
                        .readContainerFromExternalStorage(
                      storageName: 'Aktiv Rutoken ECP 1',
                      password: '11111111',
                    );
                    print(certs);
                  },
                  child: const Text(
                    "Получить сертификаты из внешнего хранилища",
                    textAlign: TextAlign.center,
                  ),
                ),
                ElevatedButton(
                  onPressed: () async {
                    final list = await _cryptoProFlutterPlugin
                        .getInstalledCertificates();
                    await _cryptoProFlutterPlugin.deleteCertificate(list.first);
                  },
                  child: const Text("Удалить сертификат"),
                ),
                ElevatedButton(
                  onPressed: () async {
                    final list = await _cryptoProFlutterPlugin
                        .getInstalledCertificates();
                    print(list);
                  },
                  child: const Text("Получить установленные сертификаты"),
                ),
                ElevatedButton(
                  onPressed: () async {
                    FilePickerResult? result =
                        await FilePicker.platform.pickFiles();
                    if (result?.isSinglePick ?? false) {
                      final list = await _cryptoProFlutterPlugin
                          .getInstalledCertificates();
                      final file = File(result!.files.single.path!);
                      final sign = await _cryptoProFlutterPlugin.signFile(
                        file: file,
                        certificate: list.first,
                        password: "11111111",
                        format: CAdESFormat.BES,
                      );
                      log(sign);
                      Clipboard.setData(ClipboardData(text: sign));
                    }
                  },
                  child: const Text("Подписать файл"),
                ),
                ElevatedButton(
                  onPressed: () async {
                    final list = await _cryptoProFlutterPlugin
                        .getInstalledCertificates();
                    final sign = await _cryptoProFlutterPlugin.signMessage(
                      message: "Тест",
                      isDetached: false,
                      certificate: list.first,
                      password: "11111111",
                      format: CAdESFormat.BES,
                    );
                    print(sign);
                  },
                  child: const Text("Подписать сообщение"),
                ),
                _errorMessage != null
                    ? Text(_errorMessage!)
                    : Text(
                        _initCSPResult.toString(),
                      ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
