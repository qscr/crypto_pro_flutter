#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint crypto_pro_flutter.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'crypto_pro_flutter'
  s.version          = '0.0.1'
  s.summary          = 'A new Flutter plugin project.'
  s.description      = <<-DESC
A new Flutter plugin project.
                       DESC
  s.homepage         = 'http://example.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'email@example.com' }
  s.source           = { :path => '.' }
  s.source_files = 'Classes/**/*'
  s.dependency 'Flutter'
  s.platform = :ios, '13.0'

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386' }
  s.swift_version = '5.0'
  s.library = ['z']
  s.vendored_frameworks = [
    'CPROCSP.xcframework',
    'CPROCSP.xcframework/readers/rdrpcsc_empty.xcframework',
    'CPROPKI.xcframework',
  ]
  s.preserve_paths = [
    'CPROCSP.xcframework',
  ]
  s.script_phases = [
  {
    :name => 'CryptoPro program_checksum (framework)',
    :execution_position => :after_compile,
    :script => <<-SCRIPT
      set -e
      SCRIPT_FILE="${PODS_TARGET_SRCROOT}/CPROCSP.xcframework/program_checksum.sh"
      BIN="${CONFIGURATION_BUILD_DIR}/${EXECUTABLE_PATH}"
      "${SCRIPT_FILE}" "${BIN}" framework_control_sum
    SCRIPT
  },
  {
    :name => 'Copy Resources folder to main bundle',
    :execution_position => :after_compile,
    :shell_path => '/bin/sh',
    :script => <<-SCRIPT
      set -e

      SRC="${PODS_TARGET_SRCROOT}/Resources"
      DST="${TARGET_BUILD_DIR}/${UNLOCALIZED_RESOURCES_FOLDER_PATH}"

      echo "Copying from $SRC to $DST"

      mkdir -p "$DST"
      cp -R "$SRC"/. "$DST"/

    SCRIPT
  }]
  s.pod_target_xcconfig = {
    'OTHER_LDFLAGS' => '$(inherited) -lc++',
    'ENABLE_DEBUG_DYLIB' => 'NO',
    'ENABLE_USER_SCRIPT_SANDBOXING' => 'NO',
    'CLANG_CXX_LIBRARY' => 'libc++'
  }

  # If your plugin requires a privacy manifest, for example if it uses any
  # required reason APIs, update the PrivacyInfo.xcprivacy file to describe your
  # plugin's privacy impact, and then uncomment this line. For more information,
  # see https://developer.apple.com/documentation/bundleresources/privacy_manifest_files
  # s.resource_bundles = {'crypto_pro_flutter_privacy' => ['Resources/PrivacyInfo.xcprivacy']}
end
