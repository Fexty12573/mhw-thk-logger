#pragma once

#include "dti/dti_types.h"

#include <d3d11.h>
#include <dxgi.h>

struct sMhResource;
class sQuest;
struct cUserData;
struct sUserData;
struct cGUIObjSizeAdjustMessage;
struct MtWeaponInfo;
struct ActionTable;
struct uGimmick;
struct cDraw;

typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned long long undefined8;
typedef unsigned char byte;
typedef unsigned long long ulonglong;
typedef long long longlong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long long QWORD;
typedef unsigned int uint32_t;

namespace MH {
	namespace Player {
		/* Does other stuff as well, did not research what inidividual parameters do */
		inline ulonglong(*DrunkBird)(undefined8, undefined8) = (ulonglong(*)(undefined8, undefined8))0x140f34970;
		// 41 F7 E0 C1 EA 05 6B C2 64 44 2B C0 33 DB 8B D3   (15.10)

		/* arg1: pointer to savedata
		 * arg2: amount of points to be given
		 * ret:  unknown */
		inline undefined8(*ApplyHRPoints)(void*, int) = (undefined8(*)(void*, int))0x14137a500;
		// 8B 8F 9C 00 00 00 B8 FF E0 F5 05 03 CE 3B C8 0F 47 C8 89 8F 9C 00 00 00 48 8D 4F 08

		/* Returns the pointer of the players current target (with target camera) 
		   arg1: unknown
		   arg2: id of target (0, 1, 2...)
		   ret:  Pointer to target monster */
		inline longlong(*CheckCurrentTarget)(longlong, uint32_t) = (longlong(*)(longlong, uint32_t))0x1412ad630;
		// 48 83 EC 38 4C 8B C1 83 FA 0E 73 7F 8B C2   (15.10)

		/* Not updated */
		inline void(*ManageSharpness)(longlong, int) = (void(*)(longlong, int))0x142134b70;

		/* Needs more testing, not updated */
		inline ulonglong(*PlayerAction)(longlong, int*) = (ulonglong(*)(longlong, int*))0x14026a8e0;

		inline void(*SetPosition)(void*, float*, float*) = (void(*)(void*, float*, float*))0x141c1f1c0;

		inline void(*SetActionSet)(void*, u32, ActionTable*, u32, u32) = decltype(SetActionSet)(0x141c186a0);

		inline void(*CreateGimmick)(void*, u32, float) = (void(*)(void*, u32, float))0x141f933a0;

		inline void(*HandleActionPacket)(void*, void*, void*) = decltype(HandleActionPacket)(0x1411ba8c0);

		inline void* (*GetPlayerFromOnlineID)(void*, u32, s32) = decltype(GetPlayerFromOnlineID)(0x141b573a0);
		inline void* (*GetUserDataFromOnlineID)(void*, u32, s32) = decltype(GetUserDataFromOnlineID)(0x141b57030);
		inline void* (*GetPlayerFromSteamID)(void*, u64) = decltype(GetPlayerFromSteamID)(0x141b57340);

		inline void(*ChangeWeapon)(void*, u32, u32) = decltype(ChangeWeapon)(0x141f76ce0);

		inline void(*dtor)(void*, u32) = decltype(dtor)(0x14204fd80);

		inline void* StaticExePointer = (void*)0x14506d270;

		inline void* GetInstance() {
			void* instance = nullptr;
			void* sActBtn = *(void**)StaticExePointer;

			if (sActBtn)
				instance = *(void**)(ulonglong(sActBtn) + 0x80);

			return instance;
		}

		inline void* GetPlayerSingleton() {
			return *reinterpret_cast<void**>(0x14506f1b0);
		}
	}
	namespace Savefile {
		/* Saves the game
		   NOTE: May or may not have a 3rd parameter
		   arg1: unknown
		   arg2: unknown
		   ret:  unknown */
		inline undefined8(*Save)(longlong, longlong*) = (undefined8(*)(longlong, longlong*))0x14137b750;
		// 48 8B 03 48 8D 57 48 41 B8 04 00 00 00 48 89 5C 24 60   (15.10)

		/* Function isn't appropriately titled, does other things regarding savedata
		   arg1: destination (unknown)
		   arg2: source (source + 0x98 -> Selected save slot, -1 if when nothing)
		   ret:  unknown */
		inline longlong(*SelectSaveSlot)(longlong, longlong) = (longlong(*)(longlong, longlong))0x141b55900;
		// C6 46 3F 00 48 8D 4F 78 8B 47 48 48 8D 53 78 89 43 48   (15.10)

		inline cUserData* (*GetSelectSave)(sUserData*) = (cUserData* (*)(sUserData*))0x141ba9280;

		inline sUserData* GetInstance() {
			return *reinterpret_cast<sUserData**>(0x145073e80);
		}
	}
	namespace Monster {
		namespace DamageBehavior {
			/* Used for monster flinch actions, can be used to overwrite anything the monster is currently doing 
			   arg1: [monster + 0x12278]
			   arg2: [[monster + 0x12278] + 0x4B0] */
			inline void(*NextAction_)(void*, void*) = (void(*)(void*, void*))0x1413a6bf0;

			/* Not in use anymore, forgot parameters */
			inline undefined8(*EmDmgStuff)(longlong, ulonglong, uint, uint*) = (undefined8(*)(longlong, ulonglong, uint, uint*))0x1402be5d0;
		}
		namespace DieBehavior {
			// @param this (nEmAI::DieBehavior)
			// @param mon (uAIEm)
			inline void(*OnDeath)(void*, void*) = (void(*)(void*, void*))0x1413a7130;
		}

		namespace DamageCheck {
			inline void(*HandleDamagePacket)(void*, void*) = decltype(HandleDamagePacket)(0x1402c4d10);
			inline void(*DeserializeDamagePacket)(void*, void*, void*) = decltype(DeserializeDamagePacket)(0x1402c2ba0);

			// @param cEmDamageCheck* this
			// @param EmDamageInfo1* in1
			// @param u32 unk1
			// @param u32 unk2
			// @param u32 unk3
			// @param EmDamageInfo2* in2
			inline void(*ProcessDamagePacket)(void*, void*, u32, u32, u32, void*) = decltype(ProcessDamagePacket)(0x1402bd0d0);
		}

		inline void* (*GetPartInfo)(void*, u32) = (void* (*)(void*, u32))0x141cd2b50;

		inline void(*Initialize)(void*) = (void(*)(void*))0x1422456f0;

		inline longlong* (*AI_Interpreter)(longlong, ulonglong, undefined8) = (longlong * (*)(longlong, ulonglong, undefined8))0x141343650;
		// 45 33 F6 40 32 FF 44 89 75 67 4D 8B E0   (15.10)

		inline int(*ProcessTHKSegment)(void*, int*, void*) = (int(*)(void*, int*, void*))0x141349980;
		// 40 38 af 60 05 00 00 74 11 40 88 af 60 05 00 00 41 8d 44 24 fb

		inline void(*MonsterTimer)(longlong, undefined8, ulonglong, undefined8) = (void(*)(longlong, undefined8, ulonglong, undefined8))0x141bd4270;
		// 48 8B F9 0F 85 A3 02 00 00    (15.10)

		/* Read a given .col file and apply it to the appropriate monster
		   arg1: destination where collision values are written to (monster object, eventually)
		   arg2: source, aka the collision file that was loaded into memory. Both are void* / char* most likely */
		inline void(*ReadMonsterCol)(longlong, longlong) = (void(*)(longlong, longlong))0x1419c5820;
		// 8B 42 08 89 41 08 8B 42 0C 89 41 0C 8B 42 18 89 41 18 8B 42 20   (15.10)


		inline void(*AITargeting)(longlong, ulonglong, ulonglong) = (void(*)(longlong, ulonglong, ulonglong))0x1413334f0;

		/* Returns the given Monsters current Target
		   arg1: monster who's target is to be retrieved. [monster + 0x12278], 
		   ret:  pointer to current target of arg1 */
		inline longlong(*GetTargetMonster)(void*) = (longlong(*)(void*))0x141bc9720; 
		// 48 8B 81 C0 0A 00 00 48 85 C0 74 06 F6 40 0C 0E 75 02 (15.10)

		/* Used by the game for in-combat targeting (and in some other situations too
		   arg1: longlong _this_ai_data: The AI Data of the monster who's target is to be checked/changed (is technically void*)
		   arg2 is unused, arg3 is irrelevant */
		inline void(*KeepTrackOfTarget)(void*, undefined8, byte) = (void(*)(void*, undefined8, byte))0x141343030;
		// 48 89 5C 24 58 0F 57 C0 48 89 7C 24 70   (15.10)

		inline void* (*AICheckEnemyState)(void*, void*) = (void* (*)(void*, void*))0x141346FB0;

		/* Don't know params anymore */
		inline void(*UpdateTarget)(longlong, longlong*) = (void(*)(longlong, longlong*))0x142244ad0;

		/* Check Monster via (param_1 - 0x1D700 == monster)													UPDATED UP UNTIL HERE
		   arg1: target, the monster to perform the operation for (monster + 0x1D700), void*
		   arg2: unknown, likely an index for something
		   arg3: coords, an array of XYZ coordinates that will be set. (Vec3) */
		inline void(*SetTargetCoord)(void*, int, float*) = (void(*)(void*, int, float*))0x1413a40b0;

		/* Enrage Monster
		   arg1: pointer to structure owned by monster to be enraged (monster + 0x1BD08) 
		   ret:  whether function succeeded or not */
		inline bool(__fastcall* Enrage)(longlong) = (bool(__fastcall*)(longlong))0x1402a8da0;

		/* Unenrage Monster
		   arg1: same as enrage */
		inline void(__fastcall* Unenrage)(longlong) = (void(__fastcall*)(longlong))0x1402a9030;

		inline void(*EnableActionLinkingForNextActions)(bool*, int) = (void(*)(bool*, int))0x1413a1040;
		inline bool(*ActionsConnectable)(void*, int, int, int*) = (bool(*)(void*, int, int, int*))0x141cd4900;

		inline ActionTable* (*GetActionTableAt)(u32) = (ActionTable * (*)(u32))0x1413a10f0;
	}
	namespace HealthManager {
		inline void(*SetHP)(void*, float, bool) = (void(*)(void*, float, bool))0x141221fd0;
		inline void(*SetMaxHP)(void*, float) = (void(*)(void*, float))0x1412220a0;
	}
	namespace Quest {
		inline void(*AcceptQuest)(sQuest*, u32, u8) = (void(*)(sQuest*, u32, u8))0x141b7e1e0;
		inline void(*EnterQuest)(sQuest*) = (void(*)(sQuest*))0x141b62900;
		inline void(*LeaveQuest)(sQuest*) = (void(*)(sQuest*))0x141b7fa30;
		inline void(*AbandonQuest)(sQuest*, u32) = (void(*)(sQuest*, u32))0x141b8caf0;
		inline void(*ReturnFromQuest)(sQuest*) = (void(*)(sQuest*))0x141b8b6f0;
		inline void(*ExitQuest)(sQuest*) = (void(*)(sQuest*))0x141b806d0;
        inline void (*DepartOnQuest)(sQuest*, bool) = (void (*)(sQuest*, bool))0x141b847a0;

		inline u32(*SetRewards)(void*, void*, void*, u32) = decltype(SetRewards)(0x141ebdf20);
	}
	namespace EmSetter {
		inline void(*CatHelperSetter)(longlong) = (void(*)(longlong))0x141b49a70; // 50 80 79 30 00 4C 8B F9 75 09   (15.10)
	}
	namespace Chat {
		/* Process shoutouts given by the player
		   arg1: unknown 
		   arg2: unknown
		   arg3: shoutout id
		   arg4: unknown
		   arg5: unknown */
		inline void(*Shoutouts)(__int64, int, unsigned int, unsigned int, int) = (void(*)(__int64, int, unsigned int, unsigned int, int))0x141a656d0;
		// Shoutouts: 48 8B F1 72 24 4C 8B 91 B0 9A 00 00   (15.10)

		inline void(*PrintMessage)(cGUIObjSizeAdjustMessage*, const char*, u32) = (void(*)(cGUIObjSizeAdjustMessage*, const char*, u32))0x1423606f0;
	}
	namespace ActionController {
		struct ActionInfo { u32 mActionSet, mActionID; };
		inline bool(*DoAction)(void*, ActionInfo*) = (bool(*)(void*, ActionInfo*))0x14026a8e0;
	}
	namespace File {
		/* Loads any file from within nativePC/chunks into the game and returns a pointer to it
		   arg1: sMhResource    Current Location of the file manager (see below)
		   arg2: objDef			Resource definition, there is one for each file type within the exe
		   arg3: filename		Path to the file without the extension
		   arg4: flags			To tell the function to do certain stuff differently, generally 1 is used
		   ret:  pointer to where in memory the file was loaded to */
		inline undefined8(*_LoadResource)(void*, void*, char*, char) = (undefined8(*)(void*, void*, char*, char))0x14223ef10;
		inline ulonglong(*Func)(uint) = (ulonglong(*)(uint))0x1418c0ce0;

		/* dereference to get location of file manager */
		inline void** FileManager = (void**)0x145183f40;
	}
	namespace Resource {
		inline void* (*LoadResource)(void*, void*, char*, char) = (void* (*)(void*, void*, char*, char))0x14223ef10;
		inline void(*TryUnloadResource)(void* sMhResource, void* instance) = (void(*)(void*, void*))0x1422422b0;

		inline void** StaticExePointer = (void**)0x145183f40;

		template<class T>
		T* GetResource(MtDTI<T>* dti, const char* path, u32 flags) {
            return (T*)LoadResource(*StaticExePointer, (void*)dti, const_cast<char*>(path), flags);
		}
	}
	namespace cEmDamageCheck {
		/* Applies Damage
		   arg1: this - cEmDamageCheck
		   arg2: unk
		   arg3: { Monster, unk, unk, ... }
		*/
		inline void(*ApplyDamage)(void*, void*, void*) = (void(*)(void*, void*, void*))0x1402c1c80;
	}
	namespace Shell {
		// arg1: Address of shell file/object (For monster only)
		//inline longlong(*Execute)(void*, long long*, unsigned int*, int*, long long) = (longlong(*)(void*, long long*, unsigned int*, int*, long long))0x141abbd40;
		inline void*(*dtor)(void*, char) = (void*(*)(void*, char))0x141a301a0;
		inline void(*ExecuteForPlayer)(void*, void*, void*, int*) = decltype(ExecuteForPlayer)(0x141aba910);
		inline void(*ExecuteOther)(void*, void*, void*, int*, int*) = decltype(ExecuteOther)(0x141abaac0);
		inline longlong(*ExecuteForMonster)(void*, long long*, unsigned int*, int*, long long) = decltype(ExecuteForMonster)(0x141abbd40);
		inline float* (*CopyEnvData)(float* dst, const float* src) = decltype(CopyEnvData)(0x140a26450);
	}
	namespace Palico {
		inline undefined8(*PalicoBuff)(longlong) = (undefined8(*)(longlong))0x1419A3E40; // 49 03 C8 FF E1 BA 3C000000 8B C2   (15.10)
		inline undefined8(*CheerbongoBuff)(longlong, undefined8, undefined8, undefined8) = (undefined8(*)(longlong, undefined8, undefined8, undefined8))0x1419a39d0;
		inline float(*ParaDamage)(longlong) = (float(*)(longlong))0x1411f7780; // 8B 4B 10 0F B7 50 0E 66 0F 6E F2 0F 5B F6   (15.10)
	}
	namespace Debug {
		inline longlong(*SpawnMonster)(undefined8, uint, uint) = (longlong(*)(undefined8, uint, uint))0x141b31d80; // 40 57 48 83 EC 20 41 8B  F8 83 FA 77
		inline longlong(*SpawnMonsterInit)(undefined8, ulonglong, ulonglong, char) = (longlong(*)(undefined8, ulonglong, ulonglong, char))0x141a6e3d0; // 41 57 48 81 ec 80 00 00 00 4c 8b f9 45 0f b6 e1 ?? ?? ?? ?? ?? ?? ?? 41 8b d8 8b fa
		inline longlong(*CallSpawnMonsterInit)(undefined8, undefined4) = (longlong(*)(undefined8, undefined4))0x141a6df10;
		inline char(*GetControllerState)(__int64) = (char(*)(__int64))0x1422c2140;

		inline undefined8* SpawnVar = (undefined8*)0x14506D410;

		inline char(__fastcall* Calls_CallsCreateMonster)(char*, __int64*) = (char(__fastcall*)(char*, __int64*))0x142124e90;
		inline char(__fastcall* CallsCreateMonster)(void*, __int64, int, QWORD*) = (char(__fastcall*)(void*, __int64, int, QWORD*))0x142125050;
		inline bool(__fastcall* BigFunction)() = (bool(__fastcall*)())0x1402519f0;

		/* Spawns monsters from the quest data list 
		 * arg1: QuestDataList (dereferenced)
		 * arg2: NULL
		 * arg3: NULL
		 * arg4: NULL 
		 * ret:  true/false based on success of the function */
		inline bool(__fastcall* SpawnQuestMonster)(undefined8, unsigned int, unsigned int, unsigned int) = (bool(__fastcall*)(undefined8, unsigned int, unsigned int, unsigned int))0x141b889b0;
		inline char* (__fastcall* GetMonsterSobjPath)(longlong, char*, ulonglong) = (char* (__fastcall*)(longlong, char*, ulonglong))0x141b68660;
		inline char* (*GetMonsterSobjPathExped)(sQuest*, char*, int, int, int) = (char* (*)(sQuest*, char*, int, int, int))0x141b689a0;
		inline void* QuestDataList = (void*)0x14506f240;
	}
	namespace Camera {
		inline float*(*UpdateCameraTarget)(longlong) = (float* (*)(longlong))0x141fc1bf0; // 48 8B C4 55 41 57 48 81  EC D8 00 00 00
		inline float*(*UpdateCameraPosition)(undefined8, float, longlong) = (float* (*)(undefined8, float, longlong))0x141fbb610; // 48 83 ec 50 0f 10 91 f0 04 00 00 48 8d b9 c0 04 00 00 0f 10 99 00 05 00 00
	}
	namespace sMhInputText {
		inline void(*Dispatch)(wchar_t*) = (void(*)(wchar_t*))0x1423bb340;
	}
	namespace sMhScene {
		/* Gets called multiple times every time a scene is loaded
		 * arg1: this
		 * arg2: unknown
		 * ret: void */
		inline void(*Unknown)(void*, void*) = (void(*)(void*, void*))0x141b2dd10;
		inline void** (*ctor)(void**) = (void** (*)(void**))0x141b2d290;
		inline void** (*dtor)(void**, void**) = (void** (*)(void**, void**))0x141b2d710;
	}
	namespace sScene {
		inline void** (*ctor)(void**, unsigned int) = (void** (*)(void**, unsigned int))0x142280eb0;
	}
	namespace Network {
		namespace cPacketBase {
			inline void* (*ctor)(void*) = (void* (*)(void*))0x1411f8d70;
		}

		inline void(*SendPacket)(void*, void*, int, int, int) = (void(*)(void*, void*, int, int, int))0x141b22c80;
		inline void(*RecvPacket)(void*, __int64, int, char*, int) = (void(*)(void*, __int64, int, char*, int))0x1424b7fb0;
        inline void(*SendBuffer)(void*, char*, u32, u32, u32, u32) = decltype(SendBuffer)(0x1424b9b50);

		inline void(*ReceiveQuestBinary)(void*, u32, void*, bool, bool) = decltype(ReceiveQuestBinary)(0x141b255a0);
	}
	namespace sLightLinker {
		namespace Area {
			/* takes this, and flags as parameters */
			inline void* (*dtor)(void*, char) = (void* (*)(void*, char))0x141ac5d10;
			inline void* (*NewInstance)(void*, void*) = (void* (*)(void*, void*))0x141acb120;
		}
	}
	namespace rLightLinker {
		inline void* (*dtor)(void*, char) = (void* (*)(void*, char))0x141a0e0d0;
		inline void* ObjectDef = (void*)0x14506af10;
	}
	namespace sMhGUI {
		/* Gets called all the time
		 * arg1: this
		 * arg2: unknown
		 * ret: void */
		inline void(*Unknown)(void*) = (void(*)(void*))0x141af39c0;
		inline void* (*dtor)(void*, char) = (void* (*)(void*, char))0x141ae74b0;
		inline void(*ShowDamageNumber)(
			void* thisptr, 
			void* entity, 
			u32 number, 
			vector3* hit_pos,
			u32 color, 
			u32 crit, 
			u32 buff, 
			u32 tenderize, 
			bool unk) = decltype(ShowDamageNumber)(0x141af6800);
		inline void(*FormatDamageNumber)(
			void* thisptr,
			u32 unk1,
			bool unk2,
			u32 damage,
			vector3* hit_pos,
			u32 color,
			u32 crit,
			u32 buff,
			u32 tenderize,
			bool unk3) = decltype(FormatDamageNumber)(0x141e361a0);

		inline void* Vftable = (void*)0x1434513b8;

		inline void* GetInstance() {
			return *reinterpret_cast<void**>(0x145224b80);
		}
	}
	namespace sMhMain {
		inline void* (*dtor)(void*, char) = (void* (*)(void*, char))0x141b02a00;

		inline void* Vftable = (void*)0x143452bc0;
		inline void** StaticExePointer = (void**)0x145224688;
	}
	namespace Gimmick {
		inline void** (*ctor)(void**) = (void** (*)(void**))0x141d01330;
		inline uGimmick* (*Spawn)(u32 gmType, u32 unk, void* source) = (uGimmick * (*)(u32, u32, void*))0x1416fbb60;
		inline void (*GenNew)(void*, int, int, float*, u64, int) = (void(*)(void*, int, int, float*, u64, int))0x1402cbe50;
		inline void (*Init)(void*, uGimmick*, int, vector3*, int, bool, int*, bool) = decltype(Init)(0x1402ccab0);
		inline void (*PostInit)(uGimmick*) = (void(*)(uGimmick*))0x141d0dba0;
	}
	namespace nActEm043 {
		namespace LargeBite {
			inline void** (*ctor)(void**) = (void** (*)(void**))0x140c2f280;
		}
	}
	namespace MtObject {
		inline void** (*CtorInstance)(unsigned __int64, void**) = (void** (*)(unsigned __int64, void**))0x14218d380;
		inline void** (*NewInstance)() = (void** (*)())0x14218d340;
	}
	namespace AK {
		namespace SoundEngine {
			inline int(*RegisterGameObj)(unsigned __int64, const char*) = (int(*)(unsigned __int64, const char*))0x1429133e0;
			inline int(*RegisterGameObj2)(unsigned __int64) = (int(*)(unsigned __int64))0x142913350;
		}
	}
	namespace GUI {
		inline void(*DetermineDamageNumberType)(void*, unsigned int, void*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned char, unsigned int)
			= (void(*)(void*, unsigned int, void*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned char, unsigned int))0x141ce1670;
		namespace cGUIObject {
			inline void(*SetCurrentFrame)(void*, float, bool) = decltype(SetCurrentFrame)(0x14235f640);
		}
		inline void(*DisplayAlert)(const char*) = decltype(DisplayAlert)(0x1418e7da0);
		inline void(*DisplayMessageWindow)(void*, const char*, void*, float*, u8) = decltype(DisplayMessageWindow)(0x141af75b0);
	}
	namespace rModel {
		inline bool(*Deserialize)(void*, void*) = decltype(Deserialize)(0x14227eee0);
	}
	namespace Weapon {
		inline MtWeaponInfo* (*GetWeaponInfo)(u32) = decltype(GetWeaponInfo)(0x142182000);
	}
	namespace Render {
		inline HRESULT(*D3D11CreateDevice)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**) = decltype(D3D11CreateDevice)(0x142795c08);
		inline void(*SetDrawPass)(cDraw*, u32) = decltype(SetDrawPass)(0x1423ea7d0);
	}
}

