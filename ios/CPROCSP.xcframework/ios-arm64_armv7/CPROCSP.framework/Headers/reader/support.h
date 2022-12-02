/*
 * Copyright(C) 2000-2014 Проект ИОК
 *
 * Этот файл содержит информацию, являющуюся
 * собственностью компании Крипто Про.
 *
 * Любая часть этого файла не может быть скопирована,
 * исправлена, переведена на другие языки,
 * локализована или модифицирована любым способом,
 * откомпилирована, передана по сети с или на
 * любую компьютерную систему без предварительного
 * заключения соглашения с компанией Крипто Про.
 */

#if !defined( _READER_SUPPORT_SUPPORT_H )
#define _READER_SUPPORT_SUPPORT_H

#include<CPROCSP/reader/support_base_defs.h>
#include<CPROCSP/reader/support_registry.h>

#if !defined( _SUPPORT_CALLBACK_CONV )
#   if defined( UNIX )
#	if defined( __GNUC__ ) && !defined(__clang__) && !defined(IOS) && defined (PROCESSOR_TYPE) && (PROCESSOR_TYPE != PROC_TYPE_ARM && PROCESSOR_TYPE != PROC_TYPE_ARM64 && PROCESSOR_TYPE != PROC_TYPE_PPC64 && PROCESSOR_TYPE != PROC_TYPE_PPC32 && PROCESSOR_TYPE != PROC_TYPE_MIPS32 && PROCESSOR_TYPE != PROC_TYPE_E2K32 && PROCESSOR_TYPE != PROC_TYPE_E2K64 && PROCESSOR_TYPE != PROC_TYPE_RISCV64)
#	    define _SUPPORT_CALLBACK_CONV __attribute__((regparm(0))) 
#	else // defined( __GNUC__ )
#	    define _SUPPORT_CALLBACK_CONV
#	endif // defined( __GNUC__ )
#   else // defined( UNIX )
#      define _SUPPORT_CALLBACK_CONV __cdecl
#   endif // defined( UNIX )
#endif // !defined( _SUPPORT_CALLBACK_CONV )

// Аналогичное определение есть в файлах reader/source/support/sup_def.h
// их надо держать в соответствии!
#if defined UNIX
    #define NO_SUP_NOTIFICATOR
    #define NO_SUP_WIZARD
    #define NO_SUP_PROPERTIES
#endif // UNIX

#if !defined( LANGUAGE ) 
    /*+ Выбор строки языка WIN1251. +*/
#   define _LANGTEXT(a/*+English+*/,b/*+Win1251+*/) (const wchar_t*)L ## a
#elif defined(LANGUAGE) && ( LANGUAGE == 1251 )
    /*+ Выбор строки языка WIN1251. +*/
#   define _LANGTEXT(a/*+English+*/,b/*+Win1251+*/) (const wchar_t*)L ## b
#else /* defined( LANGUAGE ) && ( LANGUAGE == 1251 )  */
    /*+ Выбор строки языка WIN1251. +*/
#   define _LANGTEXT(a/*+English+*/,b/*+Win1251+*/) (const wchar_t*)L ## a
#endif /* !defined( LANGUAGE ) || ( LANGUAGE != 866 ) && ( LANGUAGE != 1251 ) ) */

//------------------------------
// Хорошие макросы для конвертации строк, полученных из консоли, или перед выводом строк на консоль
// При destArrayLen == 0 возвращают длину требуемого буфера или 0 в случае ошибки
// При destArrayLen != 0 возвращают длину сконвертированной строки или 0 в случае ошибки
#ifdef UNIX
#define CPRO_CONSOLE_CP CP_UTF8
#else // UNIX
#define CPRO_CONSOLE_CP CP_ACP
#endif // UNIX

#define console_c2w(destW, srcC, destArrayLen) MultiByteToWideChar(CPRO_CONSOLE_CP, 0, (srcC), (int)(-1), (destW), (int)(destArrayLen))
#define console_w2c(destC, srcW, destArrayLen) WideCharToMultiByte(CPRO_CONSOLE_CP, 0, (srcW), (int)(-1), (destC), (int)(destArrayLen), NULL, NULL)
// console_c2t и console_t2c не могут быть определены, так как нельзя корректно сделать c2c без задания обеих кодировок
#ifdef UNICODE
#   define console_t2w(destW, srcT, destArrayLen) ((destArrayLen) ? (wcsncpy((destW), (srcT), (destArrayLen)), ((int)wcslen(srcT)+1)) : ((int)wcslen(srcT)+1))
#   define console_w2t(destT, srcW, destArrayLen) ((destArrayLen) ? (wcsncpy((destT), (srcW), (destArrayLen)), ((int)wcslen(srcW)+1)) : ((int)wcslen(srcW)+1))
#else /* UNICODE */
#   define console_t2w(destW, srcT, destArrayLen) console_c2w(destW, srcT, destArrayLen)
#   define console_w2t(destT, srcW, destArrayLen) console_w2c(destT, srcW, destArrayLen)
#endif /* UNICODE */

//------------------------------
// Устаревшие макросы для конвертации строк во внутреннее представление провайдера и обратно
// Категорически не рекомендуются для использования, т.к. через них затруднительно или невозможно
// вычислить длину требуемого буфера или узнать, произошла ли ошибка
#if defined _WIN32 && !defined CSP_LITE
#   define safe_mbsrtowcs( dest, src, len ) MultiByteToWideChar( CP_ACP, 0, (src), (int)(len), (dest), (int)(len) )
#   define safe_wcsrtombs( dest, src, len ) WideCharToMultiByte( CP_ACP, 0, (src), (int)(len), (dest), (int)(len), NULL, NULL )
#elif defined _WIN32
#   define safe_mbsrtowcs mbstowcs
#   define safe_wcsrtombs wcstombs
#endif

//не зависящие от TCHAR
#define _2un(dest, src, len) safe_mbsrtowcs(dest, src, len) //длина входного и выходного буфера в символах
#define _2u(dest, src) safe_mbsrtowcs(dest, src, strlen(src)+1)
#define _u2n(dest, src, len) safe_wcsrtombs(dest, src, len) //длина входного и выходного буфера в символах
#define _u2(dest, src) safe_wcsrtombs(dest, src, wcslen(src)+1)	

#ifdef UNICODE
#   define _2asciincpy( dest, src, len ) safe_wcsrtombs(dest, src, len)
#   define _2asciicpy( dest, src ) safe_wcsrtombs(dest, src, wcslen(src)+1)
#   define _ascii2ncpy( dest, src, len ) safe_mbsrtowcs(dest, src, len)
#   define _ascii2cpy( dest, src ) safe_mbsrtowcs(dest, src, strlen(src)+1)
#   define _2unincpy( dest, src, len ) wcsncpy( dest, src, len )
#   define _2unicpy( dest, src ) wcscpy( dest, src )
#   define _uni2ncpy( dest, src, len ) wcsncpy( dest, src, len )
#   define _uni2cpy( dest, src ) wcscpy( dest, src )
#else /* UNICODE */
#   define _2asciincpy( dest, src, len ) strncpy( dest, src, len )
#   define _2asciicpy( dest, src ) strcpy( dest, src )
#   define _ascii2ncpy( dest, src, len ) strncpy( dest, src, len )
#   define _ascii2cpy( dest, src ) strcpy( dest, src )
#   define _asciincpy( dest, src ) strcpy( dest, src )
#   define _2unincpy( dest, src, len ) safe_mbsrtowcs(dest, src, len)
#   define _2unicpy( dest, src ) safe_mbsrtowcs(dest, src, strlen(src)+1)
#   define _uni2ncpy( dest, src, len ) safe_wcsrtombs(dest, src, len)
#   define _uni2cpy( dest, src ) safe_wcsrtombs(dest, src, wcslen(src)+1)
#endif /* UNICODE */

// Thread Local Storage
#if !defined CSP_LITE
#   ifdef _WIN32
	typedef DWORD			TSupportTLSKeyValue;
#	define support_tls_free		TlsFree
#	define support_tls_get_value    TlsGetValue
#	define support_tls_set_value    TlsSetValue
#   else
	typedef pthread_key_t		TSupportTLSKeyValue;
#	define support_tls_free		pthread_key_delete
#	define support_tls_get_value    pthread_getspecific
#	define support_tls_set_value    pthread_setspecific
#   endif // _WIN32
#endif // CSP_LITE

#if !defined CSP_LITE
#if !defined UNIX
/* Mutex - блокирующий межнитевой и межпроцессный объект.
   Блокировка или отсутствие блокировки внутри одной нити зависит от
   реализации */
#   define support_mutex_init( mutex ) *mutex = INVALID_HANDLE_VALUE
#   define support_mutex_open( mutex, name, f) support_mutex_open_fun(mutex, name)
#   define support_mutex_lock( mutex ) WaitForSingleObject( (mutex), INFINITE )
#   define support_mutex_unlock( mutex ) ReleaseMutex( mutex )
#   define support_mutex_close( mutex ) { if( *mutex != INVALID_HANDLE_VALUE ) { CloseHandle( *mutex ); *mutex = INVALID_HANDLE_VALUE; } }
# ifndef _M_ARM
#   define support_try __try {
#   define support_finally } __finally {
# else
#   define support_try {
#   define support_finally } {
# endif
#   define support_finally_end }

/* Critical Section блокирующий межнитевой объект. Блокировки между процессами
    нет. Блокировка внутри одной нити зависит от реализации. */
#   define support_section_init( section, arg ) (InitializeCriticalSection(section),0)
#   define support_section_init_recursive(section) (InitializeCriticalSection(section),0)
#   define support_section_done( section ) (DeleteCriticalSection(section),0)
#   define support_section_enter( section ) (EnterCriticalSection(&section),0)
#   define support_section_leave( section ) (LeaveCriticalSection(&section),0)

#else /* UNIX */
#define support_mutex_init(m)           *m = NULL
#define support_mutex_open(pm, name, f) ubi_mutex_open(pm, name, f)
#define support_mutex_lock(m)           ubi_mutex_lock(m)
#define support_mutex_unlock(m)         ubi_mutex_unlock(m)
#define support_mutex_close(pm)         ubi_mutex_close(*pm)

# define support_section_init( section, arg ) pthread_mutex_init( section,NULL )
# define support_section_init_recursive(section) support_mutex_init_recursive(section)
# define support_section_done( section ) pthread_mutex_destroy( section )
# define support_section_enter( section ) pthread_mutex_lock( &section )
# define support_section_leave( section ) pthread_mutex_unlock( &section )

# define support_try {
# define support_finally }
# define support_finally_end

#endif /* UNIX */

#elif defined CSP_LITE

# if !defined UNIX
#   define support_try __try {
#   define support_finally } __finally {
#   define support_finally_end }
# else
#   define support_try {
#   define support_finally }
#   define support_finally_end
#endif

# define support_section_init( section, arg )
# define support_section_init_recursive(section)
# define support_section_done( section )
# define support_section_enter( section ) (void) 1
# define support_section_leave( section )

#endif /* CSP_LITE */

#define support_mutex_try_lock( mutex ) \
    support_mutex_lock( mutex ); support_try
#define support_section_try_enter( section ) \
    support_section_enter( section ); support_try
#define support_section_finally_leave( section ) \
    support_finally support_section_leave( section ); support_finally_end
#define RWLOCK_CLAIM(lock,mode) \
    CRWLock_Claim (lock, mode); support_try
#define RWLOCK_RELEASE(lock,mode) \
    support_finally CRWLock_Release (lock, mode); support_finally_end

#if defined (_WIN32)
    #define SUP_STOP __debugbreak(); 
#elif (defined __GNUC__ || defined __clang__) && PROCESSOR_TYPE != PROC_TYPE_ARM64
/* деление на 0 является "undefined behaviour" -->
 * используем встроенные средства компилятора
 * dim: не знаю, как написать для VS/sunCC
 * dedal: на linux/arm64 __builtin_trap() пытается вызвать abort
 */
    #define SUP_STOP { __builtin_trap(); }
#elif defined __xlC__
#if defined __cplusplus
#   include <builtins.h>
#endif	/* __cplusplus */
    #define SUP_STOP { __trap(1); }
#else
    #ifdef CSP_LITE
	#define SUP_STOP { \
		int x; \
		x=3;x=x/(3-x); \
		*(char *)(void *)(intptr_t)(x-3) = 0; \
	    }
    #else
	#define SUP_STOP { abort(); }
    #endif
#endif


#ifdef __cplusplus
# if defined(__xlC__) || defined(_AIX)
// TODO: fast AIX fix: RuNetCPC.cpp, line 388.9: 1540-0064 (S) Syntax error:  "(" was expected but "," was found.
#  define CPRO_ALIGNOF(type) 2
# else
   template<typename T> struct alignment_trick { char c; T member; };
#  define CPRO_ALIGNOF(type) CPRO_OFFSETOF(alignment_trick<type>, member)
# endif
#else // __cplusplus
# define CPRO_ALIGNOF(type) CPRO_OFFSETOF(struct { char x; type test; }, test)
#endif // __cplusplus

#define IS_ALIGNED_PTR(x,type) ( (x) && !( ((size_t)(x)) % (CPRO_ALIGNOF(type)) ) )

typedef void(*SUP_PROC_PTR)(void);

#if 0 //defined _WIN32 && !defined CSP_LITE
#   define IS_READ_PTRS(x,size) ( !IsBadReadPtr(x,size) )
#   define IS_WRITE_PTRS(x,size) ( !IsBadWritePtr(x,size) )
#   define IS_READ_PTR(x) ( !IsBadReadPtr(x,1) )
#   define IS_WRITE_PTR(x) ( !IsBadWritePtr(x,1) )
#   define IS_STRING_PTRS(x,size) ( !IsBadStringPtr(x,size) )
#   define IS_STRING_PTR(x) ( !IsBadStringPtr(x,(size_t)-1) )
#   define IS_FUN_PTR(x) (!IsBadCodePtr((FARPROC)x) )
#   define IS_ALIGNED_RPTR(x,type) (IS_ALIGNED_PTR(x,type) && IS_READ_PTRS(x,sizeof(type)))
#   define IS_ALIGNED_WPTR(x,type) (IS_ALIGNED_PTR(x,type) && IS_WRITE_PTRS(x,sizeof(type)))
#else
/* Проверка на ноль: 
   TODO: на выровненность можем проверять wchar-строки и там,
   где сами выделяем память: SUPSYS_PRE_CONTEXT(), SUPSYS_PRE_INFO */
SUP_INLINE static int IsGoodPtr(const void *p) { return (p != NULL); }
SUP_INLINE static int IsGoodCodePtr(SUP_PROC_PTR p) { return (p != NULL); }
#   define IS_READ_PTRS(x,size) IsGoodPtr(x)
#   define IS_WRITE_PTRS(x,size) IsGoodPtr(x)
#   define IS_READ_PTR(x) IS_READ_PTRS(x,1)
#   define IS_WRITE_PTR(x) IS_WRITE_PTRS(x,1)
#   define IS_STRING_PTRS(x,size) IsGoodPtr(x)
#   define IS_STRING_PTR(x) IsGoodPtr(x)
#   define IS_FUN_PTR(x) IsGoodCodePtr((SUP_PROC_PTR)x)
#   define IS_ALIGNED_RPTR(x,type) IS_ALIGNED_PTR(x,type)
#   define IS_ALIGNED_WPTR(x,type) IS_ALIGNED_PTR(x,type)
#endif

#define SUPDLL_NULL
#define SUPDLL_LOAD_PROC(nn,name,err) \
    if(!nn##_entry.name##_proc){ \
        if( !nn##_module ) { \
            if( nn##_load_library() ) return err; \
        } \
        nn##_entry.name##_proc = (T##name##Proc)support_load_library_getaddr( nn##_module, #name ); \
        if( !nn##_entry.name##_proc ) return err; \
    }

#define SUPDLL_ITEMT(nn,name,arg_list,type,conv) \
    typedef type (conv *T##name##Proc) arg_list;
#define SUPDLL_ITEMI(nn,name) \
    T##name##Proc name##_proc;

#define SUPDLL_ITEMI_B(nn) \
    static TSupModuleInstance nn##_module = NULL; \
    static int nn##_load_count = 0; \
    struct nn##Entry_ {
#define SUPDLL_ITEMI_E(nn) \
    } nn##_entry;

#define SUPDLL_ITEMI_B_INSIDE(nn) \
    static TSupModuleInstance nn##_module = NULL; \
    static int nn##_load_count = 0; \
    static struct nn##Entry_ {

#define SUPDLL_LOAD(nn,name) \
    DWORD nn##_load_library( void ) { \
        if( nn##_load_count ) { nn##_load_count++; return 0; } \
        nn##_module = support_load_library_registry( name, \
            SUP_LOAD_LIBRARY_DEFAULT ); \
        if( nn##_module == NULL ) return (DWORD)NTE_PROVIDER_DLL_FAIL; \
        nn##_load_count++; \
        return 0; \
    }

#define SUPDLL_LOAD_SYSTEM(nn,name) \
    DWORD nn##_load_library( void ) { \
    if( nn##_load_count ) { nn##_load_count++; return 0; } \
    nn##_module = support_load_library_registry( name, \
    SUP_LOAD_LIBRARY_DEFAULT | SUP_LOAD_LIBRARY_SYSTEM ); \
    if( nn##_module == NULL ) return (DWORD)NTE_PROVIDER_DLL_FAIL; \
    nn##_load_count++; \
    return 0; \
    }

#define SUPDLL_UNLOAD(nn) \
    void nn##_unload_library( void ) { \
        if( !nn##_load_count ) return; \
        nn##_load_count--; \
        if( nn##_load_count ) return; \
        memset( &nn##_entry, 0, sizeof( nn##_entry ) ); \
        support_unload_library_registry( nn##_module ); \
    }

#define SUPDLL_ITEMR2(nn,name,arg_list,call_list,type,conv,err) \
    type conv name arg_list { \
        SUPDLL_LOAD_PROC(nn,name,err) \
        return nn##_entry.name##_proc call_list; \
    }

#define SUPDLL_ITEMV(nn,name,arg_list,call_list) \
    void name arg_list { \
        SUPDLL_LOAD_PROC(nn,name,SUPDLL_NULL) \
        nn##_entry.name##_proc call_list; \
    }

#define SUPDLL_ITEMVL(nn,name,arg_list,name2,last,call_list) \
    void name arg_list { \
        va_list valist; \
        va_start( valist, last ); \
        SUPDLL_LOAD_PROC(nn,name2,SUPDLL_NULL); \
        nn##_entry.name2##_proc call_list; \
	va_end(valist); \
    }

#define SUPDLL_ITEMRL(nn,name,arg_list,name2,last,call_list,type,err) \
    type name arg_list { \
        va_list valist; \
	type _retcode_; \
        va_start( valist, last ); \
        SUPDLL_LOAD_PROC(nn,name2,err); \
        _retcode_ = nn##_entry.name2##_proc call_list; \
        va_end(va_list); \
        return _retcode_; \
    }

#define SUPLIB_ITEMR2(nn,name,arg_list,call_list,type,conv,err) \
    type conv name arg_list { return err; }
#define SUPLIB_ITEMV(nn,name,arg_list,call_list) \
    void name arg_list { return; }
#define SUPLIB_ITEM(nn,name,arg_list,call_list) \
    SUPLIB_ITEMR2(nn,name,arg_list,call_list,DWORD,SUPDLL_NULL,(DWORD)ERROR_NOT_SUPPORTED)
#define SUPLIB_ITEMVL(nn,name,arg_list,call_list) \
    void name arg_list { return; }

#if defined UNUSED
    #undef  UNUSED
#endif
#define UNUSED(x)   (void)(x)

/* Общие ошибки, которые рекомендуется повторять в верхнем интерфейсе. */

/* Общий код ошибки: отсутствие ошибок или функция завершилась успешно. +*/
//#define (DWORD)ERROR_SUCCESS -> ERROR_SUCCESS

/*+ Общий код ошибки: нехватка памяти. +*/
//#define (DWORD)NTE_NO_MEMORY -> NTE_NO_MEMORY

/*+ Общий код ошибки: ПО неправильно установлено или искажено. +*/
//#define (DWORD)NTE_PROVIDER_DLL_FAIL -> NTE_PROVIDER_DLL_FAIL

/*+ Общий код ошибки: неправильно пареданы параметры. +*/
//#define (DWORD)ERROR_INVALID_PARAMETER -> ERROR_INVALID_PARAMETER

/*+ Общий код ошибки: Функция не поддерживается. +*/
//#define SUP_ERR_UNSUPPORTED -> ERROR_NOT_SUPPORTED

/*+ Тип ситуаций, не являющихся ошибками. +*/
#define CPR_ERR_RETCODE ((0<<31)|(1<<29)|(0x052D0000))

/*+ Тип ситуаций, являющихся ошибками. +*/
#define CPR_ERR_FAIL (((DWORD)1<<31)|((DWORD)1<<29)|(0x052D0000))

/*+ Код возврата: возможно несколько корретных ответов для данной функции. +*/
#define CPR_ERR_UNCERTAINTY (CPR_ERR_RETCODE|0x0001)
/*+ Код возврата: требуется продолжение выполнения операции. +*/
#define CPR_ERR_BLOCK (CPR_ERR_RETCODE|0x0002)

/*+ Ошибка: выполняются команды из разных транзакций. +*/
#define CPR_ERR_TRANSACTION (CPR_ERR_FAIL|0x0001)

/*+ Ошибка: Достиг нуля один из счетчиков. +*/
#define CPR_ERR_OPLIMIT (CPR_ERR_FAIL|0x0002)

/*+ Общий код ошибки: Неизвестная ошибка. +*/
//#define SUP_ERR_UNKNOWN -> CPR_ERR_UNCERTAINTY, ERROR_INVALID_PARAMETER

/*+ Общий код ошибки: Отказ от выполнения операции. +*/
//#define SUP_ERR_CANCEL -> (DWORD)SCARD_W_CANCELLED_BY_USER

/*+ Общий код ошибки: Недостаточно прав. +*/
//#define SUP_ERR_RIGHTS -> NTE_PERM

//#define SUP_ERR_CONNECT -> (DWORD)SCARD_E_READER_UNAVAILABLE

/*+ Общий код ошибки: функция отключена. +*/
//#define SUP_ERR_FEATURE_DISABLED -> NTE_PERM

/*+ Код ошибки функции поддержки: Секция, ключ или параметр не найден. +*/
//#define SUP_ERR_KEY_NOT_FOUND -> ERROR_FILE_NOT_FOUND

/*+ Код ошибки функции поддержки: Секция, ключ или параметр искажены. +*/
//#define SUP_ERR_KEY_INVALID -> ERROR_FILE_INVALID

/*+ Код ошибки функции поддержки: Неправильный язык или ресурс. +*/
//#define SUP_ERR_RESOURCE -> ERROR_RESOURCE_NOT_PRESENT

/*+ Код ошибки, примерно означавший "не подключилась сторонняя библиотека, необходимая для работы модуля поддержки". 
    Обрабатывалась странно. +*/
//#define SUP_ERR_INSTALL_ADD1 -> ERROR_INVALID_DLL

#define SUP_TYPE_UNKNOWN 0

/* Тип параметра строка. */
#define SUP_TYPE_STRING 0x1

/* Тип параметра long. */
#define SUP_TYPE_LONG 0x2

/* Тип параметра Boolean. */
#define SUP_TYPE_CSP_BOOL 0x4

/* Тип параметра Hex. */
#define SUP_TYPE_HEX 0x8

/* Тип параметра Section. */
#define SUP_TYPE_SECTION 0x10

/* Тип параметра Section. */
#define SUP_TYPE_MULTI_SZ 0x20

/* Тип параметра long long */
#define SUP_TYPE_LONG_LONG 0x40

/* Права выдаются с учетом необходимости создания подключей. */

/* Доступ к ключу на открытие, получение значения параметров. */
#define SUP_ACCESS_READ 0x1
/* Доступ к ключу на создание, изменение и удаление параметров */
#define SUP_ACCESS_MODIFY 0x2
/* Доступ на создание подключей. */
#define SUP_ACCESS_CREATE_SUBKEY 0x4
/* Доступ на удаление подключей. */
#define SUP_ACCESS_DELETE_SUBKEY 0x8
/* Доступ на получение прав. */
#define SUP_ACCESS_GET_ACCESS 0x10
/* Доступ на установку прав. */
#define SUP_ACCESS_SET_ACCESS 0x20
/* Доступ на перечисление ключей. */
#define SUP_ACCESS_ENUM_SUBKEY 0x40
/* Доступ на уведомление об изменениях. */
#define SUP_ACCESS_NOTIFY 0x80
/*Доступ к 64-битному ключу*/
#define SUP_64KEY_DIRECT_ACCESS 0x100
/* Доступ к SACL. */
#define SUP_ACCESS_SYSTEM_SECURITY           (0x01000000L)

/* If we have bit from this mask, we need to open file for read/write */
#define SUP_ACCESS_WRITE_MASK (SUP_ACCESS_MODIFY|SUP_ACCESS_CREATE_SUBKEY|SUP_ACCESS_DELETE_SUBKEY|SUP_ACCESS_SET_ACCESS)

#define SUPPORT_NOTIFY_SUBTREE 0x1
#define SUPPORT_NOTIFY_CHANGE 0x2
#define SUPPORT_NOTIFY_RENAME 0x4
#define SUPPORT_NOTIFY_SECURITY 0x8
#define SUPPORT_NOTIFY_PASSTHROUGH 0x10000000

#define SUP_LOAD_LIBRARY_STD 0

// Разрешить загрузку библиотеки, если её регистрации нет в AppPath
#define SUP_LOAD_LIBRARY_DEFAULT    1
// Загрузка с флагом RTLD_GROUP (разделение таблиц символов). Solaris-only.
#define SUP_LOAD_LIBRARY_SEPARATE   2
// Не прогружать полностью библиотеку (RTLD_LAZY). Unix-only.
#define SUP_LOAD_LIBRARY_LAZY	    4
// При поиске DLL не использовать путь загружающего модуля. Windows-only.
#define SUP_LOAD_LIBRARY_SYSTEM	    8
// experimental, disabled - При поиске DLL пользоваться перенаправлением, если задано. Windows-only.
#define SUP_LOAD_LIBRARY_REDIRECTION 16
// Не журналировать ошибку загрузки библиотеки. Unix-only.
#define SUP_LOAD_LIBRARY_NO_ERROR_LOG 32
// Искать загружаемую библиотеку только в системном расположении. Windows-only (System32).
#define SUP_LOAD_LIBRARY_SYSTEM_PATH 64

/* --------------- TYPES --------------- */

#if defined _WIN32
    typedef unsigned ( __stdcall *TSupFun )( void* );

    typedef struct
    {
	int max_resume_time;
	int timer_id;
    } TSupResumeTimeStruct;
#endif

typedef enum CRWLockMode_ {
    CRWLOCK_NONE = 1,
    CRWLOCK_HIGHLOAD_NOSERIALIZED_READ = 2,
    CRWLOCK_READ = 3,
    CRWLOCK_ANY = 4,
    CRWLOCK_WRITE = 5,
    CRWLOCK_ERROR = -1
} CRWLockMode;

#if defined UNIX && defined CSP_LITE
    typedef void * TSupportCriticalSection;
    typedef void * TSupportMutex;
#elif defined UNIX && !defined CSP_LITE
    typedef ubi_named_mutex_t *TSupportMutex;
    typedef pthread_mutex_t TSupportCriticalSection;
#elif defined _WIN32 && defined CSP_LITE
    typedef struct _FAST_MUTEX *PFAST_MUTEX;
    typedef PFAST_MUTEX TSupportCriticalSection;
#elif defined _WIN32
    typedef HANDLE TSupportMutex;
    typedef CRITICAL_SECTION TSupportCriticalSection;
    struct CRWLock_t
    {
	LONGLONG dummy[2*16];
    };
#endif

typedef struct CRWLock_t CRWLock;

struct TSupportSharedMemory_;
typedef struct TSupportSharedMemory_ TSupportSharedMemory;

/* Типы параметров. */
typedef unsigned int TSupType;

/* Типы параметров. */
typedef unsigned int TSupAccess;

/* Контекст в registry. */
struct TSupportRegistrySearchContext_;

typedef struct TSupportRegistrySearchContext_ TSupportRegistrySearchContext;

struct TSupportRegistrySearchValue_;

typedef struct TSupportRegistrySearchValue_ TSupportRegistrySearchValue;

#ifdef NO_SUP_NOTIFICATOR
    typedef int TSupRegNotificator;
#else /* !NO_SUP_NOTIFICATOR */
    struct TSupRegNotificator_;
    typedef struct TSupRegNotificator_ TSupRegNotificator;
#endif /* !NO_SUP_NOTIFICATOR */

    struct TSupFileEnum_;

typedef struct TSupFileEnum_ TSupFileEnum;

typedef enum TSupFileType_ {
    SUP_FTYPE_IGNORE,
    SUP_FTYPE_FILE,
    SUP_FTYPE_DIR
} TSupFileType;

#if !defined CSP_LITE
    /* Гарантированный однократный запуск функции */
	/* TODO:X Заменить на что-то в духе:
	 * typedef boost::once_flag TSupportOnce;
	 * #define SUPPORT_ONCE_INIT BOOST_ONCE_INIT
	 * #define support_once(once_control, init_routine) boost::call_once(init_routine, once_control)
	 */
    #ifdef UNIX

	typedef pthread_once_t TSupportOnce;
	#define SUPPORT_ONCE_INIT PTHREAD_ONCE_INIT
	#define support_once(once_control, init_routine) \
			    (pthread_once((once_control), (init_routine)))
    #else
	typedef struct TSupportOnce_ {
	    LONG lLock;
	    LONG lDone;
	    LONG lInited;
	} TSupportOnce;
	#define SUPPORT_ONCE_INIT { 0, 0, 0 }

	static __inline int
	support_once(TSupportOnce *once_control, 
		     void (*init_routine)(void))
	{
	    LONG lThreadID = GetCurrentThreadId();
	    int ret = 0;

	    if(once_control == NULL || init_routine == NULL) {
		return /*EINVAL*/22;
	    }
	    if(once_control->lDone) {	// "Стандартная" оптимизация
	    	return 0;
	    }
	    if(!InterlockedExchangeAdd(&once_control->lDone, 0)) 
	    { /* MBR fence */
		while(0 != InterlockedCompareExchange(
				&once_control->lLock, lThreadID, 0)) {
		    Sleep(100);
		}
		if(!once_control->lDone) 
		{
		    (*init_routine)();
		    if(0 != InterlockedExchangeAdd(&once_control->lDone, 1)) {
			// TODO:XX Достаточно было бы признака volatile на 
			// lDone, однако, прототип InterlockedExchangeAdd()
			// на CE странный.
			// TODO:X Добавить сообщение об обломе. Если 
			// Interlocked*() работают, то управление сюда 
			// не попадёт.
			SUP_STOP;
			ret = -1; /* Internal error */
		    }
		    once_control->lInited = lThreadID;
		}	    
		if(lThreadID != once_control->lLock) {
		    // TODO:X Добавить сообщение об обломе. Если 
		    // Interlocked*() работают, то управление сюда 
		    // не попадёт.
		    SUP_STOP;
		    ret = -1; /* Internal error */
		}
		once_control->lLock = 0;
	    }
	    return ret;
	}
    #endif
#endif

#if defined( SUPPORT_RESOURCE_STD )
    typedef struct TSupStringTableItem_
    {
	size_t num; /*!< Номер элемента. */
	const wchar_t *string; /*!< Строка. */
    } TSupStringTableItem;

    /*! Элемент версии работающего во всех операционках. */
    typedef struct TSupVersionItem_
    {
	const wchar_t *company; /*!< Компания производитель. */
	const wchar_t *copyright; /*!< Copyright */
	unsigned int version[4]; /*!< Номер версии. */
    } TSupVersionItem;

    /*! Элемент иконок работающего во всех операционках. */
    typedef struct TSupIconTableItem_
    {
	int num; /*!< Номер элемента. */
	const wchar_t *icon_name; /*!< Имя иконки. */
    } TSupIconTableItem;

    /*!
     * \brief resource работающий во всех операционках
     */
    #if defined(UNIX) && !defined(CSP_LITE)
	typedef void (* TSupOnceFunc)(void);
	typedef struct TSupInstanceCatalog_
	{
	    TSupportOnce once_control;
	    const TSupOnceFunc once_func;
            pthread_mutex_t lock;
	    nl_catd nl_id;
	} TSupInstanceCatalog;
    #endif
    typedef struct TSupInstanceResourceDecl_
    {
	#if defined(UNIX) && !defined(CSP_LITE) && defined(USE_NLCAT)
	    TSupInstanceCatalog *catalog;
	#else
	    int string_table_max; /*!< Размер строкового ресурса. */
	    const TSupStringTableItem *string_table; /*!< Собственно string table. */
       	#endif
	const TSupVersionItem *version; /*!< Указатель на информацию о версии. */
	int icon_table_max; /*!< Размер ресурса иконок. */
	const TSupIconTableItem *icon_table; /*!< Указатель на иконки. */
    } TSupInstanceResourceDecl;

    /*!
     * \brief instance работающий во всех операционках
     */
    typedef const TSupInstanceResourceDecl * TSupResourceInstance;
#else
    /*!
     * \brief instance ресурса определяется как DLL instance.
     */
	typedef HINSTANCE TSupResourceInstance;
#endif /* !defined( SUPPORT_RESOURCE_STD ) */

#if defined _WIN32 && !defined CSP_LITE
    typedef HINSTANCE TSupModuleInstance;
    typedef HICON TSupIconHandle;
    typedef HBITMAP TSupBitMap;
    typedef FARPROC TSupProc;
#else
    typedef void* TSupModuleInstance;
    typedef void *TSupIconHandle;
    typedef void *TSupBitMap;
    typedef void *TSupProc;
#endif /* _WIN32 && !CSP_LITE */

typedef struct 
{
    DWORD width;	/* ширина */
    DWORD height;       /* высота */
    DWORD priority;	/* приоритет, если есть несколько иконок с одинаковыми размерами */
    TSupIconHandle icon; /* хендл */
} TSupIconItem;

#define TSUP_ICON_TYPE_SHARED	0 /*хендл предоставляет наиболее подходящую иконку*/
#define TSUP_ICON_TYPE_MANY	1 /*в хендле хранится массив разнообразных иконок*/

typedef struct
{
    DWORD count;	    /* число иконок */
    TSupIconItem * icons;   /* указатель на массив иконок */
    DWORD icon_type;	    /* тип иконки, TSUP_ICON_TYPE_SHARED или TSUP_ICON_TYPE_MANY */
} StTSupIcon, *TSupIcon;


#if defined(UNIX) && !defined CSP_LITE
#   include <sys/time.h>
#endif /* UNIX && !CSP_LITE */

typedef struct timeval support_timeval;

/* --------------- FUNCTIONS --------------- */
#if defined( __cplusplus )
extern "C" {
#endif /* defined( __cplusplus ) */

DWORD usenewreg(void); /* переключение на новый имитатор. 0 -- удавчно, не-0 -- ошибка */
DWORD rlsnewreg(void); /* завершаем работу с новым, переключаемся на старый. Всегда успешно */
DWORD isnewreg(void);   /* 0 -- старый, не-0 -- новый */

unsigned long       support_get_last_error (void);
void                support_set_last_error (unsigned long);

#ifdef _WIN32
void support_set_library(TSupModuleInstance support_ext_module);
#else //_WIN32
#define support_set_library(x) 
#endif //_WIN32

DWORD support_set_impersonate (int delayed, int hsmmode, int kb2mode);

/* Thread Local Storage. Склад для данных, уникальных для каждой нити. */
DWORD support_set_thread_specific (unsigned int key, const void * ptr, void **old_ptr);
void * support_get_thread_specific (unsigned int key);
DWORD support_tskey_create (unsigned int* key);
DWORD support_tskey_delete (unsigned int key);

DWORD support_thread_actualize_uids ( void );
DWORD support_thread_deactualize_uids ( void );

#if defined _WIN32 && !defined CSP_LITE
DWORD support_mutex_open_fun(TSupportMutex *mutex, const TCHAR *name);
#endif

#if defined( _WIN32 )
DWORD support_shared_memory_create( 
    TSupportSharedMemory **shared, const TCHAR *file_name, const TCHAR *lock_name,
    size_t sizeof_shared );
DWORD support_shared_memory_remove( 
    TSupportSharedMemory *shared );
void* support_shared_memory_lock( 
    TSupportSharedMemory *shared );
DWORD support_shared_memory_unlock( 
    TSupportSharedMemory *shared );
#endif /* _WIN32 */

/* Значение таймаута по умолчанию */
#define DEF_DIALOG_TIMEOUT_VALUE 600

/* Функция установки в конфиг значения таймаута диалоговых окон провайдера.
   Если start_timeout == NULL, стирает параметр.
   Иначе пишет в параметр * start_timeout.*/
DWORD support_set_to_config_dialog_timeout(CSP_BOOL is_global, int * start_timeout);

/* Функция чтения значения таймаута диалоговых окон провайдера.
   Возвращает ERROR_FILE_NOT_FOUND в случае ошибки или отсутствия параметра в конфиге.
   Иначе возвращает ERROR_SUCCESS, и, если указан start_timeout, то в нем значение таймаута. */
DWORD support_get_from_config_dialog_timeout(CSP_BOOL is_global, int * start_timeout);

/* Функция чтения значений таймаутов диалоговых окон провайдера */
void support_get_dialog_timeouts(int * start_timeout, int * show_timeout, int * resume_timeout);

/* Функция получения строкового параметра. */
DWORD support_registry_get_string(
    const TCHAR *path, size_t *length, TCHAR *dest );

DWORD support_registry_get_multi_string(
    const TCHAR *path, size_t *length, TCHAR *dest );

/* Функция получения long параметра. */
DWORD support_registry_get_long(
    const TCHAR *path, long *dest );

/* Функция получения long параметра. */
DWORD support_registry_get_long_long(
    const TCHAR *path, long long *dest );

/* Функция получения Boolean параметра. */
DWORD support_registry_get_bool(
    const TCHAR *path, int *dest );

/* Функция получения нетипизированного параметра. */
DWORD support_registry_get_hex(
    const TCHAR *path, size_t *size, void *dest );

/* Функция записи строкового параметра. */
DWORD support_registry_put_string(
    const TCHAR *path, const TCHAR *src );

/* Функция записи строкового параметра. */
DWORD support_registry_put_multi_string(
    const TCHAR *path, const TCHAR *src );

/* Функция записи long параметра. */
DWORD support_registry_put_long(
    const TCHAR *path, long src );

/* Функция записи long long параметра. */
DWORD support_registry_put_long_long(
    const TCHAR *path, long long src );

/* Функция записи Boolean параметра. */
DWORD support_registry_put_bool(
    const TCHAR *path, int src );

/* Функция записи нетипизированного параметра. */
DWORD support_registry_put_hex(
    const TCHAR *path, size_t size, const void *src );

#if defined _WIN32
/* Функция записи access control list на параметр или секцию. */
DWORD support_registry_put_acl(
    const TCHAR *path, const void *src );

/* Функция записи system access control list на параметр или секцию. */
DWORD support_registry_put_sacl(
	const TCHAR *path, const void *src );

#if !defined EXCLUDE_READER

DWORD support_registry_get_sdecr ( 
				    const TCHAR *path, 
				    const DWORD secInfo, 
				    void * pSec, DWORD * pSecLen, 
				    long * pError );

DWORD support_registry_put_sdecr ( const TCHAR *path, 
				     const DWORD secInfo, 
				     void * pSec, 
				     long * pError );

DWORD support_folder_get_sdecr(
    const TCHAR* path,
    const DWORD secInfo,
    void * pSec,
    DWORD * pSecLen
);


DWORD support_folder_put_sdecr(
    const TCHAR *path,
    const DWORD secInfo,
    const void * pSec
);

#   endif  /* !EXCLUDE_READER */

#endif /* _WIN32 */

#if defined (_WIN32) && !defined (CSP_LITE)
/* Функция получения параметра PP_KEYSET_SEC_DESCR ветки реестра где лежит ключ. */
DWORD support_get_PP_KEYSET_SEC_DESCR( BYTE *pbData, DWORD *pdwDataLen);
#endif

/* Функция записи секции. */
DWORD support_registry_put_section(const TCHAR *path);

/* Функция проверки существования секции. */
DWORD support_registry_test_section( 
    const TCHAR *path );

/* Функция удаления параметра из реестра. */
DWORD support_registry_delete_param(
    const TCHAR *path, const TCHAR *src );

/* Функция удаления секции из реестра. */
DWORD support_registry_delete_section(
    const TCHAR *path, const TCHAR *src );

/* Функция открытия контекста. */
DWORD support_registry_search_open(
    const TCHAR *path, TSupportRegistrySearchContext **context,
    size_t *max_name, int subkey );

/* Функция сравнения контекста поиска. */
int support_registry_search_cmp(
    TSupportRegistrySearchContext *left,
    TSupportRegistrySearchContext *right );

/* Функция закрытия контекста. */
DWORD support_registry_search_close(
    TSupportRegistrySearchContext *context );

/* Функция сдвига указателя контекста поиска. */
DWORD support_registry_search_next(
    TSupportRegistrySearchContext *context,
    TSupportRegistrySearchValue **right );

/* Функция поиска значения. */
DWORD support_registry_find(
    const TCHAR *path, TSupportRegistrySearchValue **value );

DWORD support_registry_value_cpy(
    TSupportRegistrySearchValue **left,
    const TSupportRegistrySearchValue *right );

/* Функция освобождения значения. */
DWORD support_registry_value_free(
    TSupportRegistrySearchValue *src );

DWORD support_registry_value_string(
    const TSupportRegistrySearchValue *src, size_t *length, TCHAR *dest );

DWORD support_registry_value_multi_string(
    const TSupportRegistrySearchValue *src, size_t *length, TCHAR *dest );

/* Функция получения long параметра. */
DWORD support_registry_value_long(
    const TSupportRegistrySearchValue *src, long *dest );

DWORD support_registry_value_long_long(
    const TSupportRegistrySearchValue *src, long long *dest );

DWORD support_registry_value_bool(
    const TSupportRegistrySearchValue *src, int *dest );

DWORD support_registry_value_hex(
    const TSupportRegistrySearchValue *src, size_t *size, void *dest );

DWORD support_registry_value_type(
    const TSupportRegistrySearchValue *src, TSupType *type, size_t *size );

DWORD support_registry_value_name(
    const TSupportRegistrySearchValue *src, size_t *length, TCHAR *dest );

/* Функция получения имени параметра. */
DWORD support_registry_get_param(
    TSupportRegistrySearchContext *context, size_t length, TCHAR *dest );

/* Функция получения типа пути. */
DWORD support_registry_type(
    const TCHAR *path, TSupType *type, size_t *size );

/* Функция получения типа пути. */
DWORD support_registry_check_access(
    const TCHAR *path, TSupAccess *acc );

#ifdef NO_SUP_NOTIFICATOR
#ifdef _MSC_VER
#pragma warning (disable:4127)
#endif
#define support_registry_notify_set(a,b,n) 0
#define support_registry_notify_is(n) (DWORD)SCARD_W_CANCELLED_BY_USER
#define support_registry_notify_done(n)
#else /* !NO_SUP_NOTIFICATOR */
DWORD support_registry_notify_set(
    const TCHAR *path, unsigned flags, TSupRegNotificator **notificator );

DWORD support_registry_notify_is(
    TSupRegNotificator *notificator );

DWORD support_registry_notify_done(
    TSupRegNotificator *notificator );
#endif /* !NO_SUP_NOTIFICATOR */

/* Функция получения строки из псевдо-ресурса. */
DWORD support_resource_string(
    TSupResourceInstance instance, size_t ids, TCHAR *dest, size_t *length );

/* Функция получения строки на заданном языке из псевдо-ресурса. */
DWORD support_resource_string_local(
    TSupResourceInstance instance, WORD language, size_t ids, TCHAR *dest, size_t *length );

/*! \ingroup group_resource
 * Функция получения компании производителя из псевдо-ресурса.
 */
DWORD support_resource_company(
    TSupResourceInstance instance, /*+ (i) для Windows Dll - instance, иначе имя массива. +*/
    TCHAR *dest, /*+ (io) Результирующая строка или NULL для определения длины. +*/
    size_t *length);

/* Функция получения произвольной именованой строки из ресурса VersionInfo */
DWORD support_resource_version_string(
    const TCHAR *target,
    TSupResourceInstance instance, TCHAR *dest, size_t *length );

DWORD support_icon_free(TSupIcon bitmap);

#define SUP_ICON_PRIORITY_FLAG_COLOR 0xf00
#define SUP_ICON_PRIORITY_1	     1
#define SUP_ICON_PRIORITY_2	     2
#define SUP_ICON_PRIORITY_4	     4
#define SUP_ICON_PRIORITY_8	     8
#define SUP_ICON_PRIORITY_16	     16
#define SUP_ICON_PRIORITY_24	     24
#define SUP_ICON_PRIORITY_32	     32

static SUP_INLINE TSupIconHandle support_get_best_icon_handle(TSupIcon icon, DWORD width, DWORD height, DWORD priority)
{
    DWORD i;
    TSupIconItem * pret = NULL;
    if (icon == NULL) return NULL;
    for (i = 0; i < icon->count - 1; i++) {
	if (icon->icons[i].width == width && icon->icons[i].height == height) {
	    if (priority >= icon->icons[i].priority && (pret == NULL || pret->priority < icon->icons[i].priority))
		pret = &icon->icons[i];
	}
    }
    if (pret == NULL)
	return NULL;
    return pret->icon;
}
DWORD support_get_icon_handle(TSupIcon icon, TSupIconHandle* dest);

DWORD support_net_2_icon(
    void* hwnd, LPBYTE pb_data, DWORD dw_size, TSupIconHandle* result);

DWORD support_icon_2_net(
    void* hwnd, TSupIconHandle icon, LPBYTE pb_data, DWORD * dw_size);

/* Функция получения иконки из псевдо-ресурса. */
DWORD support_resource_bitmap(
    TSupResourceInstance instance, size_t idi, TSupBitMap *dest );

DWORD support_bitmap_free(
    TSupBitMap bitmap );

WORD support_wnd_language_get(void);
DWORD support_wnd_language_update(void);
DWORD support_wnd_language_done(void);

// в length принимает и возвращает длину строки без учёта нуль-терминатора
DWORD support_path2dir(
    const TCHAR *src, size_t *length, TCHAR *dest );

DWORD support_opendir(
    const TCHAR *path, size_t *max_path, TSupFileEnum **ctx );

DWORD support_nextent(
    TSupFileEnum *ctx, TCHAR *name, TSupFileType *type );

DWORD support_closedir(
    TSupFileEnum *ctx );

/* Функция получения идентификатора текущего пользователя. */
DWORD support_user_id(
    size_t *length, TCHAR *dest );

#if defined UNIX && !defined CSP_LITE
#include <unistd.h>
/* Функция получения идентификатора текущего пользователя. */
DWORD support_user_dir(
    size_t *length, TCHAR *dest );
/* Функция получения идентификаторов текущего пользователя. */
DWORD support_user_id_ex(
    size_t *length, TCHAR *dest,
    uid_t *euid,  uid_t *egid );

/* Функция временной смены текущего пользователя. */
DWORD support_impersonate_user_by_uids (
    uid_t euid,  uid_t egid );
#endif	/* UNIX && !CSP_LITE */

/* Функция получения имени текущего пользователя. */
DWORD support_user_name(
    TCHAR *dest, size_t *length );

/* Функция временной смены текущего пользователя. */
DWORD support_impersonate_user (
    const TCHAR *id );

/* Функция возвращения исходного пользователя. */
DWORD support_revert_to_self (void);

/* Функция получения точного текущего времени. */
//DWORD
//support_compound_time(long long *lpCompoundCount);

/* Функция получения текущего времени. */
DWORD support_gettimeofday (
    support_timeval * stv);

/* Функция установки заданного в секундах времени. */
DWORD support_time_set (
    support_timeval * tv_res, long secs);

/* Функция сравнения и вычитания времени. tv_res = tv1 - tv2 */
DWORD support_time_sub (
    const support_timeval * tv1, const support_timeval * tv2,
    support_timeval * tv_res, int* cmp);

/* Функция сложения времени. tv_res = tv1 + tv2 */
DWORD support_time_add (
    const support_timeval * tv1, const support_timeval * tv2,
    support_timeval * tv_res);

/* Преобразование времени из timeval в ULONGLONG (filetime) */
#define TimevalToULongLong(cpc_timeval)		\
    (unsigned long long)(cpc_timeval.tv_sec*10000000ULL + cpc_timeval.tv_usec*10ULL + 116444736000000000ULL)

#ifndef CSP_LITE
    DWORD support_time2tm(
	support_timeval * tv_src, struct tm * tm_dest);

    DWORD support_tm2time(
	struct tm * tm_src, support_timeval * tv_dest );
#endif /* CSP_LITE */

#if defined _WIN32 && !defined CSP_LITE
    DWORD create_thread_same_rights(
	TSupFun fun, void* info, HANDLE *thread );
#endif /* _WIN32 && !CSP_LITE */

CSP_BOOL CRWLock_Initialize (CRWLock * lock, void *arg);
DWORD CRWLock_Cleanup (CRWLock * lock);
DWORD CRWLock_Claim (CRWLock * lock, CRWLockMode mode);
DWORD CRWLock_Release (CRWLock * lock, CRWLockMode mode);

#if !defined _WIN32
    size_t safe_mbsrtowcs( wchar_t *dest,
	const char *src, size_t len);
    size_t safe_wcsrtombs( char *dest,
	const wchar_t *src, size_t len);
#endif

TSupModuleInstance support_load_library_registry(
    const TCHAR *dll_name, int def );

void support_unload_library_registry(TSupModuleInstance handle);
TSupProc support_load_library_getaddr(TSupModuleInstance handle,
    const char *name);

#ifdef _WIN32
TSupModuleInstance support_load_dll(const TCHAR *path, int mode);
void support_unload_dll( TSupModuleInstance handle );
#endif //_WIN32

DWORD support_registry_get_app_path(
    const TCHAR *path, size_t *length, TCHAR *dest );

DWORD support_registry_get_app_path_ex(
    const TCHAR *path, size_t *length, TCHAR *dest, int do_not_use_this_file_path);

#ifdef WIN32
/* заполняет путь до вызывающего модуля для ускорения загрузки библиотек */
CSP_BOOL support_init_module_path(void);
#endif 

/* Прогрузка библиотеки. */
DWORD support_load_library( void );

/* Выгрузка библиотеки. */
void support_unload_library( void );

/* нужно при сборке CAdES на Linux */
#if defined UNIX
# ifndef TLS_OUT_OF_INDEXES
#   define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)
# endif
#endif /* UNIX */

#if !defined(SUPPORT_ALIGN)
#   define SUPPORT_ALIGN	16
#endif

#define support__empty__

#define support_align_var(type, var)                    \
    support_align_n(type, SUPPORT_ALIGN, var, support__empty__)

#define support_align_array(type, var, add)             \
    support_align_n(type, SUPPORT_ALIGN, var, add)

#if defined(_AIX)
	// Не работает на стеке
#   define support_align_n(typ, alg, var, add)		\
	typ __align(alg) var add
#elif defined(__SUNPRO_C) || defined (__SUNPRO_CC)
	// Не работает на стеке
#   define sa_da_pragma(p)	_Pragma(#p)
#   define support_align_n(typ, alg, var, add)		\
	sa_da_pragma(align alg ( var ))			\
	typ var add
#elif defined(__GNUC__)
	// Не работает на стеке
#   define support_align_n(typ, alg, var, add)		\
	typ var add __attribute__ ((aligned (alg)))
#elif defined(_WIN32)
	// Работает на стеке
#   define support_align_n(typ, alg, var, add)		\
	__declspec(align(alg)) typ var add
#else
#   error "Can't support align"
#endif

#if defined(_AIX)
#   if defined(__STDC__) && defined __STDC_VERSION__ && __STDC_VERSION__ - 0 >= 199901L
#       define SUPPORT_RESTRICT restrict
#   else
#       define SUPPORT_RESTRICT __restrict
#   endif
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#   if defined(__STDC__) && defined __STDC_VERSION__ && __STDC_VERSION__ - 0 >= 199901L
#       define SUPPORT_RESTRICT restrict
#   else
#       define SUPPORT_RESTRICT
#   endif
#elif defined(__GNUC__)
#   if defined(__STDC__) && defined __STDC_VERSION__ && __STDC_VERSION__ - 0 >= 199901L
#       define SUPPORT_RESTRICT restrict
#   else
#       define SUPPORT_RESTRICT __restrict
#   endif
#elif defined(_WIN32)
#   define SUPPORT_RESTRICT     __restrict
#else
#   error "Can't support restrict"
#endif

#if !defined(CSP_LITE) && !defined(BUG_IN_KERNEL_SPACE_C)
# if defined(UNIX) 
    typedef uint16_t WIN_WCHAR_T;

    static SUP_INLINE 
    size_t support_utf16le_len(const WIN_WCHAR_T * src)
    {
	size_t n;
	for (n=0;src[n];n++)
	    ;
	return n;
    }

    static SUP_INLINE 
    size_t support_utf16le_nlen(const WIN_WCHAR_T * src, 
				size_t maxlen /* size in win_wchars */)
    {
	size_t n;
	for (n=0; n < maxlen && src[n];n++)
	    ;
	return n;
    }
    
    /* Обратите внимание!
       а) Скорее всего вы хотите обработать всю строку, включая терминирующий
       ноль. В этом случае вы передаёте длину исходной строки без
       терминирующего нуля или с ним, получаете при первом вызове функции
       результат "нужна память для N символов", выделяете память для N+1
       символов, но в качестве длины выходного буфера передаёте N.
       б) Даже если вы обрабатываете нетерминированные строки, то обращаться с
       выходным буфером нужно так же, как описано в пункте а).
       в) В параметрах этих функций данные в UTF-16LE измеряются в символах, а
       данные в других кодировках -- в байтах.
       г) Если функция сообщает, что записала N символов/байт, то это значит,
       что она записала N символов/байт + терминирующий ноль (всего N+1
       символ/байт).
       д) Длина входного буфера (slen) считается естественным образом. */
    DWORD 
    support_to_utf16le(
	const char * encoding, /* encoding or NULL (current locale) */
	WIN_WCHAR_T * dst, /* buffer to hold utf16 string or NULL to calculate*/
	size_t * pdlen, /* max size of message in win_wchars w/o terminating zero */
	const TCHAR * src, /* source string (encoded using encoding) */
	size_t slen /* max size of message in chars w/o terminating zero */
	);

    DWORD 
    support_from_utf16le(
	 const char * encoding, /* enconding or NULL (current locale) */
	 TCHAR * dst, /* Buffer to hold the string or NULL to calculate */ 
	 size_t * pdlen, /* max size of message in chars w/o terminating zero */
	 const WIN_WCHAR_T * src, /* utf16 string */ 
	 size_t slen /* max size of message in win_wchars w/o terminating zero */
	 );
# else /* defined(UNIX) */
    typedef wchar_t WIN_WCHAR_T;

#   define support_utf16le_len(src) wcslen(src)
#   define support_utf16le_nlen(src,maxlen) wcsnlen((src),(maxlen))

#   ifdef UNICODE
	DWORD SUP_INLINE 
	support_to_utf16le(const char * encoding, 
			   WIN_WCHAR_T * dst, 
			   size_t * pdlen, 
			   const TCHAR * src, 
			   size_t slen)
	{
	    size_t len;
	    (void)(encoding); // TODO:XXX UNUSED(encoding);
	    if (pdlen == NULL || src == NULL)
		return (DWORD)ERROR_INVALID_PARAMETER;
	    // TODO: wcsncpy_s(dst, 1+*pdlen, src, slen+1);
	    len = wcsnlen(src, slen);
	    if (dst == NULL)
	    {
		*pdlen=len;
		return (DWORD)NTE_NO_MEMORY;
	    }
	    if (*pdlen >= len)
	    {
		wcsncpy_s(dst, 1+*pdlen, src, len+1);
		return ERROR_SUCCESS;
	    }
	    else
	    {
		wcscpy_s(dst, 1+*pdlen, src);
		return (DWORD)NTE_NO_MEMORY;
	    }
	}

	DWORD SUP_INLINE 
	support_from_utf16le(const char * encoding, 
			     TCHAR * dst, 
			     size_t * pdlen, 
			     const WIN_WCHAR_T * src, 
			     size_t slen)
	{
	    size_t len;
	    (void)(encoding); // TODO:XXX UNUSED(encoding);
	    if (pdlen == NULL || src == NULL)
		return (DWORD)ERROR_INVALID_PARAMETER;
	    // TODO: wcsncpy_s(dst, 1+*pdlen, src, slen+1);
	    len=wcsnlen(src,slen);
	    if (dst == NULL)
	    {
		*pdlen=len;
		return (DWORD)NTE_NO_MEMORY;
	    }
	    if (*pdlen >= len) 
	    {
		wcsncpy_s(dst, 1+*pdlen, src, len+1);
		return ERROR_SUCCESS;
	    }
	    else
	    {
		wcscpy_s(dst, 1+*pdlen, src);
		return (DWORD)NTE_NO_MEMORY;
	    }
	}
#   else
	DWORD SUP_INLINE 
	untested_support_to_utf16le(const char * encoding, 
				    WIN_WCHAR_T * dst, 
				    size_t * pdlen, 
				    const TCHAR * src, 
				    size_t slen)
	{
	    size_t alloclen = 0;
	    (void)(encoding); // TODO:XXX UNUSED(encoding);
	    (void)(slen); // TODO:XXX UNUSED(encoding);

	    if (pdlen == NULL || src == NULL)
		return (DWORD)ERROR_INVALID_PARAMETER;
	    if (dst) {
		alloclen = 1 + *pdlen;
	    }
	    if(mbstowcs_s(pdlen, dst, alloclen, src, alloclen-1) ||
		NULL == dst) {
		    return (DWORD)NTE_NO_MEMORY;
	    }
	    return ERROR_SUCCESS;
	}

	DWORD SUP_INLINE 
	untested_support_from_utf16le(const char * encoding, 
				      TCHAR * dst, 
				      size_t * pdlen, 
				      const WIN_WCHAR_T * src, 
				      size_t slen)
	{
	    size_t alloclen = 0;
	    (void)(encoding); // TODO:XXX UNUSED(encoding);
	    (void)(slen); // TODO:XXX UNUSED(encoding);

	    if (pdlen == NULL || src == NULL)
		return (DWORD)ERROR_INVALID_PARAMETER;
	    if (dst) {
		alloclen = 1 + *pdlen;
	    }
	    if(wcstombs_s(pdlen, dst, alloclen, src, alloclen-1) ||
		NULL == dst) {
		return (DWORD)NTE_NO_MEMORY;
	    }
	    return ERROR_SUCCESS;
	}
#   endif
# endif /* defined(UNIX) */
#endif /* defined(CSP_LITE) */

/*
    Собственный strdup для замены аллокатора.
*/
#ifndef CSP_LITE
#   ifdef UNICODE
#       define support_tcsdup support_wcsdup
#   else /* UNICODE */
#       define support_tcsdup support_strdup
#   endif /* UNICODE */

static SUP_INLINE char *support_strdup(const char *str) {
    size_t siz = 0;
    char *copy = NULL;
    siz = (strlen(str) + 1) * sizeof(char);
    copy = (char*)malloc(siz);
    if (copy != NULL) {
	memcpy(copy, str, siz);
    }
    return copy;
}

static SUP_INLINE wchar_t *support_wcsdup(const wchar_t *str) {
    size_t siz = 0;
    wchar_t *copy = NULL;
    siz = (wcslen(str) + 1) * sizeof(wchar_t);
    copy = (wchar_t*)malloc(siz);
    if (copy != NULL) {
	memcpy(copy, str, siz);
    }
    return copy;
}
#endif /* CSP_LITE */

#define SUPPORT_CSUM_BNAME_LEN 40
#define SUPPORT_CSUM_MNAME_LEN SUPPORT_CSUM_BNAME_LEN
#define SUPPORT_CSUM_HASH_LEN 32

DWORD support_register_checksum_block(
    const char * module_name,
    const char * block_name,
    const void * block_ptr,
    size_t block_length,
    const void * checksum);

DWORD support_unregister_checksum_block(
    const char * module_name,
    const void * block_ptr,
    size_t block_length);

DWORD support_is_checksum_block_registered(
    const char * module_name,
    const void * block_ptr,
    size_t block_length);

DWORD support_free_all_checksum_blocks(void);

DWORD support_register_csm_module(
    const char *module_name);

DWORD support_is_csm_module_registered(
    const char *module_name);

DWORD support_unregister_csm_module(
    const char *module_name);

typedef DWORD _SUPPORT_CALLBACK_CONV support_checksum_callback_t (
    const char * module_name,
    const char * block_name,
    const void * block_ptr,
    size_t block_length,
    const void * checksum,
    void * arg);

DWORD support_verify_blocks(
    support_checksum_callback_t *callback, void * arg);


//
// Поддержка кэша чтения/записи из/в реестр.
//

#ifdef UNIX // Время кэширования в секундах.
#   define SRC_TIMEOUT (10)
#else
#   define SRC_TIMEOUT (1)
#endif // UNIX

//
// Для использования SUPPORT_REGISTRY_CACHE_DECL(CP, 0)
// Требуется:
// #include "../../CSP/src/RuNetCSP/reality/stdlocks.h"
// #include "reader/support.h"
//

#define SRC_CPC_pCP_CALL_CTX		pCP_CALL_CTX

#define SRC_CPC_RWLOCK_INIT(pCC,pS,a)	CPC_RWLOCK_INIT(pCC,pS,a)
#define SRC_CPC_RWLOCK_RDLOCK(pCC,pS)	CPC_RWLOCK_RDLOCK(pCC,pS)
#define SRC_CPC_RWLOCK_WRLOCK(pCC,pS) 	CPC_RWLOCK_WRLOCK(pCC,pS) 
#define SRC_CPC_RWLOCK_UNLOCK(pCC,pS)	CPC_RWLOCK_UNLOCK(pCC,pS)
#define SRC_CPC_INTERLOCKED_CASL(pCC,plvD,S,C) \
					CPC_INTERLOCKED_CASL(pCC,plvD,S,C)
#define SRC_CPC_INTERLOCKED_INCREMENT(pCC,plvD) \
					CPC_INTERLOCKED_INCREMENT(pCC,plvD)
#define SRC_CPC_timeval			CPC_timeval
#define SRC_CPC_GET_TIME(pCC,pTV)	CPC_GET_TIME(pCC,pTV)
#define SRC_CPC_CACHE_LOCK_DECL		CPC_LOCK_OBJ* src_lock_ = &pCC_->hCSP->ParamsLock;
#define SRC_CPC_CACHE_INIT

#define SRC_CP_pCP_CALL_CTX	struct { struct { CSP_BOOL fSerialize; } *hCSP; } *

#define SRC_CP_RWLOCK_INIT(pCC,pS,a)	rwlock_init((&pS), sizeof(pS), NULL)
#define SRC_CP_RWLOCK_RDLOCK(pCC,pS)	rwlock_rdlock(&pS)
#define SRC_CP_RWLOCK_WRLOCK(pCC,pS)	rwlock_wrlock(&pS)
#define SRC_CP_RWLOCK_UNLOCK(pCC,pS)	rwlock_unlock(&pS)
#define SRC_CP_INTERLOCKED_CASL(pCC,plvD,S,C) \
				support_interlocked_CASL((plvD), (S), (C))
#define SRC_CP_INTERLOCKED_INCREMENT(pCC,plvD) \
				support_interlocked_increment(plvD)
#define SRC_CP_timeval			support_timeval
#define SRC_CP_GET_TIME(pCC,pTV)	(ERROR_SUCCESS == support_gettimeofday(pTV))
#define SRC_CP_CACHE_LOCK_DECL  static volatile LONG src_CP_inited_ = 0; \
				static CPC_RWLOCK src_lock_;

#define SRC_CP_CACHE_INIT  	while (2 != src_CP_inited_) { \
				    if(SRC_CP_INTERLOCKED_CASL(pCC_, &src_CP_inited_, 1, 0)) { \
					SRC_CP_RWLOCK_INIT(pCC_, src_lock_, TRUE); \
					SRC_CP_INTERLOCKED_INCREMENT(pCC_, &src_CP_inited_); \
				    } \
				}

#if !defined(CSP_LITE)	// CPCSP-4228 - аномалии в драйвере
	    // Начало блока кэша реестра.
	    // Начало описания static структур данных для кэширования.
	    // t == CP - не использовать pCP_CALL_CTX, в этом случае pCC 
	    //           должен быть равным 0 (функции уровня CP);
	    // t == CPC - использовать pCP_CALL_CTX (функции уровня CPС).
    #define SUPPORT_REGISTRY_CACHE_DECL(t, pCC) { \
	    SRC_##t##_pCP_CALL_CTX pCC_ = (pCC); \
	    static CSP_BOOL src_cached_ok_ = FALSE; \
	    SRC_##t##_CACHE_LOCK_DECL \
	    static SRC_##t##_timeval src_prev_ = { 0 };
		// Определения кэшируемых данных

	    // Начало раздела стирания кэш, блокировка текущего состояние на чтение
	    // после нее можно проверить закэшированные данные
    #define SUPPORT_REGISTRY_CACHE_INVALIDATE_1(t)  \
	    SRC_##t##_timeval src_cur_ = { SRC_TIMEOUT, SRC_TIMEOUT }; \
	    CSP_BOOL src_##t##_use_ = FALSE; \
	    CSP_BOOL src_cache_ = FALSE; \
	    CSP_BOOL src_try_check_ = FALSE; \
	    int src_try_cnt_ = 3; \
	    \
	    do { \
		src_try_check_ = FALSE; \
		src_cache_ = FALSE; \
		src_##t##_use_ = (0 == pCC_ || pCC_->hCSP->fSerialize) \
				 && 0 < src_try_cnt_--; \
			/* Если !fSerialize, то CPC_RWLOCK_*LOCK */ \
			/* могут возвращать ошибки, поэтому не */ \
			/* кэшируем */ \
			/* Для 0 == pCC_, rwlock_*lock имеют тип */ \
			/* void и никогда не возвращают ошибок */ \
		if(src_##t##_use_) { \
		    SRC_##t##_CACHE_INIT \
		    if(SRC_##t##_GET_TIME(pCC_, &src_cur_)) { \
			SRC_##t##_RWLOCK_RDLOCK(pCC_, src_lock_);

	    // проверка времени или условия (cond).
	    // cond == FALSE - инвалидировать кэш только по времени.
    #define SUPPORT_REGISTRY_CACHE_INVALIDATE_2(t, cond)  \
			    src_cache_ = src_cached_ok_ && \
			       src_cur_.tv_sec < src_prev_.tv_sec + SRC_TIMEOUT && \
			       src_cur_.tv_sec > src_prev_.tv_sec - SRC_TIMEOUT; \
			SRC_##t##_RWLOCK_UNLOCK(pCC_, src_lock_); \
		    }; \
		    if(!src_cache_ || (cond)) { \
			src_cache_ = FALSE; \
			SRC_##t##_RWLOCK_WRLOCK(pCC_, src_lock_); \
			support_try \
			    if (src_cached_ok_) { \
				    /* Если никто ещё не */ \
				    /* инвалидировал в параллель */ \
				src_cached_ok_ = FALSE; \
				src_prev_.tv_sec = 0; 
				// Инвалидирование кэш. Недопустимы: return,
				// исключения и др.
				/*...*/

	    // Начало раздела стирания кэш, по времени или по условию (cond).
	    // cond == FALSE - инвалидировать кэш только по времени.
    #define SUPPORT_REGISTRY_CACHE_INVALIDATE(t, cond)  \
	    SUPPORT_REGISTRY_CACHE_INVALIDATE_1(t) \
	    SUPPORT_REGISTRY_CACHE_INVALIDATE_2(t, cond) \

	    // Начало раздела реального кода (реального обращения в реестр)
	    // в случае, если данные кэширования были инвалидированны.
    #define SUPPORT_REGISTRY_CACHE_NOCACHE(t)  \
				/*...*/ \
			    } \
			support_finally \
			SRC_##t##_RWLOCK_UNLOCK(pCC_, src_lock_); \
			support_finally_end; \
		    } \
		    /* Если !src_cache_, то кэш инвалидирован в */ \
		    /* этом потоке */ \
		}; \
		if(!src_##t##_use_ || !src_cache_) {
		    // Реальный доступ к реестру для записи или 
		    // получения данных. Допустимы: return, 
		    // исключения и т.п.
		    /*...*/

	    // Начало раздела заполнения данных кэширования по результатам
	    // реального кода (реального обращения в реестр).
	    // ok == TRUE - всегда заполнять данные кэширования.
    #define SUPPORT_REGISTRY_CACHE_FILL(t, ok) \
		    /*...*/ \
		    if(src_##t##_use_ & (ok)) { \
			SRC_##t##_RWLOCK_WRLOCK(pCC_, src_lock_); \
			support_try \
			    if (!src_cached_ok_) { \
				    /* Если никто ещё не */ \
				    /* заполнил в параллель */ \
				src_cached_ok_ = FALSE; \
				src_prev_.tv_sec = 0; 
				// Заполнение кэш. Недопустимы: return,
				// исключения и др.
				/*...*/

	    // Начало раздела использования данных кэширования в случае 
	    // если реальный код (реальное обращение в реестр) не вызывалось.
    #define SUPPORT_REGISTRY_CACHE_CACHE(t) \
				/*...*/ \
				if(SRC_##t##_GET_TIME(pCC_, &src_prev_)) { \
				    src_cached_ok_ = TRUE; \
				} \
			    } \
			support_finally \
			SRC_##t##_RWLOCK_UNLOCK(pCC_, src_lock_); \
			support_finally_end; \
		    } \
		} else if(src_##t##_use_) { \
			/* TODO: if(src_##t##_use_) преобразовать в */ \
			/* assert(src_##t##_use_); */ \
		    SRC_##t##_RWLOCK_RDLOCK(pCC_, src_lock_); \
		    support_try \
			if (!src_cached_ok_) { \
			        /* Если кто-то впараллель */ \
				/* инвалидировал кэш, то повтор */ \
			    src_try_check_ = TRUE; \
			} else {
			    // Чтение кэш. Недопустимы: return,
			    // исключения и др.
			    /*...*/

	    // Окончание блока кэша реестра.
    #define SUPPORT_REGISTRY_CACHE_END(t) \
			    /*...*/ \
			} \
		    support_finally \
		    SRC_##t##_RWLOCK_UNLOCK(pCC_, src_lock_); \
		    support_finally_end; \
		} \
	    } while(src_##t##_use_ && src_try_check_); \
	}
#else
    #define SUPPORT_REGISTRY_CACHE_DECL(t, pCC)         { int src_##t##_false_ = 0;
    #define SUPPORT_REGISTRY_CACHE_INVALIDATE(t, cond)  if(src_##t##_false_) {
    #define SUPPORT_REGISTRY_CACHE_NOCACHE(t)           } \
                                                        {
    #define SUPPORT_REGISTRY_CACHE_FILL(t, ok)          } \
                                                        if(src_##t##_false_) {
    #define SUPPORT_REGISTRY_CACHE_CACHE(t)
    #define SUPPORT_REGISTRY_CACHE_END(t)               }}
#endif

//
// Контроль захвата/освобождения блокировок. А так же их освобождения при 
// возникновении необработанных C++ исключений и сигналов.
//

#define SLR_DNTABR	(1)	// Даже в случае DEBUG всё равно не делать abort(),
				// только протокольное сообщение.
#define SLR_NORMAL	(0)	// Для DEBUG - сообщение в протокол и abort(), в
				// противном случае, только сообщение.

typedef void(*slr_pfDestruct_t)(void *);
				// Указатель на функцию освобождения блокировки.

#if !defined(CSP_LITE) && defined UNIX && (SECURITY_LEVEL > KC1)
#define ENABLE_SLR_STACK
#endif

		// Регистрируем захват блокировки.
    void  support_lckrec_push_slr_impl(void *pvLock, slr_pfDestruct_t pfUnlock);
		// Регистрируем освобождение блокировки.
    void  support_lckrec_pop_slr_impl(void *pvLock);
		// Проверяем, что в стеке зарегистрированых блокировок
		// не больше uBottom записей.
    DWORD support_lckrec_check_slr_impl(unsigned uDntAbr, unsigned uBottom);
		// Освобождаем записанные блокировки до уровня uBottom записей.
    DWORD support_lckrec_flush_slr_impl(unsigned uDntAbr, unsigned uBottom);
		// Возвращает количество записанных блокировок в стеке.
    unsigned support_lckrec_top_slr_impl(void);

#if defined (ENABLE_SLR_STACK)
    #define support_lckrec_push(pvLock, pfUnlock) \
	support_lckrec_push_slr_impl((pvLock), (pfUnlock))
    #define support_lckrec_pop(pvLock) \
	support_lckrec_pop_slr_impl((pvLock))
    #define support_lckrec_check(uDntAbr, uBottom) \
	support_lckrec_check_slr_impl((uDntAbr), (uBottom))
    #define support_lckrec_flush(uDntAbr, uBottom) \
	support_lckrec_flush_slr_impl((uDntAbr), (uBottom))
    #define support_lckrec_top() \
	support_lckrec_top_slr_impl()
#else
    static SUP_INLINE void support_lckrec_push(void *pvLock, slr_pfDestruct_t pfUnlock) {
        UNUSED(pvLock); 
        UNUSED(pfUnlock);
    }

    static SUP_INLINE void support_lckrec_pop(void *pvLock) {
        UNUSED(pvLock);
    }
    
    static SUP_INLINE DWORD support_lckrec_check(unsigned uDntAbr, unsigned uBottom) {
        UNUSED(uDntAbr); 
        UNUSED(uBottom); 
        return (DWORD)ERROR_SUCCESS;
    }
    
    static SUP_INLINE DWORD support_lckrec_flush(unsigned uDntAbr, unsigned uBottom) {
        UNUSED(uDntAbr); 
        UNUSED(uBottom); 
        return (DWORD)ERROR_SUCCESS;
    }
    
    static SUP_INLINE unsigned  support_lckrec_top(void) {
        return 0;
    }
#endif // ENABLE_SLR_STACK


// Функции отладочного динамического контроля блокировок
#if defined(DEBUG) || defined(_DEBUG)
    #define SLR_DEBUG_CHECK_TOP(uT) unsigned (uT) = support_lckrec_top()
    #define SLR_DEBUG_CHECK(uT)     (support_lckrec_check(SLR_NORMAL, (uT)))
#else
    #define SLR_DEBUG_CHECK_TOP(uT) unsigned (uT)
    #define SLR_DEBUG_CHECK(uT)     ((void)(uT))
#endif

// Функции зануления памяти
#   define support_zero_memory_raw(dest,size) {volatile unsigned char *volatile_dest = (volatile unsigned char *)dest;\
					   size_t int_size = (size_t)size;\
					   while (int_size--) *volatile_dest++ = 0x00;}
#ifdef _WIN32 
#   define support_zero_memory(dest,size) RtlSecureZeroMemory(dest,size)
#else
#   define support_zero_memory(dest,size) support_zero_memory_raw(dest,size)
#endif	/* _WIN32 */

// Блок, требующий переработки (функции из support_fkc.h) TODO
//
// Макросы вызова функций преобразования типа от родителя к сыну и обратно
//

/*!
* \ingroup rdr_ext_fkc_macro
* \def P2CH(ChildType,Parent)
* \brief Приведение указателя на структуру-родителя к типу-наследнику. Используется при
*  определении типов параметров, переданных в функции аутентификации и считывания параметров папок.
*  Макрос вызывает соответствующую функцию, что гарантирует возникновение предупреждений при компиляции,
*  если данное приведение невозможно.
* \param ChildType [in] Тип-наследник, к которому будет приводится типизированный указатель.
* \param Parent [in] Типизированный указатель на структуру-родителя.
* \ret Указатель на структуру типа-наследника.
* \sa CONSTP2CH, CH2P.
* \req_suprdr
*/
#define P2CH(ChildType,Parent)	    (Child##ChildType(Parent))

/*!
* \ingroup rdr_ext_fkc_macro
* \def CONSTP2CH(ChildType,Parent)
* \brief Приведение указателя-константы на структуру-родителя к типу-наследнику. Используется при
*  определении типов параметров, переданных в функции аутентификации и считывания параметров папок.
*  Макрос вызывает соответствующую функцию, что гарантирует возникновение предупреждений при компиляции,
*  если данное приведение невозможно.
* \param ChildType [in] Тип-наследник, к которому будет приводится типизированный указатель.
* \param Parent [in] Типизированный указатель-константа на структуру-родителя.
* \ret Указатель на структуру типа-наследника.
* \sa P2CH, CH2P.
* \req_suprdr
*/
#define CONSTP2CH(ChildType,Parent) (CChild##ChildType(Parent))

/*!
* \ingroup rdr_ext_fkc_macro
* \def CH2P(ChildType,Child)
* \brief Приведение указателя на структуру-наследника к типу-родителю. Используется при
*  возврате параметров от функций аутентификации и считывания параметров папок.
*  Макрос вызывает соответствующую функцию приведения, что гарантирует возникновение предупреждений
*  при компиляции, если данное приведение невозможно.
* \param ChildType [in] Тип-наследник передаваемого указателя, который следует привести к типу-родителю.
* \param Parent [in] Типизированный указатель на структуру-наследника.
* \ret Указатель на структуру типа-родителя.
* \sa P2CH, CONSTP2CH.
* \req_suprdr
*/
#define CH2P(ChildType,Child)	    (Parent##ChildType(Child))

/* _countof helper */
/* взято из protobuf/src/google/protobuf/stubs/common.h */
/* там жило под названием GOOGLE_ARRAYSIZE */
#if !defined(_countof)
#if !defined(__cplusplus)
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#else
#define _countof(a) \
  ((sizeof(a) / sizeof(*(a))) / \
   static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif
#endif	/* !_countof */

#define support_len_of(a)    (sizeof(a)/sizeof((a)[0]))

#ifdef UNIX
void support_init_locale(void);
#endif

#if defined( __cplusplus )
}
#endif

#ifndef CSP_LITE
#include<CPROCSP/reader/support_util.h>
#endif //CSP_LITE

#endif /* !_READER_SUPPORT_SUPPORT_H */
