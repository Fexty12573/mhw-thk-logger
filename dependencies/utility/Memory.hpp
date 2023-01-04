#pragma once
#ifndef __MEMORY_H__
#define __MEMORY_H__

// ==================== Header ====================
// Author:		Fexty12573
// Date:		08/26/2020
// Description: A header file for memory editing with C++. Main focus is MHW.
//				Has functions for both Internal and External use.
// ================== Header End ==================
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <time.h>

namespace memory
{
	typedef unsigned int uint;
	typedef unsigned char byte;
	typedef unsigned long long QWORD;

	// =================== Internal functions ===================
	// To be used in a .dll injected into the target process
	// ==========================================================

	static void TriplePatch(void* dst1, void* dst2, void* dst3, std::vector<byte> src1, std::vector<byte> src2, std::vector<byte> src3);
	static uintptr_t GetPtrAddr(uintptr_t basePtr, std::vector<uint> offsets);
	static void Patch(void* dst, const std::vector<byte>& src);
	static void Nop(void* dst, uint len);

	// ========================= Other ==========================
	// General Use Functions not specific to anything.
	// ==========================================================

	static uint RandomNum(uint num = 10);
	template<class _ValueType>
	void ToByteArray(_ValueType value, byte* buf);

	// =================== External functions ===================
	// To be used in an exe. Modifying its memory externally
	// ==========================================================

	static uintptr_t GetModuleBaseEx(DWORD pid, const wchar_t* modName);
	template<class _RetType, class _BaseType> _RetType ReadPointerEx(HANDLE hProc, _BaseType pBasePtr, std::vector<int> offsets);
	static void PatchEx(HANDLE hProc, void* dst, std::vector<byte> src);
	static void NopEx(HANDLE hProc, void* dst, unsigned int len);
	static std::pair<HANDLE, DWORD> GetProcessHandleAndPId(const char* pWindowName, DWORD dwDesiredAccess = PROCESS_ALL_ACCESS)
	{
		HWND hWnd = NULL;
		DWORD pid = NULL;
		HANDLE hProc = NULL;

		hWnd = FindWindowA(NULL, pWindowName);
		GetWindowThreadProcessId(hWnd, &pid);
		hProc = OpenProcess(dwDesiredAccess, false, pid);
		
		return std::make_pair(hProc, pid);
	}

	// ================== Function Definitions ==================
	// Definitions for both internal and external functions.
	// See how the code works here.
	// ==========================================================

	// Writes to 3 locations with 3 different sources.
	// Usage: Provide 3 addresses, provide 3 instructions
	static void TriplePatch(void* dst1, void* dst2, void* dst3, std::vector<byte> src1, std::vector<byte> src2, std::vector<byte> src3)
	{
		DWORD prot1, prot2, prot3;

		VirtualProtect((LPVOID)(dst1), src1.size(), PAGE_EXECUTE_READWRITE, &prot1);
		VirtualProtect((LPVOID)(dst2), src1.size(), PAGE_EXECUTE_READWRITE, &prot2);
		VirtualProtect((LPVOID)(dst3), src1.size(), PAGE_EXECUTE_READWRITE, &prot3);

		memcpy(dst1, src1.data(), src1.size());
		memcpy(dst2, src2.data(), src2.size());
		memcpy(dst3, src3.data(), src3.size());

		VirtualProtect((LPVOID)(dst1), src1.size(), prot1, &prot1);
		VirtualProtect((LPVOID)(dst2), src1.size(), prot2, &prot2);
		VirtualProtect((LPVOID)(dst3), src1.size(), prot3, &prot3);
	}

	// Used to calculate a Pointer from a base pointer and a list of offsets
	// Usage: Provide base pointer as <module base + offset>, provide list of offsets as vector { offset }
	template<class T>
	T GetPtrAddr(uintptr_t basePtr, std::vector<uint> offsets)
	{
		uintptr_t addr = basePtr;
		for (uint i : offsets) {
			addr = *(uintptr_t*)addr;
			addr += i;
		}
		return (T)addr;
	}

	// Read multilevel Pointer and return address of it
	template<class _Ty>
	_Ty ReadPointer(QWORD base_address, std::vector<unsigned int> offsets)
	{
		QWORD address = base_address;

		for (int offset : offsets)
		{
			address = (*(QWORD*)address) + offset;
		}

		return (_Ty)address;
	}

	static uintptr_t GetPtrAddr(uintptr_t basePtr, std::vector<uint> offsets)
	{
		uintptr_t addr = basePtr;
		for (uint i = 0; i < offsets.size(); ++i) {
			addr = *(uintptr_t*)addr;
			addr += offsets[i];
		}
		return addr;
	}

	template<class T>
	T GetMonsterObjectValue(void* monster_ptr, std::vector<uint> offsets)
	{
		QWORD addr = (QWORD)monster_ptr;
		for (auto& offset : offsets)
		{
			addr += offset;
			addr = *(QWORD*)addr;
		}

		return (T)addr;
	}

	// Write to 1 location
	// Usage: Provide address and instruction
	static void Patch(void* dst, const std::vector<byte>& src)
	{
		DWORD prot;
		VirtualProtect(dst, src.size(), PAGE_EXECUTE_READWRITE, &prot);
		memcpy(dst, src.data(), src.size());
		VirtualProtect(dst, src.size(), prot, &prot);
	}

	template<size_t Size> static void Patch(void* dst, const std::array<byte, Size>& src)
	{
		DWORD prot;
		VirtualProtect(dst, src.size(), PAGE_EXECUTE_READWRITE, &prot);
		memcpy(dst, src.data(), src.size());
		VirtualProtect(dst, src.size(), prot, &prot);
	}

	static void Patch(void* dst, byte* src, size_t srclen)
	{
		DWORD prot;
		VirtualProtect(dst, srclen, PAGE_EXECUTE_READWRITE, &prot);
		memcpy(dst, src, srclen);
		VirtualProtect(dst, srclen, prot, &prot);
	}

	static void WriteByte(void* dst, byte src)
	{
		DWORD prot;
		VirtualProtect(dst, 1, PAGE_EXECUTE_READWRITE, &prot);
		*reinterpret_cast<byte*>(dst) = src;
		VirtualProtect(dst, 1, prot, &prot);
	}

	// 'Nop' a location with a specific size
	// Usage: Provide address and the length in bytes to nop
	static void Nop(void* dst, uint len)
	{
		DWORD prot;
		VirtualProtect(dst, len, PAGE_EXECUTE_READWRITE, &prot);
		memset(dst, 0x90, len);
		VirtualProtect(dst, len, prot, &prot);
	}

	// Generates a random integer between 1 and num.
	// Usage: Provide any number
	static uint RandomNum(uint num)
	{
		srand((unsigned)time(0));
		int number;
		for (uint i = 0; i < 10; i++) {
			number = rand() % num + 1;
		}
		return number;
	}

	// Gets the base address of a module in a process
	// Usage: Provide process id and the module name. (Ex: "MonsterHunterWorld.exe" or "steam_api64.dll" are modules)
	static uintptr_t GetModuleBaseEx(DWORD pid, const wchar_t* modName)
	{
		uintptr_t modBase = 0;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
		if (hSnap != INVALID_HANDLE_VALUE) {
			MODULEENTRY32W ME32 = { 0 };
			ME32.dwSize = sizeof(ME32);
			if (Module32FirstW(hSnap, &ME32)) {
				do {
					if (wcscmp(ME32.szModule, modName) == 0) {
						modBase = (uintptr_t)ME32.modBaseAddr;
						break;
					}
				} while (Module32NextW(hSnap, &ME32));
			}
			CloseHandle(hSnap);
		}
		return modBase;
	}

	// Same as Internal version
	// Usage: Same as Internal version, now also needs HANDLE to process with minimum rights PROCESS_VM_READ
	template<class _RetType, class _BaseType>
	_RetType ReadPointerEx(HANDLE hProc, _BaseType pBasePtr, std::vector<int> offsets)
	{
		QWORD addr = (QWORD)pBasePtr;
		for (int i = 0; i < offsets.size(); ++i)
		{
			ReadProcessMemory(hProc, (LPVOID)addr, &addr, sizeof(addr), 0);
			addr += offsets[i];
		}

		return (_RetType)addr;
	}

	// Writes to a location
	// Needs HANDLE to process with minimum access rights PROCESS_VM_WRITE
	static void PatchEx(HANDLE hProc, void* dst, std::vector<byte> src)
	{
		WriteProcessMemory(hProc, dst, &src[0], src.size(), 0);
	}

	// 'Nops' a location for a specified length
	// Needs HANDLE to process with minimum access rights PROCESS_VM_WRITE
	static void NopEx(HANDLE hProc, void* dst, unsigned int len)
	{
		byte* patch = new byte[len];
		memset(patch, 0x90, len);
		WriteProcessMemory(hProc, dst, patch, len, 0);
	}

	template<class _Ty>
	void WriteValue(HANDLE hProc, void* dst, _Ty value)
	{
		WriteProcessMemory(hProc, dst, &value, sizeof(_Ty), NULL);
	}

	// Pass value to be converted and buffer to store the given byte afterwards
	template<class _ValueType>
	inline void ToByteArray(_ValueType value, byte* buf)
	{
		union {
			_ValueType val;
			byte bytes[sizeof(value)];
		} u;

		u.val = value;

		std::memcpy(buf, u.bytes, sizeof(value));
	}
}

#endif // !__MEMORY_H__