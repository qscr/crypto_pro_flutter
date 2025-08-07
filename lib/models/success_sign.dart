class SuccessSign {
  SuccessSign({
    required this.base64,
  });

  late final String base64;

  SuccessSign.fromMap(Map<String, dynamic> json) {
    base64 = json["signBase64"];
  }
}
