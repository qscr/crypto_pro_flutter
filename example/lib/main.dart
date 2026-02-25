import 'dart:io';

import 'package:crypto_pro_flutter/crypto_pro_api.g.dart';
import 'package:crypto_pro_flutter/crypto_pro_flutter.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:path_provider/path_provider.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const CryptoProExampleApp());
}

class CryptoProExampleApp extends StatelessWidget {
  const CryptoProExampleApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'CryptoPro Flutter Example',
      theme: ThemeData(useMaterial3: true),
      home: const CryptoProTestScreen(),
    );
  }
}

class CryptoProTestScreen extends StatefulWidget {
  const CryptoProTestScreen({super.key});

  @override
  State<CryptoProTestScreen> createState() => _CryptoProTestScreenState();
}

class _CryptoProTestScreenState extends State<CryptoProTestScreen> {
  final _crypto = CryptoProFlutter();

  // Controllers
  final _licenseController = TextEditingController(
    text: "50502-Y0000-01D8L-5PQLK-TFGQW",
  );
  final _trustedPathsController = TextEditingController(
    text: '/path/to/ca.cer\n/path/to/root.crt',
  );

  final _pfxPathController = TextEditingController(text: '/path/to/cert.pfx');
  final _pfxPasswordController = TextEditingController(text: "123123");
  final _pfxNewPasswordController = TextEditingController();

  final _selectedAliasController = TextEditingController();

  final _signMessageController = TextEditingController(text: 'hello');
  final _signFilePathController =
      TextEditingController(text: '/path/to/data.txt');

  final _hashBase64Controller = TextEditingController(
    text: 'GUfiMWjK27v+q1eHp2IgsW2wxh91oNYrZP9COAlW+yk=',
  );
  final _hashFilePathController =
      TextEditingController(text: '/path/to/hash.txt');

  final _signPasswordController = TextEditingController(
    text: "123123",
  );

  // State
  bool _inited = false;
  CryptoProLicenseInfo? _license;
  List<Certificate> _certs = [];

  CAdESFormat _format = CAdESFormat.BES;
  bool _detached = true;
  bool _signHash = false;
  bool _disableOnlineValidation = false;
  final _tsaUrlController = TextEditingController(text: '');

  final List<String> _log = [];
  bool _busy = false;

  @override
  void dispose() {
    _licenseController.dispose();
    _trustedPathsController.dispose();
    _pfxPathController.dispose();
    _pfxPasswordController.dispose();
    _pfxNewPasswordController.dispose();
    _selectedAliasController.dispose();
    _signMessageController.dispose();
    _signFilePathController.dispose();
    _hashBase64Controller.dispose();
    _hashFilePathController.dispose();
    _signPasswordController.dispose();
    _tsaUrlController.dispose();
    super.dispose();
  }

  void _addLog(String s) {
    setState(() => _log.insert(0, '[${DateTime.now().toIso8601String()}] $s'));
  }

  Future<File> _assetToTempFile(String assetPath, {String? fileName}) async {
    final bytes = await rootBundle.load(assetPath);
    final dir = await getTemporaryDirectory();
    final out = File('${dir.path}/${fileName ?? assetPath.split('/').last}');
    await out.writeAsBytes(bytes.buffer.asUint8List(), flush: true);
    return out;
  }

  String _fmtErr(Object e) {
    if (e is PlatformException) {
      return 'PlatformException(code=${e.code}, message=${e.message}, details=${e.details}, stacktrace=${e.stacktrace})';
    }
    return e.toString();
  }

  Future<T?> _run<T>(String title, Future<T> Function() fn) async {
    if (_busy) return null;
    setState(() => _busy = true);
    _addLog('▶ $title');
    try {
      final res = await fn();
      _addLog('✅ $title: $res');
      return res;
    } catch (e) {
      _addLog('❌ $title: ${_fmtErr(e)}');
      return null;
    } finally {
      setState(() => _busy = false);
    }
  }

  Future<void> _initCsp() async {
    final ok = await _run('initCSP()', () => _crypto.initCSP());
    if (ok == true) setState(() => _inited = true);
  }

  Future<void> _getLicense() async {
    final lic = await _run('getLicense()', () => _crypto.getLicense());
    if (lic != null) setState(() => _license = lic);
  }

  Future<void> _setLicense() async {
    final num = _licenseController.text.trim();
    if (num.isEmpty) {
      _addLog('⚠️ License number is empty');
      return;
    }
    await _run('setLicense($num)', () => _crypto.setLicense(num));
    await _getLicense();
  }

  Future<void> _addTrustedCerts() async {
    final raw = _trustedPathsController.text;
    final paths = raw
        .split(RegExp(r'[\n,;]'))
        .map((e) => e.trim())
        .where((e) => e.isNotEmpty)
        .toList();

    if (paths.isEmpty) {
      _addLog('⚠️ trusted paths empty');
      return;
    }

    await _run('addCertificatesToTrustedStorage(${paths.length})',
        () => _crypto.addCertificatesToTrustedStorage(paths: paths));
  }

  Future<void> _importPfx() async {
    final pfxPath = _pfxPathController.text.trim();
    final pass = _pfxPasswordController.text;
    final newPass = _pfxNewPasswordController.text.trim().isEmpty
        ? null
        : _pfxNewPasswordController.text;

    if (pfxPath.isEmpty) {
      _addLog('⚠️ pfxPath empty');
      return;
    }
    await _run(
        'addPfxCertificate()',
        () => _crypto.importPFXContainer(
              file: File(pfxPath),
              password: pass,
              newPassword: newPass,
            ));

    await _refreshCerts();
  }

  Future<void> _refreshCerts() async {
    final list = await _run('getInstalledCertificates()',
        () => _crypto.getInstalledCertificatesWithPrivateKeys());
    if (list != null) {
      setState(() => _certs = list);
      if (_certs.isNotEmpty && (_selectedAliasController.text.trim().isEmpty)) {
        _selectedAliasController.text = _certs.first.alias ?? '';
      }
    }
  }

  Future<void> _pickFile(void Function(String filePath) onPicked) async {
    final files = await FilePicker.platform.pickFiles();
    if (files == null) return;

    if (files.xFiles.length != 1) return;

    onPicked(files.xFiles.first.path);
  }

  Future<void> _generatePathsToAssetsFiles() async {
    final certsPath = await _assetToTempFile('assets/certs.p7b');
    final certPath = await _assetToTempFile('assets/cert.cer');

    final paths = [certsPath.path, certPath.path].join('\n');
    _trustedPathsController.text = paths;
  }

  Future<void> _generatePFXAssetPath() async {
    final pfxPath = await _assetToTempFile('assets/container.pfx');

    _pfxPathController.text = pfxPath.path;
  }

  Future<void> _generateTestFileAssetPath() async {
    final samplePath = await _assetToTempFile('assets/sample.doc');

    _signFilePathController.text = samplePath.path;
  }

  Future<void> _deleteSelected() async {
    final alias = _selectedAliasController.text.trim();
    if (alias.isEmpty) {
      _addLog('⚠️ alias empty');
      return;
    }
    await _run('deleteCertificate($alias)',
        () => _crypto.deleteCertificateWithPKeyFromPersonalStorage(alias));
    await _refreshCerts();
    _selectedAliasController.clear();
  }

  Future<void> _signMessage() async {
    final alias = _selectedAliasController.text.trim();
    final pass = _signPasswordController.text;
    final msg = _signMessageController.text;

    if (alias.isEmpty) {
      _addLog('⚠️ alias empty');
      return;
    }

    final tsa = _tsaUrlController.text.trim().isEmpty
        ? null
        : _tsaUrlController.text.trim();

    final res = await _run(
      'signMessage(alias=$alias, format=$_format, detached=$_detached, signHash=$_signHash)',
      () => _crypto.signMessage(
        message: msg,
        certificateAlias: alias,
        password: pass,
        format: _format,
        isDetached: _detached,
        signHash: _signHash,
        disableOnlineValidation: _disableOnlineValidation,
        tsaUrl: tsa,
      ),
    );

    if (res != null) {
      _addLog('Signature(base64) length=${res.base64.length}');
    }
  }

  Future<void> _signFile() async {
    final alias = _selectedAliasController.text.trim();
    final pass = _signPasswordController.text;
    final path = _signFilePathController.text.trim();
    final tsa = _tsaUrlController.text.trim().isEmpty
        ? null
        : _tsaUrlController.text.trim();

    if (alias.isEmpty) {
      _addLog('⚠️ alias empty');
      return;
    }
    if (path.isEmpty) {
      _addLog('⚠️ filePath empty');
      return;
    }

    final res = await _run(
      'signFile(path=$path, alias=$alias, format=$_format, detached=$_detached, signHash=$_signHash)',
      () => _crypto.signFile(
        file: File(path),
        certificateAlias: alias,
        password: pass,
        format: _format,
        signHash: _signHash,
        isDetached: _detached,
        disableOnlineValidation: _disableOnlineValidation,
        tsaUrl: tsa,
      ),
    );

    if (res != null) {
      _addLog('Signature(base64) length=${res.base64.length}');
    }
  }

  Widget _section(String title, List<Widget> children) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(title,
                style:
                    const TextStyle(fontSize: 16, fontWeight: FontWeight.w600)),
            const SizedBox(height: 10),
            ...children,
          ],
        ),
      ),
    );
  }

  Widget _wrap(List<Widget> children) => Wrap(
        spacing: 8,
        runSpacing: 8,
        crossAxisAlignment: WrapCrossAlignment.center,
        children: children,
      );

  @override
  Widget build(BuildContext context) {
    final busy = _busy;

    return Scaffold(
      appBar: AppBar(
        title: const Text('CryptoPro Flutter — Test Console'),
        actions: [
          IconButton(
            tooltip: 'Clear log',
            onPressed: busy ? null : _refreshCerts,
            icon: const Icon(Icons.refresh),
          ),
        ],
      ),
      body: GestureDetector(
        onTap: () {
          FocusScope.of(context).unfocus();
        },
        child: SafeArea(
          child: Column(
            children: [
              // Top: Console
              Expanded(
                flex: 2,
                child: ListView(
                  padding: const EdgeInsets.all(12),
                  children: [
                    _section('1) Init', [
                      _wrap([
                        FilledButton(
                          onPressed: busy ? null : _initCsp,
                          child: const Text('initCSP'),
                        ),
                        Text(_inited ? 'inited: true' : 'inited: false'),
                      ]),
                    ]),
                    _section('2) License', [
                      _wrap([
                        FilledButton(
                          onPressed: busy ? null : _getLicense,
                          child: const Text('getLicense'),
                        ),
                        OutlinedButton(
                          onPressed: busy
                              ? null
                              : () async {
                                  await Clipboard.setData(ClipboardData(
                                      text: _license?.serialNumber ?? ''));
                                  _addLog('copied license serial to clipboard');
                                },
                          child: const Text('copy serial'),
                        ),
                      ]),
                      const SizedBox(height: 8),
                      Text(
                          'current: ${_license == null ? '—' : _license!.serialNumber}'),
                      const SizedBox(height: 8),
                      TextField(
                        controller: _licenseController,
                        decoration: const InputDecoration(
                          labelText: 'New license number',
                          border: OutlineInputBorder(),
                        ),
                      ),
                      const SizedBox(height: 8),
                      FilledButton(
                        onPressed: busy ? null : _setLicense,
                        child: const Text('setLicense'),
                      ),
                    ]),
                    _section('3) Trusted certificates (CA/ROOT)', [
                      TextField(
                        controller: _trustedPathsController,
                        minLines: 2,
                        maxLines: 6,
                        decoration: const InputDecoration(
                          labelText: 'Paths (one per line)',
                          border: OutlineInputBorder(),
                        ),
                      ),
                      const SizedBox(height: 8),
                      FilledButton(
                        onPressed: busy
                            ? null
                            : () {
                                _pickFile((filePath) {
                                  setState(() {
                                    final files = _trustedPathsController.text
                                        .split('\n');
                                    _trustedPathsController.text =
                                        [...files, filePath].join('\n');
                                  });
                                });
                              },
                        child: const Text('Pick file'),
                      ),
                      FilledButton(
                        onPressed: busy ? null : _generatePathsToAssetsFiles,
                        child: const Text('Generate paths to assets files'),
                      ),
                      FilledButton(
                        onPressed: busy ? null : _addTrustedCerts,
                        child: const Text('addCertificatesToTrustedStorage'),
                      ),
                    ]),
                    _section('4) Import PFX -> MY', [
                      TextField(
                        controller: _pfxPathController,
                        decoration: const InputDecoration(
                          labelText: 'PFX path',
                          border: OutlineInputBorder(),
                        ),
                      ),
                      const SizedBox(height: 8),
                      _wrap([
                        TextField(
                          controller: _pfxPasswordController,
                          decoration: const InputDecoration(
                            labelText: 'PFX password',
                            border: OutlineInputBorder(),
                          ),
                        ),
                        TextField(
                          controller: _pfxNewPasswordController,
                          obscureText: true,
                          decoration: const InputDecoration(
                            labelText: 'New container PIN (optional)',
                            border: OutlineInputBorder(),
                          ),
                        ),
                      ]),
                      const SizedBox(height: 8),
                      _wrap([
                        FilledButton(
                          onPressed: busy ? null : _generatePFXAssetPath,
                          child: const Text('Generate PFX asset path'),
                        ),
                        FilledButton(
                          onPressed: busy
                              ? null
                              : () {
                                  _pickFile((filePath) =>
                                      _pfxPathController.text = filePath);
                                },
                          child: const Text('Pick PFX file'),
                        ),
                        FilledButton(
                          onPressed: busy ? null : _importPfx,
                          child: const Text('addPfxCertificate'),
                        ),
                        OutlinedButton(
                          onPressed: busy ? null : _refreshCerts,
                          child: const Text('refresh certs'),
                        ),
                      ]),
                    ]),
                    _section('5) Delete certificate in MY', [
                      _wrap([
                        TextField(
                          controller: _selectedAliasController,
                          decoration: const InputDecoration(
                            labelText: 'Selected alias',
                            border: OutlineInputBorder(),
                          ),
                        ),
                        FilledButton(
                          onPressed: busy ? null : _deleteSelected,
                          child: const Text('delete'),
                        ),
                      ]),
                      const SizedBox(height: 8),
                      ListView.builder(
                        shrinkWrap: true,
                        itemCount: _certs.length,
                        itemBuilder: (context, i) {
                          final c = _certs[i];
                          final alias = c.alias ?? '';
                          return ListTile(
                            dense: true,
                            title: Text(alias),
                            subtitle: Text(
                                '${c.owner ?? ''}\n${c.issuer ?? ''}',
                                maxLines: 2),
                            isThreeLine: true,
                            onTap: () => setState(
                                () => _selectedAliasController.text = alias),
                          );
                        },
                      ),
                    ]),
                    _section('6) Sign settings', [
                      DropdownButton<CAdESFormat>(
                        value: _format,
                        items: const [
                          DropdownMenuItem(
                              value: CAdESFormat.BES, child: Text('CAdES-BES')),
                          DropdownMenuItem(
                              value: CAdESFormat.XLongType1,
                              child: Text('CAdES-X Long Type 1')),
                        ],
                        onChanged: busy
                            ? null
                            : (v) =>
                                setState(() => _format = v ?? CAdESFormat.BES),
                      ),
                      const SizedBox(height: 8),
                      _wrap([
                        FilterChip(
                          label: const Text('detached'),
                          selected: _detached,
                          onSelected: busy
                              ? null
                              : (v) => setState(() => _detached = v),
                        ),
                        FilterChip(
                          label: const Text('signHash'),
                          selected: _signHash,
                          onSelected: busy
                              ? null
                              : (v) => setState(() => _signHash = v),
                        ),
                        FilterChip(
                          label: const Text('disableOnlineValidation'),
                          selected: _disableOnlineValidation,
                          onSelected: busy
                              ? null
                              : (v) =>
                                  setState(() => _disableOnlineValidation = v),
                        ),
                      ]),
                      const SizedBox(height: 8),
                      TextField(
                        controller: _selectedAliasController,
                        decoration: const InputDecoration(
                          labelText: 'Selected alias',
                          border: OutlineInputBorder(),
                        ),
                      ),
                      const SizedBox(height: 8),
                      TextField(
                        controller: _signPasswordController,
                        decoration: const InputDecoration(
                          labelText: 'Container PIN / password for signing',
                          border: OutlineInputBorder(),
                        ),
                      ),
                      const SizedBox(height: 8),
                      TextField(
                        controller: _tsaUrlController,
                        decoration: const InputDecoration(
                          labelText: 'TSA URL (optional)',
                          border: OutlineInputBorder(),
                        ),
                      ),
                      const SizedBox(height: 8),
                      ListView.builder(
                        physics: const NeverScrollableScrollPhysics(),
                        shrinkWrap: true,
                        itemCount: _certs.length,
                        itemBuilder: (context, i) {
                          final c = _certs[i];
                          final alias = c.alias ?? '';
                          return ListTile(
                            dense: true,
                            title: Text(alias),
                            subtitle: Text(
                                '${c.owner ?? ''}\n${c.issuer ?? ''}',
                                maxLines: 2),
                            isThreeLine: true,
                            onTap: () => setState(
                                () => _selectedAliasController.text = alias),
                          );
                        },
                      ),
                    ]),
                    _section('7) Sign message', [
                      TextField(
                        controller: _signMessageController,
                        minLines: 2,
                        maxLines: 6,
                        decoration: const InputDecoration(
                          labelText: 'Message',
                          border: OutlineInputBorder(),
                        ),
                      ),
                      const SizedBox(height: 8),
                      FilledButton(
                        onPressed: busy ? null : _signMessage,
                        child: const Text('signMessage'),
                      ),
                    ]),
                    _section('8) Sign file', [
                      TextField(
                        controller: _signFilePathController,
                        decoration: const InputDecoration(
                          labelText: 'File path',
                          border: OutlineInputBorder(),
                        ),
                      ),
                      const SizedBox(height: 8),
                      FilledButton(
                        onPressed: busy
                            ? null
                            : () => _pickFile((filePath) =>
                                _signFilePathController.text = filePath),
                        child: const Text('Pick file to sign'),
                      ),
                      const SizedBox(height: 8),
                      FilledButton(
                        onPressed: busy ? null : _generateTestFileAssetPath,
                        child: const Text('Generate test file asset path'),
                      ),
                      const SizedBox(height: 8),
                      FilledButton(
                        onPressed: busy ? null : _signFile,
                        child: const Text('signFile'),
                      ),
                    ]),
                    const SizedBox(height: 24),
                  ],
                ),
              ),

              // Bottom: log
              Expanded(
                flex: 1,
                child: Container(
                  decoration: BoxDecoration(
                    border: Border(
                        left:
                            BorderSide(color: Theme.of(context).dividerColor)),
                  ),
                  child: Column(
                    children: [
                      const Padding(
                        padding: EdgeInsets.all(12.0),
                        child: Row(
                          children: [
                            Icon(Icons.terminal),
                            SizedBox(width: 8),
                            Text('Log'),
                          ],
                        ),
                      ),
                      const Divider(height: 1),
                      Expanded(
                        child: ListView.builder(
                          reverse: false,
                          padding: const EdgeInsets.all(12),
                          itemCount: _log.length,
                          itemBuilder: (context, i) => SelectableText(
                            _log[i],
                            style: const TextStyle(fontSize: 12),
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
