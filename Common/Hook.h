﻿#pragma once

//---------------------------------------------------------------------

#define DECLARE_INTERNAL_PROC(resultType, callType, procName, args) \
	typedef resultType (callType *Type_##procName) args; \
	extern Type_##procName procName

#define IMPLEMENT_INTERNAL_PROC(resultType, callType, procName, args) \
	Type_##procName procName = 0

#define DECLARE_HOOK_PROC(resultType, callType, procName, args) \
	typedef resultType (callType *Type_##procName) args; \
	extern Type_##procName true_##procName; \
	resultType callType hook_##procName args

#define IMPLEMENT_HOOK_PROC(resultType, callType, procName, args) \
	Type_##procName true_##procName = procName; \
	resultType callType hook_##procName args

#define IMPLEMENT_HOOK_PROC_NULL(resultType, callType, procName, args) \
	Type_##procName true_##procName = NULL; \
	resultType callType hook_##procName args

#define GET_INTERNAL_PROC(module, procName) \
	procName = (Type_##procName)::GetProcAddress(module, #procName)

#define GET_HOOK_PROC(module, procName) \
	true_##procName = (Type_##procName)::GetProcAddress(module, #procName)

#define ATTACH_HOOK_PROC(name) DetourAttach((PVOID*)&true_##name, hook_##name)
#define DETACH_HOOK_PROC(name) DetourDetach((PVOID*)&true_##name, hook_##name)

#define DETACH_HOOK_PROC2(name) \
do \
{ \
	DetourTransactionBegin(); \
	DetourUpdateThread(::GetCurrentThread()); \
 \
	DETACH_HOOK_PROC(name); \
 \
	if (DetourTransactionCommit() == NO_ERROR) \
	{ \
		MY_TRACE(_T(#name) _T(" の解除に成功しました\n")); \
	} \
	else \
	{ \
		MY_TRACE(_T(#name) _T(" の解除に失敗しました\n")); \
	} \
} \
while (0)

//---------------------------------------------------------------------

// コードを書き換える。
inline void writeCode(DWORD address, const BYTE* code, int c)
{
	// コードを書き換える。そのあと命令キャッシュをフラッシュする。
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, code, c, NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, c);
}

// 相対 CALL を書き換える。
template<class T>
inline T hookCall(DWORD address, T hookProc)
{
	BYTE code[5];
	code[0] = 0xE8; // CALL
	*(DWORD*)&code[1] = (DWORD)hookProc - (address + 5);

	// 元の関数を取得する。
	DWORD retValue = 0;
	::ReadProcessMemory(::GetCurrentProcess(), (LPVOID)(address + 1), &retValue, sizeof(retValue), NULL);
	retValue += (address + 5);

	// CALL を書き換える。そのあと命令キャッシュをフラッシュする。
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, code, sizeof(code), NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(code));

	// 元の関数を返す。
	return (T)retValue;
}

// 絶対 CALL を書き換える。
template<class T>
inline T hookAbsoluteCall(DWORD address, T hookProc)
{
	BYTE code[6];
	code[0] = 0xE8; // CALL
	*(DWORD*)&code[1] = (DWORD)hookProc - (address + 5);
	code[5] = 0x90; // NOP

	// 元の関数を取得する。
	DWORD retValue = 0;
	::ReadProcessMemory(::GetCurrentProcess(), (LPVOID)(address + 2), &retValue, sizeof(retValue), NULL);

	// CALL を書き換える。そのあと命令キャッシュをフラッシュする。
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, code, sizeof(code), NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(code));

	// 元の関数を返す。
	return (T)retValue;
}

// 絶対アドレスを書き換える。
template<class T>
inline T writeAbsoluteAddress(DWORD address, T x)
{
	// 元の値を取得する。
	T retValue = 0;
	::ReadProcessMemory(::GetCurrentProcess(), (LPVOID)address, &retValue, sizeof(retValue), NULL);

	// 絶対アドレスを書き換える。そのあと命令キャッシュをフラッシュする。
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, &x, sizeof(x), NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(x));

	// 元の値を返す。
	return retValue;
}

// アドレスをキャストする。
template<class T>
inline void castAddress(T& x, DWORD address)
{
	x = (T)address;
}

//---------------------------------------------------------------------
