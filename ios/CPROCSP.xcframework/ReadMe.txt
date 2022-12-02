[UTF-8]
[Use `iconv -f utf-8', if needed]

Если вы планируете использовать MonoDevelop, ознакомьтесь с инструкцией MonoDevelopBuild.txt


§.1. Особенности при обновлении
_______________________________

Если вы уже собирали проект и заменяете в нём CPROCSP.xcframework, перед сборкой надо обязательно
выполнить очистку в XCode: "Product / Clean build folder" (Shift+Apple+K).


§.2. Сборка примера в Xcode
___________________________

0. Проект CreateFile уже настроен, чтобы работать с ним, его надо извлечь в одну папку
   с CPROCSP.xcframework и выполнить пункт 1.

1. В папке CPROCSP.xcframework запустить 

	./SetApplicationLicense 5050X-80030-05WE5-0QQWR-PWQFL license.enc

   В текущей директории будет создан файл license.enc. 
2. Открыть проект в XCode
3. В левой панели выбрать проект, затем в основном окне в списке targets выбрать 
   цель сборки, перейти на вкладку "Build Phases", выбрать там "Link Binary With Libraries",
   нажать на плюс, в появившемся окне выбрать "Add other", добавить CPROCSP.xcframework.
4. Нажать на CPROCSP.xcframework правой кнопкой мыши, выбрать "Show In Finder".
   Перетащить из CPROCSP.xcframework/Resources все ресурсы
   кроме ru.lproj и en.lproj в проект (тоже в Resource).
!!!ВАЖНО!!!---------------------------------------------------------------------
   При переносе файлов следует установить флажок "Create Folder referencies for 
    any added folders".                                                         
--------------------------------------------------------------------------------
5. Созданный в п.1 файл license.enc перетащить в Resources проекта(аналогично п.4,
   тоже выбрав "Create Folder referencies for any added folders").
6. Файлы из Resources\en.lproj и Resources\ru.lproj перетащить в ресурсы приложения.
   В меню выбрать "Create Folder Groups".
   При этом они будут автоматически сгруппированы в двуязычные файлы локализации.
7. В свойствах проекта настроить 
   "Valid Architectures: $(ARCHS_STANDARD)"
8. Если используется XCode, в левой панели выбрать проект, затем 
   в основном окне в списке targets выбрать цель сборки, перейти на вкладку
   "Build Phases", выбрать там "Add Build Phase - Add Run Script". Появившуюся
   фазу сборки "Run Script" отредактировать, поместив в поле для скрипта
          <Путь к директории с фреймворком>/ConfigureApplication
   Если путь содержит пробелы, перед ними надо поставить backslash: "\".
9. В свойствах проекта укажите "C++ Standard Library" - "libc++ (LLVM C++ Stanard Library)".
10. Добавить линковку с модулем поддержки считывателя, в соответствии с инструкцией Readers.txt
11. В одном из файлов проекта определите переменную extern bool USE_CACHE_DIR; с помощью неё можно
    задавать директорию, где будут храниться ключи. 
    При задании (рекомендуется) bool USE_CACHE_DIR = false; эта директория будет Documents/cprocsp/keys/
    При задании bool USE_CACHE_DIR = true; это будет Library/Caches/cprocsp/keys/


§.3. Особые настройки и отладка
_______________________________

!!!ВАЖНО!!!---------------------------------------------------------------------
1. При сборке собственного проекта с КриптоПро CSP и в свойствах проекта и в свойствах target 
   рекомендуем отключать опции "Dead Code Striping", "Strip during copy",  "Strip linked products".
   Отметим, что включение "Dead Code Striping" позволяет в несколько раз уменьшить размер приложения. Если при этом
   планируется использовать модули считывателей ключей или ДСЧ то, чтобы линкер не удалил эти модули, 
   нужно добавить ссылки на необходиммые символы в приложении.
    #import <CPROCSP/CPROCSP.h>
    const void * ptr = bio_gui_rndm_get_table();
    const void * ptr = gemalto_media_get_table();
    const void * ptr = default_reader_get_table();
    const void * ptr = default_reader_get_group_table();
    const void * ptr = pcsc_reader_get_table();
    const void * ptr = pcsc_reader_get_group_table();
    const void * ptr = cryptoki_reader_get_table();
    const void * ptr = cryptoki_reader_get_group_table();
    const void * ptr = rutokenfkc_media_get_table();
    const void * ptr = jacarta_media_get_table();
    const void * ptr = jacarta_lt_get_table();
    const void * ptr = rutokennfc_media_get_table();
    const void * ptr = rutokenlite_media_get_table();
    const void * ptr = rutokenecp_media_get_table();
    const void * ptr = rutokenlitesc2_media_get_table();
    const void * ptr = rutokenecpsc_media_get_table();
    const void * ptr = rutoken_media_get_table();
    const void * ptr = rutokenpinpad_media_get_table();
    const void * ptr = rutokenfkcold_media_get_table();
    const void * ptr = rutokenecpm_media_get_table();
    const void * ptr = rutokenecpmsc_media_get_table();
    const void * ptr = cloud_reader_get_table();
    const void * ptr = cloud_media_get_table();
    const void * ptr = hvisdef_hvis_get_table();

2. Во время отладки проектов необходимо отключать контроль целостности CSP. Для этого нужно
   вызвать функцию DisableIntegrityCheck() из /Headers/DisableIntegrity.h .
--------------------------------------------------------------------------------


§.4. Настройки для проекта на swift
___________________________________

Кроме настроек из П.2 и П.3, для проектов на swift дополнительно нужно:

1. В левой панели выбрать проект, затем в основном окне в списке targets выбрать 
   цель сборки, перейти на вкладку "Build Settings". В "Linking" / "Other Linker Flags"
   указать -lc++
   Иначе будут ошибки отсутствия C++ символов в духе:
   Undefined symbol: std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::find(char, unsigned long) const

2. Аналогично в "Build Phases" / "Link Binary With Libraries" / "Add Items"
   найти и добавить libz.tbd
   Иначе будут ошибки отсутствия символовв духе:
   Undefined symbol: _inflate


§.5. Установка корневого сертификата удостоверяющего центра
___________________________________________________________

1. Создать копию корневого хранилища
   mv /var/opt/cprocsp/users/stores/root.sto{,.old}
2. Скопировать корневое хранилище, находящееся в ресурсах фреймворка
   cp /path_to_framework/CPROCSP.xcframework/Resources/root.sto /var/opt/cprocsp/users/stores/
3. Каким-то образом получить корневой сертификат УЦ
   Например, скачать с помощью curl/Safari/CPFox.
   Пример: /opt/cprocsp/bin/curl https://www.cryptopro.ru:5555/ui/certnew.asp?Type=chain -o /path_to_cert/certName --insecure
4. Установить его в корневое хранилище
   /opt/cprocsp/bin/certmgr -inst -store mRoot -file /path_to_cert/certName
5. Убедиться, что сертификат был добавлен
   /opt/cprocsp/bin/certmgr -list -store mRoot
6. Переместить корневое хранилище в ресурсы фреймворка
   mv /var/opt/cprocsp/users/stores/root.sto /path_to_framework/CPROCSP.xcframework/Resources/root.sto
7. Вернуть исходное корневое хранилище
   mv /var/opt/cprocsp/users/stores/root.sto.old /var/opt/cprocsp/users/stores/root.sto


§.6. Сохранение/восстановление контейнеров
__________________________________________

1. С помощью iTunes File Sharing найдите приложение, слинкованное с фреймворком CPROCSP
2.
   а) Сохранение: cкопируйте папку keys, лежащую внутри cprocsp, в директорию на компьютере
   б) Восстановление: замените папку keys, лежащую внутри cprocsp, на аналогичную заранее сохраненную папку.


§.7. Тестирование
_________________

Проверить корректность работы приложения на iOS можно воспользовавшись инструкцией RunTest.txt
