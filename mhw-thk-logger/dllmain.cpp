#include <Windows.h>
#include <string>
#include <map>
#include <cwctype>
#include <algorithm>

#include <utility/util.h>
#include <utility/game_functions.h>
#include <utility/ghidra_export.h>
#include <monster/monster.h>
#include <loader/loader.h>

#include <dti/cThinkMgr.h>
#include <dti/rThinkTable.h>
#include <fmt/format.h>

static s32 g_MonsterFilter = -1;
static bool g_LogSegments = false;
static std::map<void*, s32> g_Monsters;

void DisplayMessage(const std::string& msg)
{
	MH::Chat::ShowGameMessage(*MH::Chat::MainPtr, msg.c_str(), -1, -1, false);
}

using namespace loader;

CreateHook(MH::Monster::ctor, ctor, void*, void* thisptr, mh::Monster::ID id, u32 sub)
{
	g_Monsters[thisptr] = -1;
	return original(thisptr, id, sub);
}
CreateHook(MH::Monster::dtor, dtor, void, void* thisptr)
{
	g_Monsters.erase(thisptr);
	return original(thisptr);
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

			s32 nodeIndex = -1;
			s32 segmentIndex = -1;
			if (segment >= veryFirst && segment <= veryLast) // is THK55
			{
				thkId = 55;
				thinktable = thk55;
			}

			const auto thk = thinktable->mTHK;
			for (u32 i = 0; i < thk->mHeader.mNodeCount; i++)
			{
				const auto& info = thk->mHeader.mNodeInfoList[i];

				for (u32 j = 0; j < info.mSegmentCount; j++)
				{
					if (&info.mNode[j] == segment)
					{
						nodeIndex = i;
						segmentIndex = j;
						goto endLoop; // very bruh I know but it's the cleanest way
					}
				}
			}

		endLoop:
			if (nodeIndex == -1)
			{
				LOG(INFO) << fmt::format("Failed to get node index {}, THK({})", static_cast<void*>(segment), static_cast<void*>(thinktable));
				return original(thisptr, out, segment);
			}

			if (g_LogSegments)
			{
				LOG(INFO) << fmt::format("[{} at {}] THK{:02}, Node Index: {}, Segment: {}",
					mh::Monster::Names.at(mon.m_id), m,
					thkId, nodeIndex, segmentIndex);

				g_Monsters[m] = nodeIndex;
			}
			else
			{
				if (g_Monsters.at(m) != nodeIndex)
				{
					LOG(INFO) << fmt::format("[{} at {}] THK{:02}, Node Index: {}",
						mh::Monster::Names.at(mon.m_id), m,
						thkId, nodeIndex);

					g_Monsters[m] = nodeIndex;
				}
			}
		}
	}

	return original(thisptr, out, segment);
}

CreateHook(MH::sMhInputText::Dispatch, Dispatch, void, const wchar_t* str)
{
	std::wstring s(str);
	
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned short c) { return std::towlower(c); });
	if (s.starts_with(L"/thk filter"))
	{
		swscanf_s(s.data(), L"/thk filter %d", &g_MonsterFilter);
		if (g_MonsterFilter == -1)
		{
			auto msg = "Disabled Monster Filter";
			DisplayMessage(msg);
			LOG(INFO) << msg;
		}
		else
		{
			const auto msg = fmt::format("Monster Filter set to {0}", mh::Monster::Names.at(static_cast<mh::Monster::ID>(g_MonsterFilter)));
			DisplayMessage(msg);
			LOG(INFO) << msg;
		}
	}
	else if (s.starts_with(L"/thk segments"))
	{
		if (s.substr(14) == L"true")
		{
			g_LogSegments = true;
			DisplayMessage("Enabled Logging Segments");
		}
		else if (s.substr(14) == L"false")
		{
			g_LogSegments = false;
			DisplayMessage("Disabled Logging Segments");
		}
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
