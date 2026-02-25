#ifndef CALLCTX_H_INCLUDED
#define CALLCTX_H_INCLUDED 1

#include <CPROCSP/BaseArithmDef_64.h>
#include <CPROCSP/compiler_attributes.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _CP_CALL_CTX_ CP_CALL_CTX, *pCP_CALL_CTX;
typedef struct _CRYPT_CSP_ CRYPT_CSP, * LPCRYPT_CSP;

typedef struct _CRYPT_CSP_THREAD_		CRYPT_CSP_THREAD,		*LPCRYPT_CSP_THREAD;
typedef struct _CSP_THREAD_TEMP_DATA		CSP_THREAD_TEMP_DATA,		*LPCSP_THREAD_TEMP_DATA;
typedef struct _CSP_THREAD_TEMP_CIPHER_DATA	CSP_THREAD_TEMP_CIPHER_DATA,	*LPCSP_THREAD_TEMP_CIPHER_DATA;
typedef struct _CSP_THREAD_TEMP_HASH_DATA	CSP_THREAD_TEMP_HASH_DATA,	*LPCSP_THREAD_TEMP_HASH_DATA;
typedef struct _CSP_THREAD_VTB_MP		CSP_THREAD_VTB_MP,		*LPCSP_THREAD_VTB_MP;

typedef struct _CP_ASTACK_ CP_ASTACK;



/* Предопределение структуры ДСЧ. */
struct RND_CONTEXT_;

/*! \internal
* \brief Указатель на функцию получения случайных чисел.
*
* \param context [in/out] Контекст ДСЧ, реально передается указатель на более
*  полную структуру контекста.
* \param buffer [out] Буфер, на выходе заполненный случайной последовательностью.
* \param length [in] Размер буфера под случайную последовательность.
* \return TRUE, случайная последовательность получена, FALSE иначе.
*/
typedef CSP_BOOL(*GetRandomFunction)(pCP_CALL_CTX pCallCtx, struct RND_CONTEXT_ *context, LPBYTE buffer, uint32_t length, uint32_t flags) ATTR_USERES;

/*! \internal
* \brief Указатель на функцию получения случайных чисел.
*
* \param context [in/out] Контекст ДСЧ, реально передается указатель на более
*  полную структуру контекста.
* \param additional_input [in] Дополнительные данные для подачи на вход функции генерации
* \param additional_input_length [in] Длина дополнительных данных для подачи на вход функции генерации
* \param buffer [out] Буфер, на выходе заполненный случайной последовательностью.
* \param length [in] Размер буфера под случайную последовательность.
* \return TRUE, случайная последовательность получена, FALSE иначе.
*/

typedef CSP_BOOL(*GetRandomFunctionEx)(pCP_CALL_CTX pCallCtx, struct RND_CONTEXT_ *context,
    const BYTE * additional_input, uint32_t additional_input_length,
    LPBYTE buffer, uint32_t length, uint32_t flags) ATTR_USERES;

/*! \internal
* \brief Указатель на функцию проверки инициализированности ДСЧ.
*
* \param context [in/out] Контекст ДСЧ, реально передается указатель на более
*  полную структуру контекста.
* \return TRUE, случайная последовательность может быть получена (ДСЧ инициализирован),
*  FALSE иначе.
*/
typedef CSP_BOOL(*IsRandomInitedFunction)(pCP_CALL_CTX pCallCtx, struct RND_CONTEXT_ *context) ATTR_USERES;

/*! \internal
* \brief Указатель на функцию установки инициализирующей последовательности.
*
* \param context [in/out] Контекст ДСЧ, реально передается указатель на более
*  полную структуру контекста.
* \param seed [in] Инициализирующая последовательность.
*/
typedef CSP_BOOL(*SetRandomSeedFunction)(pCP_CALL_CTX pCallCtx, struct RND_CONTEXT_ *context, const LPBYTE seed, size_t length, DWORD dwInitFlags) ATTR_USERES;

/*! \internal
* \brief Указатель на функцию установки перезамешивания энтропии.
*
* \param context [in/out] Контекст ДСЧ
* \param entropy_input_reseed [in] Энтропия
* \param entropy_input_length [in] Длина энтропии
* \param additional_input [in] Дополнительные данные
* \param additional_input_length [in] Длина дополнительных данных
*/
typedef CSP_BOOL(*ReseedFunction)(pCP_CALL_CTX pCallCtx, struct RND_CONTEXT_ *context,
    const BYTE * entropy_input_reseed, DWORD entropy_input_length,
    const BYTE * additional_input_reseed, DWORD additional_input_length) ATTR_USERES;

/*! \internal
* \brief Контекст функции получения случайной последовательности.
*/
typedef struct RND_CONTEXT_ {

    DWORD Flags;			    /*!< Флаг состояния контекста.*/
    DWORD dwSecurityLevel;

    /* общие функции для всех датчиков */
    GetRandomFunction make_random; /*!< Указатель на функцию получения случайного числа. */
    IsRandomInitedFunction is_inited; /*!< Указатель на функцию проверки инициализированности ДСЧ. */

    /* функции гостового датчика */
    SetRandomSeedFunction set_random_seed; /*!< Указатель на функцию установки random_seed. */

    /* функции зарубежного датчика */
    ReseedFunction reseed;
    GetRandomFunctionEx make_random_ex; /*!< Указатель на функцию получения случайного числа с возможностью дозамешать дополнительные данные. */

    void * pOuter; /* указатель на внешнюю структуру DRBG, используется в зарубежке */
} RND_CONTEXT, *LPRND_CONTEXT;

/* Реентерабельные варианты CSP_SetLastError/CSP_GetLastError */
void	rSetLastError	(pCP_CALL_CTX pCallCtx, DWORD err);
DWORD	rGetLastError	(pCP_CALL_CTX pCallCtx) ATTR_USERES;

#ifdef _CRTDBG_MAP_ALLOC
#define rAllocMemory(pCallCtx,dwSize,dwMemPoolId) (pCallCtx, dwMemPoolId,calloc(dwSize, 1))
#define rFreeMemory(pCallCtx,pMem,dwMemPoolId)  (pCallCtx, dwMemPoolId,free(pMem))
#else
/* Реентерабельные варианты AllocMemory/FreeMemory */
LPVOID	rAllocMemory	(pCP_CALL_CTX pCallCtx, size_t dwSize, DWORD dwMemPoolId) ATTR_USERES;
void	rFreeMemory	(pCP_CALL_CTX pCallCtx, VOID *pMem, DWORD dwMemPoolId);
#endif

/*! Инициализация памяти */
CSP_BOOL	rInitMemory	(pCP_CALL_CTX pCallCtx) ATTR_USERES;

#if !defined UNIX
/*! Уничтожение всех куч */
void	rDoneMemory	(pCP_CALL_CTX pCallCtx);
#endif /* !UNIX */
/*! Проверка целостности куч */
void	rValidateMemory	(pCP_CALL_CTX pCallCtx);

void	rInitCallCtx	(pCP_CALL_CTX pCallCtx, LPCRYPT_CSP hCSP);


#define	 ROOT_PRSG_CRITICAL_ERROR	   1

struct _CP_CALL_CTX_
{
    LPCRYPT_CSP			hCSP;
    LPCRYPT_CSP_THREAD		hCSPthread; // логичнее, чем каждый раз переходить к основному hCSP через hCSPthread
    LPRND_CONTEXT		ThreadPRSG; // Чтобы можно было подменять ДСЧ в макросе MakeRandom
    DWORD			dwError;
    DWORD			dwThreadId;
    DWORD			dwCommonKArrayLength;
    BYTE                       *pbCommonKArray; 
    CSP_BOOL			bOwnFPU;
    int				iCntFPU;
    DWORD			dwFPUOpType;
    DWORD			dwCriticalErrorFlags;

    struct cp_vtb_28147_89     *pVTB;
    CP_ASTACK		       *pAStk;
};


#ifdef __cplusplus
}
#endif

#endif /* CALLCTX_H_INCLUDED */
