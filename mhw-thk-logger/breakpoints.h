#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <cwctype>
#include <algorithm>
#include <set>
#include <locale>
#include <dti/rThinkTable.h>

using bp_triplet = std::tuple<s32, s32, s32>;
inline bool g_Pause = false;
inline std::map<s32, std::set<bp_triplet>> g_MonsterBreakpoints;


inline void setTime(float scale)
{
	uint64_t* sMhUnit = (uint64_t*)0x145183e00;
	float* timescale = (float*)(((byte*)(*sMhUnit)) + 0xF8 * 8 + 0x9C);
	*timescale = scale;
}

inline void pause()
{
	setTime(0);
	//g_Pause = true;
}

inline void unpause()
{
	setTime(1);
	//g_Pause = false;
}
std::string wide_string_to_string(const std::wstring& wide_string);
std::vector<std::string> python_split(std::wstring wstr, const char c);
bp_triplet split_breakpoint_arguments(std::wstring s);
void add_breakpoint(bp_triplet t);
void missing_breakpoint(const char* monster_name, bp_triplet f);
void del_breakpoint(bp_triplet t);
void show_breakpoint();
void manage_breakpoint(std::wstring s);
void check_breakpoints(int, int, int, int);