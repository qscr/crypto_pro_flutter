import 'package:crypto_pro_flutter/models/certificate.dart';

class SuccessSign {
  SuccessSign({
    required this.base64,
    required this.chain,
  });

  late final String base64;

  late final List<Certificate> chain;

  SuccessSign.fromMap(Map<String, dynamic> json) {
    base64 = json["signBase64"];
    chain = (json["chain"] as List<dynamic>)
        .map((item) => Certificate.fromMap(item))
        .toList();
  }
}
