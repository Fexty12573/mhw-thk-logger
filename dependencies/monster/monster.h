#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>

namespace mh {

	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned long;
    using s64 = signed long long;
	using u64 = unsigned long long;

    struct HitzoneData
    {
        float Timer;
        unsigned int Sever;
        unsigned int Blunt;
        unsigned int Shot;
        unsigned int Fire;
        unsigned int Water;
        unsigned int Ice;
        unsigned int Thunder;
        unsigned int Dragon;
        unsigned int Stun;
        unsigned int Stamina;
    };
    struct Ailment
    {
        void* owner;
        int active;
        int unk1;
        int id;
        float max_duration;
        float unk3;
        int unk4;
        int unk5;
        int unk6;
        float unk7;
        int unk8;
        float buildup;
        int unk10;
        float unk11;
        float unk12;
        float max_buildup;
        float unk13;
        float unk14;
        float unk15;
        float unk16;
        float unk17;
        float unk18;
        int unk19;
        float unk20;
        float unk21;
        int unk22;
        float unk23;
        float duration;
        int unk25;
        int counter;
        int unk27;
        int unk28;
        int unk29;
    };
    struct PartData
    {
        float hp;
        float max_hp;
        int counter;
    };

    struct Vec3 {
        float x, y, z;
    };
    struct Vec4 {
        float x, y, z, w;
    };

    class Monster
    {
    public:
        enum class ID
        {
            Anjanath = 0x00,
            Rathalos = 0x01,
            Aptonoth = 0x02,
            Jagras = 0x03,
            ZorahMagdaros = 0x04,
            Mosswine = 0x05,
            Gajau = 0x06,
            GreatJagras = 0x07,
            KestodonM = 0x08,
            Rathian = 0x09,
            PinkRathian = 0x0A,
            AzureRathalos = 0x0B,
            Diablos = 0x0C,
            BlackDiablos = 0x0D,
            Kirin = 0x0E,
            Behemoth = 0x0F,
            KushalaDaora = 0x10,
            Lunastra = 0x11,
            Teostra = 0x12,
            Lavasioth = 0x13,
            Deviljho = 0x14,
            Barroth = 0x15,
            Uragaan = 0x16,
            Leshen = 0x17,
            Pukei = 0x18,
            Nergigante = 0x19,
            XenoJiiva = 0x1A,
            KuluYaKu = 0x1B,
            TzitziYaKu = 0x1C,
            Jyuratodus = 0x1D,
            TobiKadachi = 0x1E,
            Paolumu = 0x1F,
            Legiana = 0x20,
            GreatGirros = 0x21,
            Odogaron = 0x22,
            Radobaan = 0x23,
            VaalHazak = 0x24,
            Dodogama = 0x25,
            KulveTaroth = 0x26,
            Bazelgeuse = 0x27,
            Apceros = 0x28,
            KelbiM = 0x29,
            KelbiF = 0x2A,
            Hornetaur = 0x2B,
            Vespoid = 0x2C,
            Mernos = 0x2D,
            KestodonF = 0x2E,
            Raphinos = 0x2F,
            Shamos = 0x30,
            Barnos = 0x31,
            Girros = 0x32,
            AncientLeshen = 0x33,
            Gastodon = 0x34,
            Noios = 0x35,
            Magmacore = 0x36,
            Magmacore2 = 0x37,
            Gajalaka = 0x38,
            SmallBarrel = 0x39, //training objects
            LargeBarrel = 0x3A,
            TrainingPole = 0x3B,
            TrainingWagon = 0x3C,
            Tigrex = 0x3D,
            Nargacuga = 0x3E,
            Barioth = 0x3F,
            SavageDeviljho = 0x40,
            Brachydios = 0x41,
            Glavenus = 0x42,
            AcidicGlavenus = 0x43,
            FulgurAnjanath = 0x44,
            CoralPukei = 0x45,
            RuinerNergigante = 0x46,
            ViperTobi = 0x47,
            NightshadePaolumu = 0x48,
            ShriekingLegiana = 0x49,
            EbonyOdogaron = 0x4A,
            BlackveilVaal = 0x4B,
            SeethingBazelgeuse = 0x4C,
            Beotodus = 0x4D,
            Banbaro = 0x4E,
            Velkhana = 0x4F,
            Namielle = 0x50,
            Shara = 0x51,
            Popo = 0x52,
            Anteka = 0x53,
            Wulg = 0x54,
            Cortos = 0x55,
            Boaboa = 0x56,
            Alatreon = 0x57,
            GoldRathian = 0x58,
            SilverRathalos = 0x59,
            YianGaruga = 0x5A,
            Rajang = 0x5B,
            FuriousRajang = 0x5C,
            BruteTigrex = 0x5D,
            Zinogre = 0x5E,
            StygianZinogre = 0x5F,
            RagingBrachy = 0x60,
            SafiJiiva = 0x61,
            Unavaliable = 0x62,
            ScarredYianGaruga = 0x63,
            FrostfangBarioth = 0x64,
            Fatalis = 0x65
        };
        static const std::map<ID, const char*> Names;

    public:
        void* m_ptr;
        ID m_id;
        unsigned int m_variant;
        std::vector<HitzoneData> m_HZV;
        std::vector<Ailment> m_ailments;
        std::vector<PartData> m_parts;

        float* m_timer;
        float* m_max_timer;

        class cAngryData
        {
        public:
            void* vft;
            uint32_t mBuildup;
            float mDuration;
            float mSpeedMod;
            float mDamageMod;
            float mPlayerDamageMod;
            // ...
        };

    private:
        using enr_t = bool(__fastcall*)(void*);
        using unr_t = void(__fastcall*)(void*);
        using fa_t = void(*)(void*, void*);

		Vec3* m_pos;
        Vec3* m_col_pos;
        Vec3* m_scale;
		Vec4* m_rot;

        std::function<bool(void*)> pEnrage = enr_t(0x1402a8da0);
        std::function<void(void*)> pUnenrage = unr_t(0x1402a9030);
        std::function<void(void*, void*)> pForceAction = fa_t(0x1413a6bf0);

    public:
        Monster(ID id, unsigned int variant, void* _this)
            : m_id(id), m_variant(variant), m_ptr(_this),
            m_timer(nullptr), m_max_timer(nullptr)
        {
			m_pos = (Vec3*)((u64)m_ptr + 0x160);
			m_col_pos = (Vec3*)((u64)m_ptr + 0xA50);
			m_scale = (Vec3*)((u64)m_ptr + 0x180);
			m_rot = (Vec4*)((u64)m_ptr + 0x170);

            // SetBaseSpeed failsafe
            *(float*)((u64)m_ptr + 0x1D8A8) = 1.0f;
        }

        void Wipe();

        void SetX(float f);
        void SetY(float f);
        void SetZ(float f);
        void Teleport(Vec3 xyz);
        float GetX() const;
        float GetY() const;
        float GetZ() const;
        Vec3 GetPosition() const;


        float GetRotX() const;
        float GetRotY() const;
        float GetRotZ() const;
        float GetRotW() const;
        Vec4 GetRotation() const;
        void SetRotX(float rot);
        void SetRotY(float rot);
        void SetRotZ(float rot);
        void SetRotW(float rot);
        void Rotate(Vec4 rot);

        Vec3 GetSize() const;
        void SizeX(float sz);
        void SizeY(float sz);
        void SizeZ(float sz);
        void SetSize(float size_mod);

        void SetHP(float hp);
        void SetMaxHP(float hp);

        void SetBaseSpeed(float spd);
        void SetBaseSpeedNoModifyCode(float spd);

        void SetDespawnTime(float seconds);

        std::string GetActionName(int id);

        void SetDifficultyIndex(u32 idx);
        void SetPaused(bool v);

        void ForceAction(int action, bool link = false);
        void LinkNextActions(int begin, int end);

        void Enrage();
        void Unenrage();

        cAngryData* GetAngryData() const;

        void SetEnrageSpeed(float speed);
        void SetEnrageDamage(float damage);

        void SetTarget(void* target_pointer);

        template<class _T> _T& at(size_t _Off) { return *(_T*)(u64(m_ptr) + _Off); }

        bool operator==(const Monster& rhs);
        bool operator!=(const Monster& rhs);
        inline bool operator==(const void* ptr) const { return m_ptr == ptr; }
        inline bool operator!=(const void* ptr) const { return m_ptr != ptr; }

        static bool InitializeSpawner(bool initMinHook = true);
        static bool Spawn(ID id, u32 subid, u32 sobjid);

    private:
        std::pair<void*, void*> GetForceActionParams();
        void* GetAIData() const;

    private:
		static const std::map<ID, const char*> s_MonsterFileIDs;
    };

    inline Monster::ID& operator++(Monster::ID& id)
    {
        int val = static_cast<int>(id);
        id = static_cast<Monster::ID>(val + 1);

        return id;
    }
    inline Monster::ID& operator++(Monster::ID& id, int)
    {
        int val = static_cast<int>(id);
        id = static_cast<Monster::ID>(val + 1);

        return id;
    }
    inline bool operator==(const Monster::ID& id, int& i) { return static_cast<int>(id) == i; }
    inline bool operator!=(const Monster::ID& id, int& i) { return !(id == i); }

    inline bool operator==(int& i, const Monster::ID& id) { return static_cast<int>(id) == i; }
    inline bool operator!=(int& i, const Monster::ID& id) { return !(id == i); }
}
