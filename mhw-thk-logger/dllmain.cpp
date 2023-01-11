#include <Windows.h>
#include <string>
#include <map>
#include <cwctype>
#include <algorithm>
#include <locale>
#include <codecvt>

#include <utility/util.h>
#include <utility/game_functions.h>
#include <utility/ghidra_export.h>
#include <monster/monster.h>
#include <loader/loader.h>

#include <dti/cThinkMgr.h>
#include <dti/rThinkTable.h>
#include <fmt/format.h>

#include <breakpoints.h>
#include <globals.h>
#include <symbols.h>

using namespace loader;


CreateHook(MH::Monster::ctor, ctor, void*, void* thisptr, mh::Monster::ID id, u32 sub)
{
	g_Monsters[thisptr] = -1;
	g_ThkMonsters[thisptr] = -1;
	return original(thisptr, id, sub);
}
CreateHook(MH::Monster::dtor, dtor, void, void* thisptr)
{
	g_Monsters.erase(thisptr);
	g_ThkMonsters.erase(thisptr);
	return original(thisptr);
}

std::tuple<int, int> get_indices(THK::Segment *segment, THK *thk) {
	for (u32 i = 0; i < thk->mHeader.mNodeCount; i++)
	{
		const auto& info = thk->mHeader.mNodeInfoList[i];

		for (u32 j = 0; j < info.mSegmentCount; j++)
		{
			if (&info.mNode[j] == segment)
			{
				return { i,j };
			}
		}
	}
	return { -1,-1 };
}

CreateHook(MH::Monster::ProcessTHKSegment, ProcessTHKSegment, int, cThinkMgr* thisptr, int* out, THK::Segment* segment)
{
	void* m = thisptr->GetParent();
	if (g_Monsters.contains(m))
	{
		if (g_MonsterFilter == -1 || g_MonsterFilter == *OffsetPointer<int*>(m, 0x12280))
		{
			mh::Monster mon(*OffsetPointer<mh::Monster::ID*>(m, 0x12280), *OffsetPointer<int*>(m, 0x12288), m);

			const void* aidata = mon.at<void*>(0x12278);
			rThinkTable* thinktable = *OffsetPointer<rThinkTable**>(aidata, 0xAB0);
			rThinkTable* thk55 = rThinkTable::GetThinkTableAtIndex(thisptr->GetTHKList(), 55);

			const THK::Header& header = thk55->mTHK->mHeader;
			const THK::Segment* veryFirst = header.mNodeInfoList[0].mNode;
			const THK::Segment* veryLast = header.mNodeInfoList[header.mNodeCount - 1].mNode
				+ header.mNodeInfoList[header.mNodeCount - 1].mSegmentCount;

			const auto path = thinktable->mPath;

			u32 emId = 0, subId = 0, thkId = 0;
			sscanf_s(path, R"(em\em%03u\%02u\data\em%03u_%02u)", &emId, &subId, &emId, &thkId);

			if (segment >= veryFirst && segment <= veryLast) // is THK55
			{
				thkId = 55;
				thinktable = thk55;
			}

			const auto thk = thinktable->mTHK;
			auto [nodeIndex, segmentIndex] = get_indices(segment,thk);

			if (nodeIndex == -1)
			{
				LOG(INFO) << fmt::format("Failed to get node index {}, THK({})", static_cast<void*>(segment), static_cast<void*>(thinktable));
				if (g_PauseOnCrash) {
					pause();//mon.at<byte>(0x14) &= 0xFE;
				}
				return original(thisptr, out, segment);
			}
			//LOG(INFO) << fmt::format("{} {} {}", (int) mon.m_id, thkId, nodeIndex);
			symbol_check((int) mon.m_id, (int) thkId, (int) nodeIndex, (int) segmentIndex, m,
								(int) g_MonsterIx, (int) g_ThkMonsters.at(m), (int)g_Monsters.at(m));
			if (g_LogRegisters && g_LogSegments) {
				void* cThinkEm = thisptr->GetcThinkEm();
				symbol_check_registers(cThinkEm);
			}
			g_Monsters[m] = nodeIndex;
			g_ThkMonsters[m] = thkId;
			g_MonsterIx = (int) mon.m_id;
			//LOG(INFO) << fmt::format("{} {} {}", (int) mon.m_id ,thkId, nodeIndex);
			//LOG(INFO) << "";

			//if (g_Pause) {
			//	mon.at<byte>(0x14) &= 0xFE;
			//}
			//else {
			//	mon.at<byte>(0x14) |= 1;
			//}
		}
	}

	return original(thisptr, out, segment);
}

void monster_filter(std::wstring s) 
{
	swscanf_s(s.data(), L"/thk filter %d", &g_MonsterFilter);
	if (g_MonsterFilter == -1)
	{
		auto msg = "Disabled Monster Filter";
		MH::Chat::DisplayMessage(msg);
		LOG(INFO) << msg;
	}
	else
	{
		const auto msg = fmt::format("Monster Filter set to {0}", mh::Monster::Names.at(static_cast<mh::Monster::ID>(g_MonsterFilter)));
		MH::Chat::DisplayMessage(msg);
		LOG(INFO) << msg;
	}
}

void segment_filter(std::wstring s)
{
	if (s.size() <= 14) {
		g_LogSegments = !g_LogSegments;
		if (g_LogSegments)MH::Chat::DisplayMessage("Enabled Logging Segments");
		else MH::Chat::DisplayMessage("Disabled Logging Segments");
	}
	else if (s.substr(14) == L"true")
	{
		g_LogSegments = true;
		MH::Chat::DisplayMessage("Enabled Logging Segments");
	}
	else if (s.substr(14) == L"false")
	{
		g_LogSegments = false;
		MH::Chat::DisplayMessage("Disabled Logging Segments");
	}
}

void register_filter(std::wstring s)
{
	if (s.size() <= 15) {
		g_LogRegisters = !g_LogRegisters;
		if (g_LogSegments)MH::Chat::DisplayMessage("Enabled Logging Registers");
		else MH::Chat::DisplayMessage("Disabled Logging Registers");
	}
	else if (s.substr(15) == L"true")
	{
		g_LogRegisters = true;
		MH::Chat::DisplayMessage("Enabled Logging Registers");
	}
	else if (s.substr(15) == L"false")
	{
		g_LogRegisters = false;
		MH::Chat::DisplayMessage("Disabled Logging Registers");
	}
}

void pause_filter(std::wstring s)
{
	if (s.size() <= 17) {
		g_PauseOnCrash = !g_PauseOnCrash;
		if (g_LogSegments)MH::Chat::DisplayMessage("Enabled Pause On Crash");
		else MH::Chat::DisplayMessage("Disabled Pause On Crash");
	}
	else if (s.substr(17) == L"true")
	{
		g_PauseOnCrash = true;
		MH::Chat::DisplayMessage("Enabled Pause On Crash");
	}
	else if (s.substr(17) == L"false")
	{
		g_PauseOnCrash = false;
		MH::Chat::DisplayMessage("Disabled Pause On Crash");
	}
}

CreateHook(MH::sMhInputText::Dispatch, Dispatch, void, const wchar_t* str)
{
	std::wstring s(str);
	
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned short c) { return std::towlower(c); });
	if (s.starts_with(L"/thk filter"))
	{
		monster_filter(s);
	}
	else if (s.starts_with(L"/thk segments"))
	{
		segment_filter(s);
	}
	else if (s.starts_with(L"/thk registers"))
	{
		register_filter(s);
	}
	else if (s.starts_with(L"/thk pause"))
	{
		pause();
	}
	else if (s.starts_with(L"/thk unpause"))
	{
		unpause();
	}
	else if (s.starts_with(L"/thk crash_pause"))
	{
		pause_filter(s);
	}
	else if (s.starts_with(L"/thk bp"))
	{
		manage_breakpoint(s);
	}
	else if (s.starts_with(L"/thk symbols"))
	{
		load_symbols();
	}
	return original(str);
}


static bool DllInit()
{
	MH_Initialize();
	QueueHook(Dispatch);
	QueueHook(ProcessTHKSegment);
	QueueHook(ctor);
	QueueHook(dtor);
	MH_ApplyQueued();

	return true;
}

BOOL APIENTRY DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DllInit();
	}

	return TRUE;
}
