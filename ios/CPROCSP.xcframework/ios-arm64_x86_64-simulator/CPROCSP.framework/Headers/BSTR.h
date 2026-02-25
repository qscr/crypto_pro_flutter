#ifndef __BSTR_H__
#define __BSTR_H__

#include <vector>

#ifdef _WIN32
#   include <comutil.h>
    
    using _com_util::ConvertBSTRToString;
    using _com_util::ConvertStringToBSTR;
#endif //_WIN32

#include <CPROCSP/compiler_attributes.h>
#ifdef UNIX
#include <cstdarg>
#include <wchar.h>
#include <CPROCSP/CSP_WinDef.h>
typedef wchar_t* BSTR;

// Utility functions to work with BSTR
/*! \ingroup EnrollAPI_support
 *  \brief Преобразование нуль-терминированной строки wchar_t в BSTR
 *
 *  \param str [in] Нуль-терминированная строка wchar_t
 *
 *  \return Полученная BSTR строка должна быть освобождена функцией 
 *	    SysFreeString()
 */
BSTR SysAllocString( const wchar_t* str);

// Utility functions to work with BSTR
/*! \ingroup EnrollAPI_support
 *  \brief Преобразование нуль-терминированной строки wchar_t в BSTR
 *
 *  \param str [in] Нуль-терминированная строка wchar_t или 0
 *  \param cch [in] Количество символов в строке
 *
 *  \return Полученная BSTR строка должна быть освобождена функцией 
 *	    SysFreeString()
 */

BSTR SysAllocStringLen( const wchar_t *str, UINT cch);

/*! \ingroup EnrollAPI_support
 *  \brief Освобождение BSTR строки
 *
 *  \param bStr [in] BSTR строка
 */
void SysFreeString( BSTR bStr);

/*! \ingroup EnrollAPI_support
 *  \brief Освобождение BSTR строки
 *
 *  \param bStr [in] BSTR строка
    \return Длина строки
 */

UINT SysStringLen(BSTR bStr);

/*! \ingroup EnrollAPI_support
 *  \brief Преобразование нуль-терминированной строки в BSTR
 *
 *  \param str [in] Нуль-терминированная строка
 *
 *  \return Полученная BSTR строка должна быть освобождена функцией 
 *	    SysFreeString()
 */

BSTR ConvertStringToBSTR( const char* str);

/*! \ingroup EnrollAPI_support
 *  \brief Преобразование BSTR в нуль-терминированную строку
 *
 *  \param bStr [in] BSTR строка
 *
 *  \return Полученная строка типа char *, должна быть 
 *	    освобождена оператором delete[] 
 */
char* ConvertBSTRToString( BSTR bStr);

#endif //UNIX

/*! \ingroup EnrollAPI_support
 *  \brief Преобразование нуль-терминированной строки в BSTR
 *
 *  \param str [in] Нуль-терминированная строка
 *  \param encoding [in] int кодировка входной строки
 *
 *  \return Полученная BSTR строка должна быть освобождена функцией 
 *	    SysFreeString()
 */

BSTR ConvertStringToBSTREx( const char* str, int encoding);

/*! \ingroup EnrollAPI_support
 *  \brief Преобразование BSTR в нуль-терминированную строку
 *
 *  \param bStr [in] BSTR строка
 *  \param encoding [in] int кодировка входной строки
 *
 *  \return Полученная строка типа char *, должна быть 
 *	    освобождена оператором delete[] 
 */

char* ConvertBSTRToStringEx( BSTR bStr, int encoding);

HRESULT ConvertBinToBSTR(const std::vector<BYTE>& encodedRequest, BSTR * bStr);
HRESULT ConvertBSTRToBin(BSTR msg, std::vector<BYTE>& ret);

#endif //__BSTR_H__

