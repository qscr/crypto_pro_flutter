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

/*
 * \file $RCSfile$
 * \version $Revision: 260531 $
 * \date $Date:: 2023-08-15 08:00:42 -0700#$
 * \author $Author: raa $
 */
/*!
 * \brief Интерфейс безблокировочного (lock-free) менеджера памяти.
 *
 * Содержит прототипы функций безблокировочного менеджера памяти уровня CPC.
 */
#ifndef _LFMM_H_INCLUDED
#define _LFMM_H_INCLUDED

#include <wincspc.h>

#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)

typedef struct CPC_LFMM_CONFIG_ {
    CPC_INTERLOCKED_FUNCS   interlockedFuncs;
    LPVOID		    Buffer;
    LONG		    Size;
    CSP_BOOL		    fSMP;
    CSP_BOOL		    fStat;
    LONG 		    *PoolSizes;
    DWORD		    nPools;
    DWORD		    nCPUs;
} CPC_LFMM_CONFIG, * LPCPC_LFMM_CONFIG;

typedef DWORD CPCAPI CPCInitMemoryLF_t(
    /* [out] */ LPCPC_MEMORY_ARENA* pArena,
    /* [in] */ LPCPC_LFMM_CONFIG pCfg
);

#if !defined(CSP_LITE) || defined(LINUX) || defined(SOLARIS)
	// При использовании библиотек, а так же в Linux и Solaris
	// Символы доступны прямо.
#elif defined(FREEBSD)
    // ?????
    #define LFMM_SEPARATE_NAMESPACE_BINDING 1
#elif !defined(UNIX)
    // #include "cpdrvlib.h"
    // IOCTL_GETCPCINITMEMORYLF
    #define LFMM_SEPARATE_NAMESPACE_BINDING 1
#endif

#if !defined(LFMM_SEPARATE_NAMESPACE_BINDING)
    CPCInitMemoryLF_t CPCInitMemoryLF;
#endif

#if defined(__cplusplus)
} // extern "C"
#endif // defined(__cplusplus)

#endif /* _LFMM_H_INCLUDED */
