import Flutter
import UIKit

public class CryptoProFlutterPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    CryptoProFlutterApiSetup.setUp(binaryMessenger: registrar.messenger(), api: CryptoProFlutterApiImpl())
  }
}

class CryptoProFlutterApiImpl: CryptoProFlutterApi {
    // MARK: - Private variables and helper methods
    
    private let bridge = CryptoProBridge()
    private let dispatcher = DispatchQueue.global(qos: .default)
    
    private func launchResult<T>(
        completion: @escaping (Result<T, any Error>) -> Void,
        block: @escaping () throws -> Void,
    ) {
        dispatcher.async {
            do {
                try block()
            } catch let error as NSError {
                let customCode = error.userInfo["customCode"] != nil && error.userInfo["customCode"] is String ? error.userInfo["customCode"] as! String : String(error.code)
                let stacktrace = error.userInfo["stackTrace"] ?? ""
                completion(Result.failure(PigeonError(code: customCode, message: error.localizedDescription, details: stacktrace)))
            }
        }
    }
    
    private func getCertificateFromDict(dict: [AnyHashable: Any]) -> Certificate {
        return Certificate(
            alias: dict["alias"] as? String,
            owner: dict["owner"] as? String,
            issuer: dict["issuer"] as? String,
            serialNumber: dict["serialNumber"] as? String,
            algorithm: dict["algorithm"] as? String,
            version: dict["version"] as? Int64,
            oid: dict["oid"] as? String,
            validFrom: dict["validFrom"] as? String,
            validTo: dict["validTo"] as? String
        )
    }
    
    // MARK: - API
    
    func initCSP() throws -> Bool {
        return true
    }
    
    func getLicense() throws -> CryptoProLicenseInfo? {
        do {
            let dict = try bridge.getLicenseInfo()

            let validity = dict["validity"] as! Int
            let daysToExpire = dict["daysToExpire"] as? Int
            let serialNumber = dict["serialNumber"] as! String
            
            return CryptoProLicenseInfo(
                serialNumber: serialNumber,
                expiredThroughDays: daysToExpire != nil ? Int64(daysToExpire!) : nil,
                licenseInstallDate: nil,
                existingLicenseStatus: Int64(validity),
            )
        } catch let error as NSError {
            if (error.userInfo["customCode"] as? String == "get") {
                return nil
            } else {
                throw error
            }
        }
    }
    
    func setLicense(number: String) throws -> Bool {
        let dict = try bridge.setLicense(number)
        return dict["success"] as! Bool
    }
    
    func importPFXContainer(filePath: String, password: String, newPassword: String?, completion: @escaping (Result<[Certificate], any Error>) -> Void) {
        launchResult(completion: completion) {
            let certs = try self.bridge.importPFXContainer(
                filePath,
                password: password,
                newPassword: newPassword
            )
            
            let certificatesList = certs.map { elem in
                self.getCertificateFromDict(dict: elem as! [AnyHashable: Any])
            }
            
            completion(.success(certificatesList))
        }
    }
    
    func importContainerFromExternalStorage(storageName: String, password: String, newPassword: String?, completion: @escaping (Result<[Certificate], any Error>) -> Void) {
        fatalError("Method not implemented")
    }
    
    func getCertificateWithPrivateKeyFromPersonalContainer(certificateAlias: String, password: String, completion: @escaping (Result<Certificate, any Error>) -> Void) {
        fatalError("Method not implemented")
    }
    
    func deleteCertificateWithPKeyFromPersonalStorage(certificateAlias: String, completion: @escaping (Result<Void, any Error>) -> Void) {
        launchResult(completion: completion) {
            try self.bridge.deleteCertificate(fromPersonalStore: certificateAlias)
            
            completion(.success(()))
        }
    }
    
    func readContainerFromExternalStorage(storageName: String, password: String, completion: @escaping (Result<[Certificate], any Error>) -> Void) {
        fatalError("Method not implemented")
    }
    
    func addCertificatesToTrustedStorage(paths: [String], completion: @escaping (Result<Void, any Error>) -> Void) {
        launchResult(completion: completion) {
            try self.bridge.addCertificates(toTrustedStore: paths)
            
            completion(.success(()))
        }
    }
    
    func getInstalledCertificatesWithPrivateKeys(completion: @escaping (Result<[Certificate], any Error>) -> Void) {
        launchResult(completion: completion) {
            let certs = try self.bridge.getInstalledCertificatesWithPrivateKey()
            
            let certificatesList = certs.map { elem in
                self.getCertificateFromDict(dict: elem as! [AnyHashable: Any])
            }
            
            completion(.success(certificatesList))
        }
    }
    
    func signFile(filePath: String, certificateAlias: String, password: String, format: CAdESFormat, signHash: Bool, isDetached: Bool, disableOnlineValidation: Bool, tsaUrl: String?, storageName: String?, completion: @escaping (Result<SuccessSign, any Error>) -> Void) {
        launchResult(completion: completion) {
            let cert = try self.bridge.sign(
                certificateAlias,
                contentToSign: nil,
                contentToSignPath: filePath,
                detached: isDetached,
                signHash: signHash,
                disableOnlineValidation: disableOnlineValidation,
                cadesFormat: format.rawValue,
                storageName: nil,
                password: password,
                tsaUrl: tsaUrl,
            )

            completion(.success(SuccessSign(base64: cert)))
        }
    }
    
    func signMessage(message: String, certificateAlias: String, password: String, format: CAdESFormat, isDetached: Bool, signHash: Bool, disableOnlineValidation: Bool, tsaUrl: String?, storageName: String?, completion: @escaping (Result<SuccessSign, any Error>) -> Void) {
        launchResult(completion: completion) {
            let cert = try self.bridge.sign(
                certificateAlias,
                contentToSign: message,
                contentToSignPath: nil,
                detached: isDetached,
                signHash: signHash,
                disableOnlineValidation: disableOnlineValidation,
                cadesFormat: format.rawValue,
                storageName: nil,
                password: password,
                tsaUrl: tsaUrl,
            )

            completion(.success(SuccessSign(base64: cert)))
        }
    }
}
