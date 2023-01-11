#pragma once

typedef signed int s32;
typedef unsigned int u32;

inline s32 g_MonsterFilter = -1;
inline bool g_LogSegments = true;
inline bool g_LogRegisters = false;
inline bool g_PauseOnCrash = true;
inline std::map<void*, s32> g_Monsters;
inline std::map<void*, s32> g_ThkMonsters;
inline int g_MonsterIx = -1;
