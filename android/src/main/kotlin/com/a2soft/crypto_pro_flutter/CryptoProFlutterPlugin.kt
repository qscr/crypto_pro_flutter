package com.a2soft.crypto_pro_flutter

import CAdESFormat
import Certificate
import CryptoProFlutterApi
import CryptoProLicenseInfo
import SuccessSign
import android.content.Context
import io.flutter.embedding.engine.plugins.FlutterPlugin
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import ru.CryptoPro.AdES.AdESConfig
import ru.CryptoPro.AdES.Options
import ru.CryptoPro.CAdES.CAdESSignature
import ru.CryptoPro.CAdES.CAdESType
import ru.CryptoPro.CAdES.exception.CAdESException
import ru.CryptoPro.JCP.JCP
import ru.CryptoPro.JCP.KeyStore.InvalidPasswordException
import ru.CryptoPro.JCP.KeyStore.JCPPrivateKeyEntry
import ru.CryptoPro.JCP.params.JCPProtectionParameter
import ru.CryptoPro.JCP.tools.Encoder
import ru.CryptoPro.JCSP.CSPConfig
import ru.CryptoPro.JCSP.CSPConfigBase
import ru.CryptoPro.JCSP.JCSP
import ru.CryptoPro.JCSP.exception.WrongPasswordException
import ru.CryptoPro.JCSP.support.BKSTrustStore
import ru.CryptoPro.reprov.RevCheck
import ru.cprocsp.ACSP.tools.common.HexString
import ru.cprocsp.ACSP.tools.license.CSPLicenseConstants
import java.io.BufferedInputStream
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.InputStream
import java.math.BigInteger
import java.security.KeyStore
import java.security.KeyStore.ProtectionParameter
import java.security.Security
import java.security.cert.CertificateFactory
import java.security.cert.X509Certificate

/** CryptoProFlutterPlugin */
class CryptoProFlutterPlugin: FlutterPlugin, CryptoProFlutterApi {
  private var context: Context? = null

  private val scope = CoroutineScope(Dispatchers.Default)

  override fun onAttachedToEngine(flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
    CryptoProFlutterApi.setUp(flutterPluginBinding.binaryMessenger, this)
    context = flutterPluginBinding.applicationContext
  }

  override fun onDetachedFromEngine(binding: FlutterPlugin.FlutterPluginBinding) {}

  override fun initCSP(): Boolean {
    if (CSPConfig.isInitiated()) return true;

    val initCode = CSPConfig.init(context)
    val initOk = initCode == CSPConfig.CSP_INIT_OK
    if (initOk) {
      if (Security.getProvider(JCSP.PROVIDER_NAME) == null) {
        Security.addProvider(JCSP())
      }
      if (Security.getProvider(JCP.PROVIDER_NAME) == null) {
        Security.addProvider(JCP())
      }
      if (Security.getProvider(RevCheck.PROVIDER_NAME) == null) {
        Security.addProvider(RevCheck())
      }
      //
      // Отключаем проверку цепочки штампа времени (CAdES-T),
      // чтобы не требовать него CRL.
      //
      System.setProperty("ru.CryptoPro.CAdES.validate_tsp", "false")

      // Таймауты для CRL на всякий случай.
      System.setProperty("com.sun.security.crl.timeout", "60")
      System.setProperty("ru.CryptoPro.crl.read_timeout", "60")


      // Задание провайдера по умолчанию для CAdES.
      AdESConfig.setDefaultProvider(JCSP.PROVIDER_NAME)

      // Включаем возможность онлайновой проверки статуса
      // сертификата.
      //
      // В случае создания подписей формата BES или T можно отключить
      // проверку цепочки сертификатов подписанта (и службы) с помощью
      // параметра:
      // cAdESSignature.setOptions((new Options()).disableCertificateValidation()); // CAdES
      // или
      // xAdESSignature.setOptions((new Options()).disableCertificateValidation()); // XAdES
      // перед добавлением подписанта.
      // По умолчанию проверка цепочки сертификатов подписанта всегда
      // включена.
      System.setProperty("ru.CryptoPro.reprov.enableCRLDP", "true");
      System.setProperty("com.sun.security.enableCRLDP", "true")
      System.setProperty("com.ibm.security.enableCRLDP", "true")
      System.setProperty("com.sun.security.enableAIAcaIssuers", "true");
      System.setProperty("ru.CryptoPro.reprov.enableAIAcaIssuers", "true");
    }

    return initOk
  }

  override fun getLicense(): CryptoProLicenseInfo? {
    val providerInfo = CSPConfigBase.getCSPProviderInfo()
    val license = providerInfo.license
    return CryptoProLicenseInfo(
      license.serialNumber, license.expiredThroughDays.toLong(),
      license.licenseInstallDate?.toString(), license.existingLicenseStatus.toLong()
    )
  }

  override fun setLicense(number: String): Boolean {
    val providerInfo = CSPConfigBase.getCSPProviderInfo()
    val license = providerInfo.license
    return license.checkAndSave(number, true) == CSPLicenseConstants.LICENSE_STATUS_OK
  }

  override fun importPFXContainer(
    filePath: String,
    password: String,
    newPassword: String?,
    callback: (Result<List<Certificate>>) -> Unit
  ) {
    scope.launchResult(callback) {
      val keyStore = KeyStore.getInstance(JCSP.PFX_STORE_NAME, JCSP.PROVIDER_NAME)
      val fileInputStream: InputStream = FileInputStream(filePath)
      keyStore.load(null, null)
      try {
        keyStore.load(fileInputStream, password.toCharArray())
      } catch (_: WrongPasswordException) {
        throw CustomWrongPasswordException();
      }
      val certificate = addContainerFromStorage(
        storage = keyStore,
        password = password,
        newPassword = newPassword
      )
      callback(Result.success(certificate))
    }
  }

  override fun importContainerFromExternalStorage(
    storageName: String,
    password: String,
    newPassword: String?,
    callback: (Result<List<Certificate>>) -> Unit
  ) {
    scope.launchResult(callback) {
      val keyStore = KeyStore.getInstance(storageName, JCSP.PROVIDER_NAME)
      keyStore.load(null, null)
      val certificate = addContainerFromStorage(
        storage = keyStore,
        password = password,
        newPassword = newPassword
      )
      callback(Result.success(certificate))
    }
  }

  override fun getCertificateWithPrivateKeyFromPersonalContainer(
    certificateAlias: String,
    password: String,
    callback: (Result<Certificate>) -> Unit
  ) {
    scope.launchResult(callback) {
      val keyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
      keyStore.load(null, null)
      try {
        val protectionParam = JCPProtectionParameter(password.toCharArray())
        val entry = keyStore.getEntry(certificateAlias, protectionParam) as JCPPrivateKeyEntry
        val certificate = entry.certificate as X509Certificate

        callback(Result.success(certificate.toModel(certificateAlias)))
      } catch (_: WrongPasswordException) {
        throw CustomWrongPasswordException()
      }
    }
  }

  override fun deleteCertificateWithPKeyFromPersonalStorage(
    certificateAlias: String,
    callback: (Result<Unit>) -> Unit
  ) {
    scope.launchResult(callback) {
      val keyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
      keyStore.load(null, null)
      keyStore.deleteEntry(certificateAlias)

      callback(Result.success(Unit))
    }
  }

  override fun readContainerFromExternalStorage(
    storageName: String,
    password: String,
    callback: (Result<List<Certificate>>) -> Unit
  ) {
    scope.launchResult(callback) {
      val keyStore = KeyStore.getInstance(storageName, JCSP.PROVIDER_NAME)
      keyStore.load(null, null)
      val certificates = readContainerFromStorage(storage = keyStore, password = password)
      callback(Result.success(certificates))
    }
  }

  override fun addCertificatesToTrustedStorage(
    paths: List<String>,
    callback: (Result<Unit>) -> Unit
  ) {
    scope.launchResult(callback) {
      val errorCertificates = mutableListOf<String>()
      if (paths.isEmpty()) return@launchResult

      val certificateFactory = CertificateFactory.getInstance("X.509")

      for (path in paths) {
        val file = File(path)
        if (!file.exists()) {
          errorCertificates.add(path)
          continue
        }

        try {
          FileInputStream(file).use { inputStream ->
            if (path.endsWith(".p7b", ignoreCase = true) || path.endsWith(
                ".p7c",
                ignoreCase = true
              )
            ) {
              // Обработка p7b: возможно несколько сертификатов
              val certificates = certificateFactory.generateCertificates(inputStream)
              if (certificates.isEmpty()) {
                errorCertificates.add(path)
              } else {
                val certificatesToAdd = certificates
                  .filter { entry -> entry is X509Certificate }
                  .map { entry -> entry as X509Certificate }
                  .toList()
                addToBksTrustStore(certificatesToAdd)
              }
            } else {
              // Обработка одиночного сертификата (например .cer, .crt, .der)
              val cert = certificateFactory.generateCertificate(inputStream)
              if (cert is X509Certificate) {
                addToBksTrustStore(List(1) { cert })
              } else {
                errorCertificates.add(path)
              }
            }
          }
        } catch (_: Exception) {
          errorCertificates.add(path)
        }
      }

      CAdESSignature.reloadCACerts()

      if (errorCertificates.isNotEmpty()) {
        throw SomeCertificatesAreNotAddedToTrustStoreException(errorCertificates.toTypedArray())
      } else {
        callback(Result.success(Unit))
      }
    }
  }

  override fun getInstalledCertificatesWithPrivateKeys(callback: (Result<List<Certificate>>) -> Unit) {
    scope.launchResult(callback) {
      val keyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
      keyStore.load(null, null)
      val aliases = keyStore.aliases().toList().filter {  keyStore.isKeyEntry(it) }

      val certificates =
        aliases.map { alias -> (keyStore.getCertificate(alias) as X509Certificate).toModel(alias) }
          .toList()
      callback(Result.success(certificates))
    }
  }

  override fun signFile(
    filePath: String,
    certificateAlias: String,
    password: String,
    format: CAdESFormat,
    signHash: Boolean,
    isDetached: Boolean,
    disableOnlineValidation: Boolean,
    tsaUrl: String?,
    storageName: String?,
    callback: (Result<SuccessSign>) -> Unit
  ) {
    scope.launchResult(callback) {
      sign(
        callback = callback,
        contentToSign = null,
        contentToSignPath = filePath,
        alias = certificateAlias,
        password = password,
        detached = isDetached,
        signHash = signHash,
        disableOnlineValidation = disableOnlineValidation,
        tsaUrl = tsaUrl,
        storageName = storageName,
        format = format,
        rootPaths = null,
      )
    }
  }

  override fun signMessage(
    message: String,
    certificateAlias: String,
    password: String,
    format: CAdESFormat,
    isDetached: Boolean,
    signHash: Boolean,
    disableOnlineValidation: Boolean,
    tsaUrl: String?,
    storageName: String?,
    callback: (Result<SuccessSign>) -> Unit
  ) {
    scope.launchResult(callback) {
      sign(
        callback = callback,
        contentToSign = message.toByteArray(),
        contentToSignPath = null,
        alias = certificateAlias,
        password = password,
        detached = isDetached,
        signHash = signHash,
        disableOnlineValidation = disableOnlineValidation,
        tsaUrl = tsaUrl,
        storageName = storageName,
        format = format,
        rootPaths = null,
      )
    }
  }

  private fun sign(
    callback: (Result<SuccessSign>) -> Unit,
    contentToSign: ByteArray?,
    contentToSignPath: String?,
    alias: String,
    password: String,
    detached: Boolean,
    signHash: Boolean,
    disableOnlineValidation: Boolean,
    format: CAdESFormat,
    storageName: String? = null,
    tsaUrl: String? = null,
    rootPaths: List<String>? = null
  ) {
    var keyStore: KeyStore?
    try {
      keyStore = KeyStore.getInstance(storageName ?: JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
      keyStore.load(null, null)
    } catch (e: Exception) {
      throw GetCertificateFromContainerException(storageName ?: JCSP.HD_STORE_NAME, e.toString())
    }

    var privateKeyEntry: JCPPrivateKeyEntry?
    try {
      val protectionParam = JCPProtectionParameter(password.toCharArray())
      privateKeyEntry =
        keyStore!!.getEntry(alias, protectionParam as ProtectionParameter) as JCPPrivateKeyEntry
    } catch (_: InvalidPasswordException) {
      throw CustomWrongPasswordException();
    } catch (e: Exception) {
      throw GetCertificatePrivateKeyException(e.toString())
    }
    val certificateChain = privateKeyEntry.certificateChain
    val privateKey = privateKeyEntry.privateKey

    val chain: MutableList<X509Certificate> = ArrayList()
    for (cert in certificateChain) {
      chain.add(cert as X509Certificate)
    }

    if (rootPaths != null) {
      val certificateFactory = CertificateFactory.getInstance("X.509")
      for (certPath in rootPaths) {
        val file = File(certPath)
        val certificateInputStream = FileInputStream(file)
        val cert = certificateFactory.generateCertificate(certificateInputStream)
        if (cert is X509Certificate) {
          chain.add(cert)
        }
        certificateInputStream.close()
      }
    }

    val cAdESSignature = CAdESSignature(detached, signHash)
    if (disableOnlineValidation) {
      cAdESSignature.setOptions((Options()).disableCertificateValidation());
    }

    val tsaSeverUrl = if (format == CAdESFormat.XLONG_TYPE1 && tsaUrl != null) tsaUrl else null
    val cadesType = when (format) {
      CAdESFormat.BES -> CAdESType.CAdES_BES
      CAdESFormat.XLONG_TYPE1 -> CAdESType.CAdES_X_Long_Type_1
    }

    try {
      cAdESSignature.addSigner(
        JCSP.PROVIDER_NAME, null, null, privateKey, chain,
        cadesType, tsaSeverUrl, false, null, null, null, true,
      )
    } catch (e: CAdESException) {
      if (e.errorCode == 44) {
        throw AddSignerCertificateStatusUnknownOrRevokedException(e)
      } else {
        throw AddSignerUnknownException(e)
      }
    }
    try {
      val signatureStream = ByteArrayOutputStream()

      cAdESSignature.open(signatureStream)

      if (contentToSignPath != null) {
        BufferedInputStream(FileInputStream(contentToSignPath), 64 * 1000).use { input ->
          val buf = ByteArray(64 * 1000)

          while (true) {
            val read = input.read(buf)
            if (read < 0) break

            cAdESSignature.update(buf, 0, read)
          }
        }
      }

      if (contentToSign != null) {
        cAdESSignature.update(contentToSign)
      }

      cAdESSignature.close()
      signatureStream.close()

      val enc = Encoder()
      val base64 = enc.encode(signatureStream.toByteArray())
      callback(Result.success(SuccessSign(base64)))
    } catch (e: Exception) {
      throw ReadSignatureFromStreamException(e.toString())
    }
  }

  private fun readContainerFromStorage(storage: KeyStore, password: String): List<Certificate> {
    val certificateWithPrivateKeys: MutableList<Certificate> = ArrayList()
    val aliases = storage.aliases()
    while (aliases.hasMoreElements()) {
      val alias = aliases.nextElement()
      if (storage.isKeyEntry(alias)) {
        val protectionParam = JCPProtectionParameter(password.toCharArray())
        val entry = storage.getEntry(alias, protectionParam) as JCPPrivateKeyEntry
        val certificate = entry.certificate as X509Certificate
        val certificateModel = certificate.toModel(alias)
        certificateWithPrivateKeys.add(certificateModel)
      }
    }

    return certificateWithPrivateKeys
  }

  private fun addContainerFromStorage(
    storage: KeyStore,
    password: String,
    newPassword: String?
  ): List<Certificate> {
    try {
      val certificatesWithoutPrivateKey: MutableList<X509Certificate> = ArrayList()
      val certificatesWithPrivateKey: MutableList<Certificate> = ArrayList()

      val hdKeyStore = KeyStore.getInstance(JCSP.HD_STORE_NAME, JCSP.PROVIDER_NAME)
      hdKeyStore.load(null, null)

      val aliases = storage.aliases()
      while (aliases.hasMoreElements()) {
        val alias = aliases.nextElement()
        if (storage.isKeyEntry(alias)) {
          val protectionParam = JCPProtectionParameter(password.toCharArray())
          val certificateWithPrivateKey = storage.getEntry(alias, protectionParam) as JCPPrivateKeyEntry

          val certificate = certificateWithPrivateKey.certificate as X509Certificate
          val anotherCertificatesInChain = certificateWithPrivateKey.certificateChain
            .filterNot { it is X509Certificate && certificate != it }
            .map { it as X509Certificate }
          certificatesWithoutPrivateKey.addAll(anotherCertificatesInChain)
          val newAlias = certificate.serialNumber.toHexString()

          val certificateModel = certificate.toModel(newAlias)
          certificatesWithPrivateKey.add(certificateModel)

          if (!hdKeyStore.containsAlias(newAlias)) {
            val newProtectionParam = JCPProtectionParameter((newPassword ?: password).toCharArray())
            hdKeyStore.setEntry(newAlias, certificateWithPrivateKey, newProtectionParam)
          }
        } else {
          val certificate = storage.getCertificate(alias) as X509Certificate
          certificatesWithoutPrivateKey.add(certificate)
        }
      }

      if (certificatesWithoutPrivateKey.isNotEmpty()) {
        addToBksTrustStore(certificatesWithoutPrivateKey)
        CAdESSignature.reloadCACerts()
      }

      return certificatesWithPrivateKey
    } catch (_: WrongPasswordException) {
      throw CustomWrongPasswordException()
    }
  }

  private fun addToBksTrustStore(certificates: List<X509Certificate>) {
    val trustStorePath = CSPConfig.getBksTrustStore() + File.separator +
            BKSTrustStore.STORAGE_FILE_TRUST

    val trustStoreFile = File(trustStorePath)
    val trustStoreStream = trustStoreFile.inputStream();
    val keyStore = KeyStore.getInstance(BKSTrustStore.STORAGE_TYPE)
    keyStore.load(
      trustStoreStream,
      BKSTrustStore.STORAGE_PASSWORD,
    )
    trustStoreStream.close()
    for (certificate in certificates) {
      val needAdd = (keyStore.getCertificateAlias(certificate) == null)
      if (needAdd) {
        val alias = certificate.serialNumber.toHexString()
        keyStore.setCertificateEntry(alias, certificate)
      }
    }
    val updatedTrustStore = FileOutputStream(trustStoreFile)
    keyStore.store(
      updatedTrustStore,
      BKSTrustStore.STORAGE_PASSWORD,
    )
    updatedTrustStore.close()
  }

  private fun X509Certificate.toModel(alias: String): Certificate =
    Certificate(
      alias = alias,
      owner = subjectDN.name,
      issuer = issuerDN.name,
      serialNumber = serialNumber.toString(),
      algorithm = sigAlgName,
      version = version.toLong(),
      oid = sigAlgOID,
      validFrom = notBefore.toString(),
      validTo = notAfter.toString(),
    )

  private fun BigInteger.toHexString(): String =
    HexString.toHex(
      toByteArray(),
      true
    )

  private fun <T> CoroutineScope.launchResult(
    callback: (Result<T>) -> Unit,
    block: suspend () -> Unit
  ) = launch {
    try {
        block()
    } catch (e: Exception) {
      callback(Result.failure(e))
    }
  }
}
