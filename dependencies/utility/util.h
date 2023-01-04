#pragma once
#ifndef MINHOOK_VCPKG
#include <MinHook.h>
#endif // !MINHOOK_VCPKG

template<typename T>
inline T* offsetPtr(void* ptr, int offset) { return (T*)(((char*)ptr) + offset); }

template<class _Ty, class _Ty2 = void*>
inline _Ty OffsetPointer(_Ty2 ptr, int offset) { return (_Ty)(((char*)ptr) + offset); }

#define SHOW(VAR) "\""#VAR"\"={" << VAR << "}"

#define DeclareHook(TARGET, NAME, RET, PARAMS)	\
namespace hooks::NAME {							\
	static void* target = TARGET;				\
	RET hook PARAMS;							\
	static RET(*original) PARAMS;				\
}												\
RET hooks::NAME::hook PARAMS

#define DeclareApiHook(MODULE, FUNCNAME, TARGET, NAME, RET, PARAMS)	\
namespace apihooks::NAME {								\
	const wchar_t* pModule = MODULE;					\
	const char* pFunc = FUNCNAME;						\
	static void* target = TARGET;						\
	RET hook PARAMS;									\
	static RET(*original) PARAMS;						\
}														\
RET apihooks::NAME::hook PARAMS

//Create hook, write code as function, use QueueHook to actually hook
#define CreateHook(ADDR, NAME, RET, ...) DeclareHook(ADDR, NAME, RET, (__VA_ARGS__))

#define CreateApiHook(MODULE, FUNCNAME, TARGET, NAME, RET, ...) DeclareApiHook(MODULE, FUNCNAME, TARGET, NAME, RET, (__VA_ARGS__))

#define QueueApiHook(NAME)	\
do {						\
	MH_CreateHookApi(apihooks::NAME::pModule, apihooks::NAME::pFunc, apihooks::NAME::hook, (LPVOID*)&apihooks::NAME::original);\
	MH_QueueEnableHook(apihooks::NAME::target);\
} while(0)
#define QueueHook(NAME) \
 do {\
	MH_CreateHook(hooks::NAME::target, &hooks::NAME::hook, (LPVOID *)&hooks::NAME::original);\
	MH_QueueEnableHook(hooks::NAME::target);\
} while(0) 


template<typename R, typename ...Args>
void CreateHookFunction(R(*target)(Args...), R(*hook)(Args...), R(**trampoline)(Args...)) {
	MH_CreateHook(target, hook, (LPVOID*)trampoline);
}

//TARGET = function pointer, LAMBDA hook code 
#define HookLambda(TARGET, LAMBDA) do {\
	static decltype(TARGET) original;\
	CreateHookFunction(TARGET, (decltype(TARGET)) LAMBDA, &original);\
	MH_QueueEnableHook(TARGET);\
} while(false)

