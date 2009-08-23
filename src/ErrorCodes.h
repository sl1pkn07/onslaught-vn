/*
* Copyright (c) 2008, 2009, Helios (helios.vmg@gmail.com)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, 
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of the author may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*     * Products derived from this software may not be called "ONSlaught" nor
*       may "ONSlaught" appear in their names without specific prior written
*       permission from the author. 
*
* THIS SOFTWARE IS PROVIDED BY HELIOS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL HELIOS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef NONS_ERROR_CODES
#define NONS_ERROR_CODES

#ifndef TOOLS_BARE_FILE
#include "CommonTypes.h"

typedef Uint32 ErrorCode;
#else
typedef unsigned long ErrorCode;
#endif

#define NONS_FATAL_ERROR							0x40000000
#define NONS_BREAK_WORTHY_ERROR						0x20000000
#define NONS_NO_ERROR_FLAG							0x10000000
#define NONS_INTERNAL_ERROR							(0x08000000|NONS_FATAL_ERROR)
#define NONS_SYNTAX_ERROR							0x04000000
#define NONS_WARNING								0x02000000
#define NONS_END									(0x01000000|NONS_NO_ERROR_FLAG)
#define NONS_UNDEFINED_ERROR						0x00010000

#define NONS_NO_ERROR								(0|NONS_NO_ERROR_FLAG)
#define NONS_INVALID_PARAMETER						1
#define NONS_INVALID_ARCHIVE						2
#define NONS_ARCHIVE_UNINIT							3
#define NONS_NOT_IMPLEMENTED						4
#define NONS_NOT_IN_DEFINE_MODE						5
#define NONS_DUPLICATE_CONSTANT_DEFINITION			6
#define NONS_ALREADY_INITIALIZED					7
#define NONS_NO_SUCH_BLOCK							8
#define NONS_NO_ERROR_BUT_BREAK						(9|NONS_NO_ERROR_FLAG|NONS_BREAK_WORTHY_ERROR)
#define NONS_EMPTY_CALL_STACK						(10|NONS_FATAL_ERROR)
#define NONS_UNDEFINED_SYNTAX_ERROR					(11|NONS_SYNTAX_ERROR)
#define NONS_DIVISION_BY_ZERO						12
#define NONS_NON_INTEGRAL_VARIABLE_IN_EXPRESSION	13
#define NONS_UNDEFINED_CONSTANT						14
#define NONS_UNRECOGNIZED_COMMAND					15
#define NONS_UNMATCHING_OPERANDS					16
#define NONS_INVALID_ID_NAME						17
#define NONS_INSUFFICIENT_PARAMETERS				18
#define NONS_FILE_NOT_FOUND							19
#define NONS_NO_MUSIC_LOADED						(20|NONS_WARNING)
#define NONS_NO_SOUND_EFFECT_LOADED					(21|NONS_WARNING)
#define NONS_INTERNAL_INVALID_PARAMETER				(22|NONS_INTERNAL_ERROR)
#define NONS_DUPLICATE_EFFECT_DEFINITION			23
#define NONS_INVALID_RUNTIME_PARAMETER_VALUE		24
#define NONS_UNMATCHED_BRAKETS						(25|NONS_SYNTAX_ERROR)
#define NONS_UNRECOGNIZED_OPERATOR					(26|NONS_SYNTAX_ERROR)
#define NONS_ARRAY_INDEX_OUT_OF_BOUNDS				27
#define NONS_MISSING_Q_IN_ARRAY_DECLARATION			(28|NONS_SYNTAX_ERROR)
#define NONS_MISSING_B_IN_ARRAY_DECLARATION			(29|NONS_SYNTAX_ERROR)
#define NONS_EXPECTED_NUMERIC_VARIABLE				30
#define NONS_TOO_MANY_PARAMETERS					(31|NONS_WARNING)
#define NONS_NO_JUMPS								32
#define NONS_UNMATCHED_QUOTES						(33|NONS_SYNTAX_ERROR)
#define NONS_ZERO_VALUE_IN_SKIP						34
#define NONS_EFFECT_CODE_OUT_OF_RANGE				35
#define NONS_EMPTY_STRING							36
#define NONS_INVALID_CHANNEL_INDEX					37
#define NONS_SCREEN_UNINIT							38
#define NONS_SCRIPT_NOT_FOUND						39
#define NONS_INI_SECTION_NOT_FOUND					40
#define NONS_INI_KEY_NOT_FOUND						41
#define NONS_INVALID_HEX							42
#define NONS_UNIMPLEMENTED_COMMAND					43
#define NONS_NO_EFFECT								44
#define NONS_UNDEFINED_EFFECT						45
#define NONS_UNEXPECTED_NEXT						(46|NONS_FATAL_ERROR)
#define NONS_NO_NEXT								47
#define NONS_TRANSPARENCY_METHOD_UNIMPLEMENTED		48
#define NONS_NO_TRAP_SET							49
#define NONS_MENU_UNINITIALIZED						50
#define NONS_NO_BUTTON_IMAGE						51
#define NONS_NO_BUTTONS_DEFINED						52
#define NONS_SELECT_TOO_BIG							53
#define NONS_NO_START_LABEL							(54|NONS_FATAL_ERROR)
#define NONS_GOSUB									(55|NONS_NO_ERROR_FLAG|NONS_BREAK_WORTHY_ERROR)
#define NONS_NO_SPRITE_LOADED_THERE					56
#define NONS_INVALID_TRANSPARENCY_METHOD			57
#define NONS_UNSUPPORTED_SAVEGAME_VERSION			58
#define NONS_UNDOCUMENTED_COMMAND					59
#define NONS_EXPECTED_VARIABLE						60
#define NONS_EXPECTED_STRING_VARIABLE				61
#define NONS_EXPECTED_SCALAR						62
#define NONS_EXPECTED_ARRAY							63
#define NONS_VARIABLE_OUT_OF_RANGE					64
#define NONS_UNDEFINED_ARRAY						65
#define NONS_OUT_OF_BOUNDS							66
#define NONS_NO_DEFINE_LABEL						(67|NONS_FATAL_ERROR)
#define NONS_INSUFFICIENT_DIMENSIONS				68
#define NONS_TOO_MANY_DIMENSIONS					69
#define NONS_ILLEGAL_ARRAY_SPECIFICATION			(70|NONS_SYNTAX_ERROR)
#define NONS_NEGATIVE_INDEX_IN_ARRAY_DECLARATION	71
#define NONS_LEXICALLY_UNCASTABLE					(72|NONS_SYNTAX_ERROR)
#define NONS_DUPLICATE_LABEL						(73|NONS_SYNTAX_ERROR)
#define NONS_INVALID_COMMAND_NAME					(74|NONS_SYNTAX_ERROR)
#define NONS_NOT_ENOUGH_LINES_TO_SKIP				75
#define NONS_BAD_MATRIX								(76|NONS_WARNING)
#define NONS_NOT_ENOUGH_VARIABLE_INDICES			77
#define NONS_NO_SUCH_SAVEGAME						(78|NONS_WARNING)
#define NONS_HASH_DOES_NOT_MATCH					79
#define NONS_DUPLICATE_COMMAND_DEFINITION_BUILTIN	80
#define NONS_DUPLICATE_COMMAND_DEFINITION_USER		81
#define NONS_NOT_IN_A_USER_COMMAND_CALL				82

extern const char *errorMessages[];
#endif
