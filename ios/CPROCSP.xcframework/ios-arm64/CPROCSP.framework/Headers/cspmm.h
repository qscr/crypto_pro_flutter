/*
 * Copyright (c) 2000, компания Крипто-Про
 * 
 * Разрешается повторное распространение и использование как в виде исходного
 * кода, так и в двоичной форме, с изменениями или без, при соблюдении
 * следующих условий:
 * 
 * 1) При повторном распространении исходного кода должно оставаться 
 *    указанное выше уведомление об авторском праве, этот список условий 
 *    и последующий отказ от гарантий.
 * 
 * 2) При повторном распространении двоичного кода должно сохраняться
 *    указанная выше информация об авторском праве, этот список условий
 *    и последующий отказ от гарантий в документации и/или в других материалах,
 *    поставляемых при распространении.
 * 
 * ЭТА ПРОГРАММА ПРЕДОСТАВЛЕНА БЕСПЛАТНО ВЛАДЕЛЬЦАМИ АВТОРСКИХ ПРАВ И/ИЛИ
 * ДРУГИМИ СТОРОНАМИ "КАК ОНА ЕСТЬ" БЕЗ КАКОГО-ЛИБО ВИДА ГАРАНТИЙ, ВЫРАЖЕННЫХ
 * ЯВНО ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ, НО НЕ ОГРАНИЧИВАЯСЬ ИМИ, ПОДРАЗУМЕВАЕМЫЕ
 * ГАРАНТИИ КОММЕРЧЕСКОЙ ЦЕННОСТИ И ПРИГОДНОСТИ ДЛЯ КОНКРЕТНОЙ ЦЕЛИ. НИ В КОЕМ
 * СЛУЧАЕ, ЕСЛИ НЕ ТРЕБУЕТСЯ СООТВЕТСТВУЮЩИМ ЗАКОНОМ, ИЛИ НЕ УСТАНОВЛЕНО В
 * УСТНОЙ ФОРМЕ, НИ ОДИН ВЛАДЕЛЕЦ АВТОРСКИХ ПРАВ И НИ ОДНО ДРУГОЕ ЛИЦО, КОТОРОЕ
 * МОЖЕТ ИЗМЕНЯТЬ И/ИЛИ ПОВТОРНО РАСПРОСТРАНЯТЬ ПРОГРАММУ, КАК БЫЛО СКАЗАНО
 * ВЫШЕ, НЕ НЕСЁТ ОТВЕТСТВЕННОСТИ, ВКЛЮЧАЯ ЛЮБЫЕ ОБЩИЕ, СЛУЧАЙНЫЕ, СПЕЦИАЛЬНЫЕ
 * ИЛИ ПОСЛЕДОВАВШИЕ УБЫТКИ, В СЛЕДСТВИИ ИСПОЛЬЗОВАНИЯ ИЛИ НЕВОЗМОЖНОСТИ
 * ИСПОЛЬЗОВАНИЯ ПРОГРАММЫ (ВКЛЮЧАЯ, НО НЕ ОГРАНИЧИВАЯСЬ ПОТЕРЕЙ ДАННЫХ,
 * ИЛИ ДАННЫМИ, СТАВШИМИ НЕПРАВИЛЬНЫМИ, ИЛИ ПОТЕРЯМИ ПРИНЕСЕННЫМИ ИЗ-ЗА ВАС ИЛИ
 * ТРЕТЬИХ ЛИЦ, ИЛИ ОТКАЗОМ ПРОГРАММЫ РАБОТАТЬ СОВМЕСТНО С ДРУГИМИ ПРОГРАММАМИ),
 * ДАЖЕ ЕСЛИ ТАКОЙ ВЛАДЕЛЕЦ ИЛИ ДРУГОЕ ЛИЦО БЫЛИ ИЗВЕЩЕНЫ О ВОЗМОЖНОСТИ
 * ТАКИХ УБЫТКОВ.
 * 
 * Copyright (c) 2000, Crypto-Pro Company All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1) Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2) Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \file $RCSfile$
 * \version $Revision: 260542 $
 * \date $Date:: 2023-08-15 12:16:30 -0700#$
 * \author $Author: raa $
 *
 * \brief XXX
 *
 * XXX
 */
#ifndef _CSPMM_H_INCLUDED
#define _CSPMM_H_INCLUDED

#ifdef UNIX
    #include <CPROCSP/CSP_WinDef.h>
#else // UNIX
    #include <windef.h>
#endif // UNIX
#include <CPROCSP/WinCryptEx.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CPC_MEMORY_ARENA_ CPC_MEMORY_ARENA, *LPCPC_MEMORY_ARENA;

typedef struct CPC_MEMORY_STATS_ {
    LONG nBytes;
    LONG nChunks;
    LONG Used;
    LONG Size;
} CPC_MEMORY_STATS, *LPCPC_MEMORY_STATS;

typedef DWORD CPCAPI CPC_AllocMemory_Callback(
    LPCPC_MEMORY_ARENA pArena, 
    CPC_SIZE_T dwSize,
    DWORD dwMemPoolId,
    DWORD dwThreadId,
    LPVOID *pRes
);

typedef DWORD CPCAPI CPC_FreeMemory_Callback(
    LPCPC_MEMORY_ARENA pArena,
    VOID *pMem,
    DWORD dwMemPoolId
);

typedef VOID CPCAPI CPC_StatMemory_Callback(
    LPCPC_MEMORY_ARENA pArena,
    LPCPC_MEMORY_STATS pStats,
    DWORD dwMemPoolId
);

typedef VOID CPCAPI CPC_ValidateMemory_Callback(
    LPCPC_MEMORY_ARENA pArena
);

typedef VOID CPCAPI CPC_DoneMemory_Callback(
    LPCPC_MEMORY_ARENA pArena
);

typedef VOID CPCAPI CPC_MemoryException_Callback(
    LPCPC_MEMORY_ARENA pArena,
    VOID *arg
);

struct CPC_MEMORY_ARENA_ {
    CPC_ValidateMemory_Callback     *pValidateMemory;
    CPC_DoneMemory_Callback	    *pDoneMemory;
    CPC_AllocMemory_Callback	    *pAllocMemory;
    CPC_FreeMemory_Callback	    *pFreeMemory;
    CPC_StatMemory_Callback	    *pStatMemory;
    CPC_MemoryException_Callback    *pHandleException;
    LPVOID			    lpArg;
};

/* Идентификаторы рабочих куч библиотеки (memory pool id)
   для случая, когда только ключи помещаются в MP_PRIME и 
   только маски помещаются в MP_SEC*/
#define MP_WORK		3
#define MP_PRIME	MP_WORK
#define MP_SEC		MP_WORK
#define MP_PRIME_M	4
#define MP_SEC_M	5
#define MP_WORK_M	6
#if defined ( _WIN32 ) || defined ( _WIN64 )
#define MP_BIG		7
#else
#define MP_BIG		MP_WORK
#endif /* defined ( _WIN32 ) || defined ( _WIN64 ) */

#define NO_ZERO_MEMORY		(0x80000000)

#ifdef __cplusplus
}
#endif

#endif /* _CSPMM_H_INCLUDED */
