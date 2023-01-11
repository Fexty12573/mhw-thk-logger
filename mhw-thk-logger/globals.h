#pragma once

typedef signed int s32;
typedef unsigned int u32;

static s32 g_MonsterFilter = -1;
static bool g_LogSegments = true;
static bool g_LogRegisters = true;
static bool g_PauseOnCrash = true;
static std::map<void*, s32> g_Monsters;
static std::map<void*, s32> g_ThkMonsters;
static int g_MonsterIx = -1;
