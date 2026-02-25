[UTF-8]
[Use `iconv -f utf-8', if needed]

Если вы планируете использовать MonoDevelop, ознакомьтесь с инструкцией MonoDevelopBuild.txt


§.1. Особенности при обновлении
_______________________________

Если вы уже собирали проект и заменяете в нём CPROCSP.xcframework, перед сборкой надо обязательно
выполнить очистку в Xcode: "Product / Clean build folder" (Shift+Apple+K).

Для Xcode 15 и новее требуется подпись фреймворка. В силу ограничений со стороны Apple,
для работы с подписью можно переподписать фреймворк под своей учетной записью или использовать его без подписи:

	codesign --timestamp -s <identity> CPROCSP.xcframework

§.2. Сборка примера в Xcode
___________________________

0. Проект CreateFile уже настроен, чтобы работать с ним, его надо извлечь в одну папку
   с CPROCSP.xcframework и выполнить пункт 1.

1. В папке проекта запустить 

    ./CPROCSP.xcframework/SetApplicationLicense 5050X-80030-05WE5-0QQWR-PWQFL license.enc

   В текущей директории будет создан файл license.enc. 
2. Открыть проект в Xcode
3. В левой панели выбрать проект, затем в основном окне в списке targets выбрать 
   цель сборки, перейти на вкладку "Build Phases", выбрать там "Link Binary With Libraries",
   нажать на плюс, в появившемся окне выбрать "Add other", добавить CPROCSP.xcframework.
4. Из папки Resources из комплекта перенести все ресурсы кроме ru.lproj и en.lproj в проект.
!!!ВАЖНО!!!---------------------------------------------------------------------
   При переносе файлов следует установить флажок "Create folder referencies".
--------------------------------------------------------------------------------
5. Созданный в п.1 файл license.enc перетащить в Resources проекта(аналогично п.4,
   тоже выбрав "Create folder referencies").
6. Выделить файлы с расширением *.strings из папок Resources\en.lproj и Resources\ru.lproj,
   перетащить файлы в ресурсы приложения. В меню выбрать "Create folder referencies".
   При этом они будут автоматически сгруппированы в двуязычные файлы локализации.
7. В свойствах проекта настроить 
   "Valid Architectures: $(ARCHS_STANDARD)"
8. Если используется Xcode, в левой панели выбрать проект, затем 
   в основном окне в списке targets выбрать цель сборки, перейти на вкладку
   "Build Phases", выбрать там "Add Build Phase - Add Run Script". Появившуюся
   фазу сборки "Run Script" отредактировать, поместив в поле для скрипта
          <Путь к директории с фреймворком>/ConfigureApplication
   Если путь содержит пробелы, перед ними надо поставить backslash: "\".
9. Добавить линковку с модулем поддержки считывателя, в соответствии с инструкцией Readers.txt
10. В одном из файлов проекта определите переменную extern bool USE_CACHE_DIR; с помощью неё можно
    задавать директорию, где будут храниться ключи. 
    При задании (рекомендуется) bool USE_CACHE_DIR = false; эта директория будет Documents/cprocsp/keys/
    При задании bool USE_CACHE_DIR = true; это будет Library/Caches/cprocsp/keys/


§.3. Статическая линковка КриптоПро CSP Framework в сторонний динамический фреймворк
____________________________________________________________________________________

Для динамического фреймворка необходимо обеспечить работу контроля целостности. Для этого
используется переменная framework_control_sum. 

1. В проекте фреймворка в любой файл, где есть #import <CPROCSP/CPROCSP.h> добавить:

    #import <CPROCSP/CPROCSP.h>
    // объявляем контрольную сумму для фреймворка
    // имя переменной должно быть именно framework_control_sum
    DECLARE_CHECKSUM_VARIABLE(framework_control_sum);
    // задаем значение для использования кэша
    extern bool USE_CACHE_DIR;
    bool USE_CACHE_DIR = false;

2. Если используется Xcode, в левой панели выбрать проект, затем 
   в основном окне в списке targets выбрать цель сборки, перейти на вкладку
   "Build Phases", выбрать там "Add Build Phase - Add Run Script". Появившуюся
   фазу сборки "Run Script" отредактировать, поместив в поле для скрипта

    <Путь к директории с фреймворком>/program_checksum.sh "$CONFIGURATION_BUILD_DIR/$EXECUTABLE_PATH" framework_control_sum

3. В целевом приложении линкуем динамически свой фреймворк

§.3.1. Настройка контроля целостности для приложения при использовании динамического фреймворка
_______________________________________________________________________________________________

1. Добавить C-файл с содержимым:

    #include <CPROCSP/AppControl.h>

    check_info program_control_sum __attribute__ ((used))
                 __attribute__ ((__visibility__("default")))
                 __attribute__ ((section("__TEXT,__const")));

2. При старте приложения вызвать функцию register_checksum без аргумента framework_name

    register_checksum(NULL, &program_control_sum);

3. Добавить в целевое приложение Resources и лицензию как описано в П.2
!!!ВАЖНО!!!-------------------------------------------------------------
   Также для целевого приложения потребуются скрипты:
   ConfigureApplication, cpverify, EmbedChecksum, program_checksum.sh
   Их можно, например, добавить в папку с вашим фреймоворком
------------------------------------------------------------------------

4. Если используется Xcode, в левой панели выбрать проект, затем 
   в основном окне в списке targets выбрать цель сборки, перейти на вкладку
   "Build Phases", выбрать там "Add Build Phase - Add Run Script". Появившуюся
   фазу сборки "Run Script" отредактировать, поместив в поле для скрипта

    <Путь к директории со скриптами из п.5>/ConfigureApplication

   Таким образом контроль целостности будет обеспечен для динамического фреймворка
   и для приложения


§.4. Особые настройки и отладка
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


§.5. Настройки для проекта на swift
___________________________________

Кроме настроек из П.2 и П.4, для проектов на swift дополнительно нужно:

1. В левой панели выбрать проект, затем в основном окне в списке targets выбрать 
   цель сборки, перейти на вкладку "Build Settings". В "Linking" / "Other Linker Flags"
   указать -lc++
   Иначе будут ошибки отсутствия C++ символов в духе:
   Undefined symbol: std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >::find(char, unsigned long) const

2. Аналогично в "Build Phases" / "Link Binary With Libraries" / "Add Items"
   найти и добавить libz.tbd
   Иначе будут ошибки отсутствия символовв духе:
   Undefined symbol: _inflate


§.6. Установка корневого сертификата удостоверяющего центра
___________________________________________________________

1. Создать копию корневого хранилища
   mv /var/opt/cprocsp/users/stores/root.sto{,.old}
2. Скопировать корневое хранилище, находящееся в ресурсах приложения
   cp Resources/root.sto /var/opt/cprocsp/users/stores/
3. Каким-то образом получить корневой сертификат УЦ
   Например, скачать с помощью curl/Safari/CPFox.
   Пример: /opt/cprocsp/bin/curl https://www.cryptopro.ru:5555/ui/certnew.asp?Type=chain -o /path_to_cert/certName --insecure
4. Установить его в корневое хранилище
   /opt/cprocsp/bin/certmgr -inst -store mRoot -file /path_to_cert/certName
5. Убедиться, что сертификат был добавлен
   /opt/cprocsp/bin/certmgr -list -store mRoot
6. Переместить корневое хранилище в ресурсы приложения
   mv /var/opt/cprocsp/users/stores/root.sto Resources/root.sto
7. Вернуть исходное корневое хранилище
   mv /var/opt/cprocsp/users/stores/root.sto.old /var/opt/cprocsp/users/stores/root.sto


§.7. Сохранение/восстановление контейнеров
__________________________________________

1. С помощью iTunes File Sharing найдите приложение, слинкованное с фреймворком CPROCSP
2.
   а) Сохранение: cкопируйте папку keys, лежащую внутри cprocsp, в директорию на компьютере
   б) Восстановление: замените папку keys, лежащую внутри cprocsp, на аналогичную заранее сохраненную папку.


§.8. Тестирование
_________________

Проверить корректность работы приложения на iOS можно воспользовавшись инструкцией RunTest.txt
