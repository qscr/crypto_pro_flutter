import 'dart:io';

import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:crypto_pro_flutter/crypto_pro_flutter.dart';

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

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Column(
            children: [
              ElevatedButton(
                onPressed: () async {
                  FilePickerResult? result = await FilePicker.platform.pickFiles();
                  if (result?.isSinglePick ?? false) {
                    final file = File(result!.files.single.path!);
                    final ad = await _cryptoProFlutterPlugin.addPfxCertificate(file, "12345678");
                  }
                },
                child: const Text("Добавить сертификат"),
              ),
              ElevatedButton(
                onPressed: () async {
                  final list = await _cryptoProFlutterPlugin.getInstalledCertificates();
                  final ad = await _cryptoProFlutterPlugin.deleteCertificate(list.first);
                },
                child: const Text("Удалить сертификат"),
              ),
              ElevatedButton(
                onPressed: () async {
                  final list = await _cryptoProFlutterPlugin.getInstalledCertificates();
                  print(list);
                },
                child: const Text("Получить установленные сертификаты"),
              ),
              ElevatedButton(
                onPressed: () async {
                  FilePickerResult? result = await FilePicker.platform.pickFiles();
                  if (result?.isSinglePick ?? false) {
                    final list = await _cryptoProFlutterPlugin.getInstalledCertificates();
                    final file = File(result!.files.single.path!);
                    final sign = await _cryptoProFlutterPlugin.signFile(
                      file: file,
                      certificate: list.first,
                      password: "12345678",
                    );
                    print(sign);
                  }
                },
                child: const Text("Подписать файл"),
              ),
              ElevatedButton(
                onPressed: () async {
                  final list = await _cryptoProFlutterPlugin.getInstalledCertificates();
                  final sign = await _cryptoProFlutterPlugin.signMessage(
                    message: "Тест",
                    certificate: list.first,
                    password: "12345678",
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
    );
  }
}
