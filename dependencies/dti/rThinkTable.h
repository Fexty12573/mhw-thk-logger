#pragma once
#include "dti_types.h"
#include <functional>

#pragma execution_character_set("utf-8")

struct THK
{
	struct Segment
	{
		u8 NodeEndRandom;
		u8 InterruptType;
		u8 SegmentType;
		u8 unk1;
		u32 unk2;
		s32 CheckType;
		u32 Parameter1;
		u32 unk3;
		u32 unk4;
		u32 unk5;
		u32 unk6;
		u32 Parameter2;
		u32 NodeEndingData;
		u32 ExtRefThkID;
		u32 ExtRefNodeID;
		s64 LocalRefNodeIndex;
		u32 unk7;
		u32 unk8;
		u32 unk9;
		u32 unk10;
		u32 unk11;
		u32 ActionID;
		u32 ActionUnknown[3];
		u32 unk12;
		u32 ActionUnknown3;
		u32 ActionUnknown4;
		u32 unkExtra[6];
	};
	struct NodeInfo
	{
		Segment* mNode;
		u32 mSegmentCount;
		u32 mNodeExternalID;
	};
	struct Header
	{
		char mType[4];
		u16 mVersion;
		u16 mNodeCount;
		u32 mThkType;
		u32 mNull;
		u32 mMonsterID;
		u32 mNull2;
		NodeInfo* mNodeInfoList;
	};
	
	Header mHeader;
};

struct rThinkTable
{
	void* vft;
	u32 mHash;
	char mPath[80];
	s32 mRefCount;
	u32 mAttr;
	u8 mPadding1[0xc];
	u64 mSize;
	u64 mID;
	u64 mCreateTime;
	u8 mData[32];
	THK* mTHK;
	u32 get_mQuality() { return std::function<u32(rThinkTable*)>((u32(*)(rThinkTable*))0x1402B7C90)(this); }
	void set_mQuality(u32 val) { return std::function<void(rThinkTable*, u32)>((void(*)(rThinkTable*, u32))0x1402B8DF0)(this, val); }
	u32 get_mState() { return std::function<u32(rThinkTable*)>((u32(*)(rThinkTable*))0x1404521A0)(this); }
	void set_mState(u32 val) { return std::function<void(rThinkTable*, u32)>((void(*)(rThinkTable*, u32))0x140452380)(this, val); }
	u32 get_mTag() { return std::function<u32(rThinkTable*)>((u32(*)(rThinkTable*))0x1404521B0)(this); }
	void set_mTag(u32 val) { return std::function<void(rThinkTable*, u32)>((void(*)(rThinkTable*, u32))0x140452390)(this, val); }
	cstring get_mPath() { return std::function<cstring(rThinkTable*)>((cstring(*)(rThinkTable*))0x141986F00)(this); }
	void set_mPath(cstring val) { return std::function<void(rThinkTable*, cstring)>((void(*)(rThinkTable*, cstring))0x14023D5D0)(this, val); }
	template<class _T> _T& at(size_t _Off) { return *(_T*)(u64(this) + _Off); }

	static rThinkTable* GetThinkTableAtIndex(void* thkList, u32 idx)
	{
		return ((rThinkTable * (*)(void*, u32))0x141a391a0)(thkList, idx);
	}
};

