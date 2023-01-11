#pragma once
#include "dti_types.h"
#include "rThinkTableList.h"
#include <functional>

#pragma execution_character_set("utf-8")

struct cThinkMgr {
	void* GetParent()
	{
		return at<cThinkMgr*>(0x538)->at<void*>(0x8);
	}
	void* GetcThinkEm()
	{
		return at<cThinkMgr*>(0x538);
	}
	void* GetTHKList()
	{
		return at<rThinkTableList*>(0x130);
	}
	template<class _T> _T& at(size_t _Off) { return *(_T*)(u64(this) + _Off); }
};

