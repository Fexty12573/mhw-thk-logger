
#include "monster.h"
#include "Memory.hpp"
#include "game_functions.h"

#include <MinHook.h>
#include <string>

namespace mh {

	template<class T> static inline T& deref(void* ptr) { return *(T*)ptr; }
	template<class T> static inline T& deref(u64 ptr) { return *(T*)ptr; }

    template<class T> static inline T& offset(void* ptr, u64 off) { return *(T*)((char*)ptr + off); }
    template<class T> static inline T& offset(u64 ptr, u64 off) { return *(T*)(ptr + off); }

    template<class T> inline T offset_nd(void* ptr, u64 off) { return (T)((char*)ptr + off); }
    template<class T> inline T offset_nd(u64 ptr, u64 off) { return (T)(ptr + off); }
   
    void Monster::Wipe()
    {
        m_id = ID(0);
        m_variant = 0;

        m_HZV.clear();
        m_ailments.clear();
        m_parts.clear();

        m_ptr = nullptr;
        m_timer = nullptr;
        m_max_timer = nullptr;
    }

    void Monster::SetX(float f)
    {
        m_pos->x = f;
        m_col_pos->x = f;
    }
    void Monster::SetY(float f)
    {
        m_pos->y = f;
        m_col_pos->y = f;
    }
    void Monster::SetZ(float f)
    {
        m_pos->z = f;
        m_col_pos->z = f;
    }
    void Monster::Teleport(Vec3 xyz)
    {
        xyz.y += 100;
		*m_pos = xyz;
		*m_col_pos = xyz;
    }
    float Monster::GetX() const
    {
        return m_pos->x;
    }
    float Monster::GetY() const
    {
        return m_pos->y;
    }
    float Monster::GetZ() const
    {
        return m_pos->z;
    }
    Vec3 Monster::GetPosition() const
    {
        return *m_pos;
    }

    float Monster::GetRotX() const
    {
        return m_rot->x;
    }
    float Monster::GetRotY() const
    {
        return m_rot->y;
    }
    float Monster::GetRotZ() const
    {
        return m_rot->z;
    }
    float Monster::GetRotW() const
    {
        return m_rot->w;
    }
    Vec4 Monster::GetRotation() const
    {
        return *m_rot;
    }
    void Monster::SetRotX(float rot)
    {
        m_rot->x = rot;
    }
    void Monster::SetRotY(float rot)
    {
        m_rot->y = rot;
    }
    void Monster::SetRotZ(float rot)
    {
        m_rot->z = rot;
    }
    void Monster::SetRotW(float rot)
    {
        m_rot->w = rot;
    }
    void Monster::Rotate(Vec4 rot)
    {
        *m_rot = rot;
    }

    Vec3 Monster::GetSize() const
    {
        return *m_scale;
    }
    void Monster::SizeX(float sz)
    {
        m_scale->x = sz;
    }
    void Monster::SizeY(float sz)
    {
        m_scale->y = sz;
    }
    void Monster::SizeZ(float sz)
    {
        m_scale->z = sz;
    }
    void Monster::SetSize(float size_mod)
    {
        m_scale->x = size_mod;
        m_scale->y = size_mod;
        m_scale->z = size_mod;
    }

    void Monster::SetHP(float hp)
    {
        void* healthMgr = offset<void*>(m_ptr, 0x7670);
        MH::HealthManager::SetHP(healthMgr, hp, false);
    }

    void Monster::SetMaxHP(float hp)
    {
        void* healthMgr = offset<void*>(m_ptr, 0x7670);
        MH::HealthManager::SetMaxHP(healthMgr, hp);
    }

    void Monster::SetBaseSpeed(float spd)
    {
        static bool init = false;
        if (!init)
        {
            memory::Nop((void*)0x141CCCD4B, 10); // assures speed won't be reset
            memory::Nop((void*)0x140B0513F, 6); // mov [rdx+1d8a8],eax
            init = true;
        }

        offset<float>(m_ptr, 0x1D8A8) = spd;
    }

    void Monster::SetBaseSpeedNoModifyCode(float spd) {
        offset<float>(m_ptr, 0x1D8A8) = spd;
	}

    void Monster::SetDespawnTime(float seconds)
    {
        offset<float>(m_ptr, 0x1C3D4) = seconds;
    }

    std::string Monster::GetActionName(int id)
    {
        void* actmgr = offset_nd<void*>(m_ptr, 0x61C8);
        void** actlst = offset<void**>(actmgr, 0x78);

        if (!actlst)
            return "";
        if (!actlst[id])
            return "";

        return std::string(offset<char*>(actlst[id], 0x20));
    }

    void Monster::SetDifficultyIndex(u32 idx)
    {
        void* uai = offset<void*>(m_ptr, 0x12278);
        if (uai) offset<u32>(uai, 0x8AC) = idx;
    }

    void Monster::SetPaused(bool v)
    {
        ((void(*)(void*, bool))0x1422456D0)(m_ptr, v);
    }

    void Monster::ForceAction(int action, bool link)
    {
        auto pair = GetForceActionParams();

        if (link)
            MH::Monster::EnableActionLinkingForNextActions(offset_nd<bool*>(m_ptr, 0x1D900), 0);

        offset<int>(m_ptr, 0x18938) = action;
        MH::Monster::DamageBehavior::NextAction_(pair.first, pair.second);
    }
    void Monster::LinkNextActions(int begin, int end)
    {
        int out;
        if (MH::Monster::ActionsConnectable(m_ptr, begin, end, &out))
            MH::Monster::EnableActionLinkingForNextActions(offset_nd<bool*>(m_ptr, 0x1D900), out);
    }
    std::pair<void*, void*> Monster::GetForceActionParams()
    {
        std::pair<void*, void*> params;

        auto aidata = GetAIData();

        params.first = offset<void*>(aidata, 0x4B0);
        params.second = aidata;

        return params;
    }

    void Monster::Enrage()
    {
        pEnrage(offset_nd<void*>(m_ptr, 0x1BD08));
    }
    void Monster::Unenrage()
    {
        pUnenrage(offset_nd<void*>(m_ptr, 0x1BD08));
    }

    Monster::cAngryData* Monster::GetAngryData() const
    {
        void* dtt = offset<void*>(m_ptr, 0x89E0);
        if (!dtt) return nullptr;

        void* em = offset<void*>(dtt, 0xB0);
        if (!em) return nullptr;

        return offset_nd<cAngryData*>(em, 0x100);
    }

    void Monster::SetEnrageSpeed(float speed)
    {
        auto data = GetAngryData();
        if (data)
            data->mSpeedMod = speed;
    }
    void Monster::SetEnrageDamage(float damage)
    {
        auto data = GetAngryData();
        if (data)
            data->mDamageMod = damage;
    }

    void Monster::SetTarget(void* target_pointer)
    {
        void** target_address = memory::ReadPointer<void**>((QWORD)m_ptr + 0x12278, { 0xAA8, 0x5D0 });
        *target_address = target_pointer;
    }

    bool Monster::operator==(const Monster& rhs)
    {
        return m_ptr == rhs.m_ptr;
    }
    bool Monster::operator!=(const Monster& rhs)
    {
        return !(*this == rhs);
    }

    void* Monster::GetAIData() const
    {
        return offset<void*>(m_ptr, 0x12278);
    }
    
    const std::map<Monster::ID, const char*> Monster::s_MonsterFileIDs = {
        { ID::Rathian, "001_00" },
        { ID::PinkRathian, "001_01" },
        { ID::GoldRathian, "001_02" },
        { ID::Rathalos, "002_00" },
        { ID::AzureRathalos, "002_01" },
        { ID::SilverRathalos, "002_02" },
        { ID::Diablos, "007_00" },
        { ID::BlackDiablos, "007_01" },
        { ID::Kirin, "011_00" },
        { ID::Fatalis, "013_00" },
        { ID::YianGaruga, "018_00" },
        { ID::ScarredYianGaruga, "018_05" },
        { ID::Rajang, "023_00" },
        { ID::FuriousRajang, "023_05" },
        { ID::KushalaDaora, "024_00" },
        { ID::Lunastra, "026_00" },
        { ID::Teostra, "027_00" },
        { ID::Tigrex, "032_00" },
        { ID::BruteTigrex, "032_01" },
        { ID::Lavasioth, "036_00" },
        { ID::Nargacuga, "037_00" },
        { ID::Barioth, "042_00" },
        { ID::FrostfangBarioth, "042_05" },
        { ID::Deviljho, "043_00" },
        { ID::SavageDeviljho, "043_05" },
        { ID::Barroth, "044_00" },
        { ID::Uragaan, "045_00" },
        { ID::Alatreon, "050_00" },
        { ID::Zinogre, "057_00" },
        { ID::StygianZinogre, "057_01" },
        { ID::Brachydios, "063_00" },
        { ID::RagingBrachy, "063_05" },
        { ID::Glavenus, "080_00" },
        { ID::AcidicGlavenus, "080_01" },
        { ID::Anjanath, "100_00" },
        { ID::FulgurAnjanath, "100_01" },
        { ID::GreatJagras, "101_00" },
        { ID::Pukei, "102_00" },
        { ID::CoralPukei, "102_01" },
        { ID::Nergigante, "103_00" },
        { ID::RuinerNergigante, "103_05" },
        { ID::SafiJiiva, "104_00" },
        { ID::XenoJiiva, "105_00" },
        { ID::ZorahMagdaros, "106_00" },
        { ID::KuluYaKu, "107_00" },
        { ID::Jyuratodus, "108_00" },
        { ID::TobiKadachi, "109_00" },
        { ID::ViperTobi, "109_01" },
        { ID::Paolumu, "110_00" },
        { ID::NightshadePaolumu, "110_01" },
        { ID::Legiana, "111_00" },
        { ID::ShriekingLegiana, "111_05" },
        { ID::GreatGirros, "112_00" },
        { ID::Odogaron, "113_00" },
        { ID::EbonyOdogaron, "113_01" },
        { ID::Radobaan, "114_00" },
        { ID::VaalHazak, "115_00" },
        { ID::BlackveilVaal, "115_00" },
        { ID::Dodogama, "116_00" },
        { ID::KulveTaroth, "117_00" },
        { ID::Bazelgeuse, "118_00" },
        { ID::SeethingBazelgeuse, "118_05" },
        { ID::TzitziYaKu, "120_00" },
        { ID::Behemoth, "121_00" },
        { ID::Beotodus, "122_00" },
        { ID::Banbaro, "123_00" },
        { ID::Velkhana, "124_00" },
        { ID::Namielle, "125_00" },
        { ID::Shara, "126_00" },
        { ID::Leshen, "127_00" },
        { ID::AncientLeshen, "127_01" },
        { ID::Aptonoth, "s001_00" },
        { ID::Apceros, "s002_00" },
        { ID::KelbiM, "s003_00" },
        { ID::KelbiF, "s003_05" },
        { ID::Mosswine, "s004_00" },
        { ID::Hornetaur, "s005_00" },
        { ID::Vespoid, "s006_00" },
        { ID::Popo, "s014_00" },
        { ID::Anteka, "s016_00" },
        { ID::Gajau, "s029_00" },
        { ID::Jagras, "s049_00" },
        { ID::Mernos, "s050_00" },
        { ID::KestodonM, "s051_00" },
        { ID::KestodonF, "s051_05" },
        { ID::Raphinos, "s053_00" },
        { ID::Shamos, "s054_00" },
        { ID::Barnos, "s055_00" },
        { ID::Girros, "s056_00" },
        { ID::Gastodon, "s058_00" },
        { ID::Noios, "s059_00" },
        { ID::Magmacore, "s060_00" },
        { ID::Magmacore2, "s060_01" },
        { ID::Gajalaka, "s061_00" },
        { ID::Boaboa, "s061_01" },
        { ID::SmallBarrel, "s062_00" },
        { ID::LargeBarrel, "s062_01" },
        { ID::TrainingPole, "s062_02" },
        { ID::TrainingWagon, "s062_03" },
        { ID::Wulg, "s063_00" },
        { ID::Cortos, "s064_00" }
    };

    const std::map<Monster::ID, const char*> Monster::Names {
        {ID::Anjanath, "Anjanath"},
        {ID::Rathalos, "Rathalos"},
        {ID::Aptonoth, "Aptonoth"},
        {ID::Jagras, "Jagras"},
        {ID::ZorahMagdaros, "Zorah Magdaros"},
        {ID::Mosswine, "Mosswine"},
        {ID::Gajau, "Gajau"},
        {ID::GreatJagras, "Great Jagras"},
        {ID::KestodonM, "Kestodon M"},
        {ID::Rathian, "Rathian"},
        {ID::PinkRathian, "Pink Rathian"},
        {ID::AzureRathalos, "Azure Rathalos"},
        {ID::Diablos, "Diablos"},
        {ID::BlackDiablos, "Black Diablos"},
        {ID::Kirin, "Kirin"},
        {ID::Behemoth, "Behemoth"},
        {ID::KushalaDaora, "Kushala Daora"},
        {ID::Lunastra, "Lunastra"},
        {ID::Teostra, "Teostra"},
        {ID::Lavasioth, "Lavasioth"},
        {ID::Deviljho, "Deviljho"},
        {ID::Barroth, "Barroth"},
        {ID::Uragaan, "Uragaan"},
        {ID::Leshen, "Leshen"},
        {ID::Pukei, "Pukei"},
        {ID::Nergigante, "Nergigante"},
        {ID::XenoJiiva, "Xeno'Jiiva"},
        {ID::KuluYaKu, "Kulu-Ya-Ku"},
        {ID::TzitziYaKu, "Tzitzi-Ya-Ku"},
        {ID::Jyuratodus, "Jyuratodus"},
        {ID::TobiKadachi, "Tobi Kadachi"},
        {ID::Paolumu, "Paolumu"},
        {ID::Legiana, "Legiana"},
        {ID::GreatGirros, "Great Girros"},
        {ID::Odogaron, "Odogaron"},
        {ID::Radobaan, "Radobaan"},
        {ID::VaalHazak, "Vaal Hazak"},
        {ID::Dodogama, "Dodogama"},
        {ID::KulveTaroth, "Kulve Taroth"},
        {ID::Bazelgeuse, "Bazelgeuse"},
        {ID::Apceros, "Apceros"},
        {ID::KelbiM, "Kelbi"},
        {ID::KelbiF, "Kelbi2"},
        {ID::Hornetaur, "Hornetaur"},
        {ID::Vespoid, "Vespoid"},
        {ID::Mernos, "Mernos"},
        {ID::KestodonF, "Kestodon F"},
        {ID::Raphinos, "Raphinos"},
        {ID::Shamos, "Shamos"},
        {ID::Barnos, "Barnos"},
        {ID::Girros, "Girros"},
        {ID::AncientLeshen, "Ancient Leshen"},
        {ID::Gastodon, "Gastodons"},
        {ID::Noios, "Noios"},
        {ID::Gajalaka, "Gajalaka"},
        {ID::Tigrex, "Tigrex"},
        {ID::Nargacuga, "Nargacuga"},
        {ID::Barioth, "Barioth"},
        {ID::SavageDeviljho, "Savage Deviljho"},
        {ID::Brachydios, "Brachydios"},
        {ID::Glavenus, "Glavenus"},
        {ID::AcidicGlavenus, "Acidic Glavenus"},
        {ID::FulgurAnjanath, "Fulgur Anjanath"},
        {ID::CoralPukei, "Coral Pukei"},
        {ID::RuinerNergigante, "Ruiner Nergigante"},
        {ID::ViperTobi, "Viper Tobi Kadachi"},
        {ID::NightshadePaolumu, "Nightshade Paolumu"},
        {ID::ShriekingLegiana, "Shrieking Legiana"},
        {ID::EbonyOdogaron, "Ebony Odogaron"},
        {ID::BlackveilVaal, "Blackveil Vaal Hazak"},
        {ID::SeethingBazelgeuse, "Seething Bazelgeuse"},
        {ID::Beotodus, "Beotodus"},
        {ID::Banbaro, "Banbaro"},
        {ID::Velkhana, "Velkhana"},
        {ID::Namielle, "Namielle"},
        {ID::Shara, "Shara Ishvalda"},
        {ID::Popo, "Popo"},
        {ID::Anteka, "Anteka"},
        {ID::Wulg, "Wulg"},
        {ID::Cortos, "Cortos"},
        {ID::Boaboa, "Boaboa"},
        {ID::Alatreon, "Alatreon"},
        {ID::GoldRathian, "Gold Rathian"},
        {ID::SilverRathalos, "Silver Rathalos"},
        {ID::YianGaruga, "Yian Garuga"},
        {ID::Rajang, "Rajang"},
        {ID::FuriousRajang, "Furious Rajang"},
        {ID::BruteTigrex, "Brute Tigrex"},
        {ID::Zinogre, "Zinogre"},
        {ID::StygianZinogre, "Stygian Zinogre"},
        {ID::RagingBrachy, "Raging Brachydios"},
        {ID::SafiJiiva, "Safi'Jiiva"},
        {ID::Unavaliable, "Unavaliable"},
        {ID::ScarredYianGaruga, "Scarred Yian Garuga"},
        {ID::FrostfangBarioth, "Frostfang Barioth"},
        {ID::Fatalis, "Fatalis"},
        {ID::SmallBarrel, "Small Barrel"},
        {ID::LargeBarrel, "Large Barrel"},
        {ID::TrainingPole, "Training Pole"},
        {ID::Magmacore, "Magmacore"},
        {ID::Magmacore2, "Magmacore2"}
    };
}
