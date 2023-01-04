#pragma once

#include "dti_types.h"
#include <functional>

#pragma execution_character_set("utf-8")

struct rThinkTableList {
	u8 mPadding0[0x5c];
	s32 mRefCount;
	u32 mAttr;
	u8 mPadding1[0xc];
	u64 mSize;
	u64 mID;
	u64 mCreateTime;
	u32 get_mQuality() { return std::function<u32(rThinkTableList*)>((u32(*)(rThinkTableList*))0x1402B7C90)(this); }
	void set_mQuality(u32 val) { return std::function<void(rThinkTableList*, u32)>((void(*)(rThinkTableList*, u32))0x1402B8DF0)(this, val); }
	u32 get_mState() { return std::function<u32(rThinkTableList*)>((u32(*)(rThinkTableList*))0x1404521A0)(this); }
	void set_mState(u32 val) { return std::function<void(rThinkTableList*, u32)>((void(*)(rThinkTableList*, u32))0x140452380)(this, val); }
	u32 get_mTag() { return std::function<u32(rThinkTableList*)>((u32(*)(rThinkTableList*))0x1404521B0)(this); }
	void set_mTag(u32 val) { return std::function<void(rThinkTableList*, u32)>((void(*)(rThinkTableList*, u32))0x140452390)(this, val); }
	cstring get_mPath() { return std::function<cstring(rThinkTableList*)>((cstring(*)(rThinkTableList*))0x141986F00)(this); }
	void set_mPath(cstring val) { return std::function<void(rThinkTableList*, cstring)>((void(*)(rThinkTableList*, cstring))0x14023D5D0)(this, val); }
	template<class _T> _T& at(size_t _Off) { return *(_T*)(u64(this) + _Off); }
};

