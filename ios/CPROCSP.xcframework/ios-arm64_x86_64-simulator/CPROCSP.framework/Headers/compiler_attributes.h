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

#if !defined _COMPILER_ATTRIBUTES_H_
#define _COMPILER_ATTRIBUTES_H_

#if !defined __GNUC__
#   define ATTR_NOASAN
#   define ATTR_NOTSAN
#   define ATTR_NORETURN
#   define ATTR_USERES
#if defined _WIN32
#   define CP_FORCE_INLINE __inline
#else
#   define CP_FORCE_INLINE inline
#endif	/* _WIN32 */
#   define DLL_LOCAL /* dim: для не-gcc отдельно посмотрю, если для gcc получится хорошо */
#   define DLL_PUBLIC
#else

/* CPCSP-11164 */
/* 2020-jun-11 dim: после отказа от gcc-3.2/CheckPoint убрать #if */
#if ((__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || __GNUC__ >= 4)
#   define DLL_LOCAL  __attribute__((visibility("hidden")))
#   define DLL_PUBLIC __attribute__((visibility("default")))
#else
#   define DLL_LOCAL
#   define DLL_PUBLIC
#endif

/* CPCSP-8892 dim: gcc-4.8 не меняется, чтобы не менялась сборка */
#if !((__GNUC__ == 4 && __GNUC_MINOR__ >= 8) || __GNUC__ >= 5)
#   define ATTR_NOASAN
#   define ATTR_NOTSAN
#   define ATTR_NORETURN
#   define ATTR_USERES
#   define CP_FORCE_INLINE inline
#else

/* наши реализации mem*() и str*() вызывают ошибки с -fsanitize=address */
#if defined __SANITIZE_ADDRESS__
# define ATTR_NOASAN __attribute__((no_sanitize_address))
#else
# define ATTR_NOASAN
#endif	/* __SANITIZE_ADDRESS__ */

/* распределитель хэндлов вызывает сообщения, про которые и так известно */
#if defined __SANITIZE_THREAD__
# define ATTR_NOTSAN __attribute__((no_sanitize_thread))
#else
# define ATTR_NOTSAN
#endif	/* __SANITIZE_THREAD__ */

#   define ATTR_NORETURN     __attribute__((noreturn))
#   define ATTR_USERES       __attribute__((warn_unused_result))
#   define CP_FORCE_INLINE   inline __attribute__ ((__always_inline__))
#endif	/* не новый gcc */
#endif	/* не gcc */
#endif	/* _COMPILER_ATTRIBUTES_H_ */
