class Certificate {
  late final String? alias;

  late final String? owner;

  late final String? issuer;

  late final String? serialNumber;

  late final String? algorithm;

  late final int? version;

  late final String? oid;

  late final String? validFrom;

  late final String? validTo;

  Certificate({
    required this.alias,
    required this.algorithm,
    required this.issuer,
    required this.oid,
    required this.owner,
    required this.serialNumber,
    required this.validFrom,
    required this.validTo,
    required this.version,
  });

  Certificate.fromMap(Map<String, dynamic> json) {
    alias = json["alias"];
    algorithm = json["algorithm"];
    issuer = json["issuer"];
    oid = json["oid"];
    owner = json["owner"];
    serialNumber = json["serialNumber"];
    validFrom = json["validFrom"];
    validTo = json["validTo"];
    version = json["version"] != null ? int.tryParse(json["version"]) : null;
  }
}
