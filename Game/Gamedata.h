#pragma once

#include <sstream>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>

struct ElementalAffinity;
struct PersonaStats;
struct PersonaData;
struct FusionData;
struct SpecialData;
struct SkillData;
struct SkillUser;
struct SkillCardData;
struct HeartItemData;
struct FusionSkillData;
struct GameExclusives;
struct PersonaCombos;
struct PersonaLevelPair;

using ElemWeakness       = int                                    ;
using GetPersonaFilter   = int                                    ;
using GetSkillListFlag   = int                                    ;
using ImChar             = const char*                            ;
using IntVec             = std::vector<int>                       ;
using CharVec            = std::vector<const char*>               ;
using StrVec             = std::vector<std::string>               ;
using FusionVec          = std::vector<FusionData>                ;
using PDataVec           = std::vector<PersonaData>               ;
using FusionVec          = std::vector<FusionData>                ;
using SpDataVec          = std::vector<SpecialData>               ;
using SkillVec           = std::vector<SkillData>                 ;
using SkillVec2          = std::vector<SkillVec>                  ;
using PComboVec          = std::vector<PersonaCombos>             ;
using SkillUserVec       = std::vector<SkillUser>                 ;
using PtrSkillVec        = std::vector<SkillData*>                ;
using PLvlPairVec        = std::vector<PersonaLevelPair>          ;
using CharIntMap         = std::unordered_map<const char*, int>   ;
using StrIntMap          = std::unordered_map<std::string, int>   ;
using SkillIDVec         = IntVec                                 ;
using IntVec2            = std::vector<IntVec>                    ;
using StrVec2            = std::vector<StrVec>                    ;
template<size_t _S>
using CharArr            = std::array<const char*, _S>;

enum GameTitles__
{
    GameTitle_NoTitle          = 0,
    GameTitle_Persona3FES      = 1 << 0,
    GameTitle_Persona3Portable = 1 << 1,
    GameTitle_Persona4         = 1 << 2,
    GameTitle_Persona4Golden   = 1 << 3,
    GameTitle_Persona5         = 1 << 4
};

enum ElementalWeakness__
{
    Normal,
    Weak,
    Strong,
    Reflect,
    Absorb
};

enum GetPersonaFlag_
{
    GetPersonaFlag_None                 = 0,
    GetPersonaFlag_Max                  = 1,                                                                       // <* Get only personas that can be gotten when you have maxed social link
    GetPersonaFlag_Special              = 1 << 1,                                                                  // <* Get only personas that are special
    GetPersonaFlag_Arcana               = 1 << 2,                                                                  // <* Get only personas of the specific arcana. Need to specify arcana parameter
    GetPersonaFlag_MaxArcana            = GetPersonaFlag_Arcana | GetPersonaFlag_Max,                              // <* Get only personas of the specific arcana that can only be gotten when you have maxed social link
    GetPersonaFlag_SpecialArcana        = GetPersonaFlag_Arcana | GetPersonaFlag_Special,                          // <* Get only personas of the specific arcana that are special
    GetPersonaFlag_SpecialMaxArcana     = GetPersonaFlag_Arcana | GetPersonaFlag_Special | GetPersonaFlag_Max      // <* Get only personas of the specific arcana that are special and can only be gotten when you have maxed social linl
};

enum GetSkillListFlag_
{
    GetSkillListFlag_None                       = 0,
    GetSkillListFlag_PersonaOfArcana            = 1 << 0,
    GetSkillListFlag_InnateOnlyOfArcana         = 1 << 1,
    GetSkillListFlag_NonInnateOnlyOfArcana      = 1 << 2,
    GetSkillListFlag_InnateNonInnateOfArcana    = 1 << 3,
    GetSkillListFlag_SkillCardOfArcana          = 1 << 4,
};

enum PersonaElement_
{
    PersonaElement_Slash     = 0 , PersonaElement_Sword    = 0,
    PersonaElement_Strike    = 1 , PersonaElement_Physical = 1,
    PersonaElement_Pierce    = 2 , PersonaElement_Gun      = 2,
    PersonaElement_Fire      = 3 ,
    PersonaElement_Ice       = 4 ,
    PersonaElement_Elec      = 5 ,
    PersonaElement_Wind      = 6 ,
    PersonaElement_Light     = 7 , PersonaElement_Bless    = 7,
    PersonaElement_Dark      = 8 , PersonaElement_Curse    = 8,
    PersonaElement_Almighty  = 9 ,
    PersonaElement_Ailment   = 10,
    PersonaElement_Healing   = 11,
    PersonaElement_Support   = 12,
    PersonaElement_Passive   = 13,
    PersonaElement_Nuke      = 14,
    PersonaElement_Undefined = 15
};

struct PersonaLevelPair
{
    int Persona;
    int Level;

    PersonaLevelPair() = default;
    PersonaLevelPair(const int& P, const int& L) :
        Persona(P), Level(L) {}

    constexpr void Reset()
    {
        Persona = -1;
        Level = 0;
    }
};

struct PersonaCombos
{
    int P1;
    int P2;
    int P3;
    int Result;

    PersonaCombos() = default;
    PersonaCombos(const int& _p1, const int& _p2, const int& _p3 = -1, const int& r = -1) :
        P1(_p1), P2(_p2), P3(_p3), Result(r)
    {}

    constexpr bool operator == (const PersonaCombos& P) const
    {
        if (P3 == -1)   return ((P1 == P.P1 && P2 == P.P2) || (P1 == P.P2 && P2 == P.P1)); // <* Normal Fusion Combo

        if (P1 == P.P1) return ((P2 == P.P2 && P3 == P.P3) || (P2 == P.P3 && P3 == P.P2)); // <* Triangle Fusion Combo
        if (P1 == P.P2) return ((P2 == P.P1 && P3 == P.P3) || (P2 == P.P3 && P3 == P.P1));
        if (P1 == P.P3) return ((P2 == P.P1 && P3 == P.P2) || (P2 == P.P2 && P3 == P.P1));

        return false;
    }

    constexpr bool operator != (const PersonaCombos& P) const
    {
        return !(*this == P);
    }
};


struct GameExclusives
{
    struct SkillCardData
    {
        int ID;
        int level = -1;

        constexpr void SetSkillCard(int _id, int _lvl)
        {
            ID = _id;
            level = _lvl;
        }
    };

    struct HeartItemData
    {
        std::string persona;
        int level;
        std::string item;

        HeartItemData() = default;
        HeartItemData(const std::string& _p, const int& _lvl, const std::string& _i) :
            persona(_p), level(_lvl), item(_i)
        {}
    };

    struct FusionSkillData
    {
        std::string persona1;
        std::string persona2;
        std::string skill;
        int cost;
        std::string description;
    };

    SkillCardData    SkillCard;
    HeartItemData    HeartItem;
    FusionSkillData  FusionSkill;
};

// Structure to store the persona id and level needed to unlock the corresponding skill it represents
struct SkillUser
{
    const PersonaData* Persona;
    int                SkillLevel;
    bool               SkillCard;

    SkillUser() = default;
    SkillUser(const PersonaData* _P, const int& _Lvl, const bool& is_skillcard) : 
        Persona(_P), SkillLevel(_Lvl), SkillCard(is_skillcard)
    {
    }
};

struct PersonaStats
{
    int Strength;
    int Magic;
    int Agility;
    int Endurance;
    int Luck;
};

struct ElementalAffinity
{
    IntVec Element;
    IntVec Affinity;

    constexpr void SetElements(const IntVec& elements)
    {
        Element = elements;
        Affinity.reserve(Element.size());

        for (const auto& E : Element)
            Affinity.emplace_back(0);
    }

    constexpr void SetAffinity(const int& element, const int& affinity)
    {
        bool stop = false;
        for (int i = 0; i < Element.size() && !stop; i++)
        {
            if (Element[i] == element)
            {
                Affinity[i] = affinity;
                stop = true;
            }
        }
    }
};

struct FusionData
{
    std::string Arcana1;
    std::string Arcana2;
    std::string ResultantArcana;

    FusionData() = default;
    FusionData(const std::string& a1, const std::string& a2) :
        Arcana1(a1), Arcana2(a2)
    {}
    FusionData(const std::string& a1, const std::string& a2, const std::string& result_a) :
        Arcana1(a1), Arcana2(a2), ResultantArcana(result_a)
    {}

    constexpr bool operator == (const FusionData& _F) const
    {
        return ( (Arcana1 == _F.Arcana1 && Arcana2 == _F.Arcana2) || (Arcana1 == _F.Arcana2 && Arcana2 == _F.Arcana1) );
    }
};

struct SpecialData
{
	std::string ResultantPersona;
	IntVec      PersonaCombos;

    SpecialData() = default;
    SpecialData(const std::string& p, const IntVec& pcombos):
        ResultantPersona(p), PersonaCombos(pcombos)
    {}

    constexpr bool operator == (const SpecialData& SP) const
    {
        return std::is_permutation(PersonaCombos.begin(), PersonaCombos.end(), SP.PersonaCombos.begin(), SP.PersonaCombos.end());
    }
};

struct PersonaData
{
    // For containing the skill id and level needed to unlock the skill since
    // every persona has varying level for unlocking a particular skill
    struct SkillAndLevel
    {
        int SkillID;
        int Level;

        SkillAndLevel(const int s, const int lvl) :
            SkillID(s), Level(lvl)
        {}
    };

	std::string                 Name;
	std::string                 Arcana;
	int                         Level;
    bool                        Max;
	bool                        Special;
    PersonaStats                Stats;
    ElementalAffinity           Elements;
    GameExclusives              GameEx;
    std::vector<SkillAndLevel>  Skills;

    PersonaData() = default;
    PersonaData(const std::string& name, const std::string& arcana, const int lvl, const bool& max, const bool& sp) :
        Name(name), Arcana(arcana), Level(lvl), Max(max), Special(sp)
    {}

    constexpr void SetElements(const IntVec& elements)
    {
        Elements.SetElements(elements);
    }

    constexpr void SetAffinity(const int& element, const int& affinity)
    {
        Elements.SetAffinity(element, affinity);
    }

    constexpr void SetStats(const int& str, const int& mag, const int& agi, const int& endu, const int& lck)
    {
        Stats.Strength  = str;
        Stats.Magic     = mag;
        Stats.Agility   = agi;
        Stats.Endurance = endu;
        Stats.Luck      = lck;
    }
};


struct SkillData
{
    int           ID;             // <* Skill ID
    std::string   Name;           // <* Name of the skill
    int           Element;        // <* Element type of the skill
    std::string   CostType;       // <* Cost type. Whether it cost SP or HP
    int           Cost;           // <* Cost of the skill
    int           ChanceValue;    // <* Pertains to the crit value or insta-kill chance value
    std::string   Description;    // <* Skill description
    bool          Inherit;        // <* Is the skill inheritable (can be transferred from persona to persona)
    int           Power;          // <* Power value of the skill, if there is any
    int           Accuracy;       // <* Accuracy of the skill, if there is any
    SkillUserVec  Personas;       // <* Personas that have the skill

    SkillData() = default;
    SkillData(
                  const int&         ID, 
                  const int&         type, 
                  const std::string& skill, 
                  const std::string& desc, 
                  const bool&        inherit, 
                  const std::string& cost_type = "N/A", 
                  const int&         cost      = 0, 
                  const int&         crit      = 0, 
                  const int&         power     = 0, 
                  const int&         accuracy  = 0
                 ) :
        ID(ID), 
        Name(skill), 
        Element(type), 
        CostType(cost_type), 
        Cost(cost), 
        ChanceValue(crit), 
        Description(desc), 
        Inherit(inherit), 
        Power(power), 
        Accuracy(accuracy)
    {}

    constexpr void AddPersonaWithSkill(const PersonaData* _P, const int& _Lvl, bool is_skillcard)
    {
        Personas.emplace_back(_P, _Lvl, is_skillcard);
    }

    constexpr bool operator < (const SkillData& T) const
    {
        return ID < T.ID;
    }
    
    constexpr bool operator == (const SkillData& T) const
    {
        return ID == T.ID;
    }
};


//------------------------------------------------------------
// MAIN GAME DATA CLASS
//------------------------------------------------------------
class Gamedata
{
public:
    PDataVec    Personas         ; // <* Contains the data of personas (name, arcana, level, special)
    FusionVec   NFusionData      ; // <* Contains the combinations of normal fusion of arcanas
    FusionVec   TFusionData      ; // <* Contains the combinations of triangle fusion of arcanas
    SpDataVec   SpFusionData     ; // <* Contains the combination for special personas
    StrVec      Arcana_List      ; // <* Contains the ranking of arcanas (Used for listing)
    StrVec      Persona_List     ; // <* Contains the names of the persona ( Used for listing )
    SkillVec    GameSkills       ; // <* Contains all the game's skill information (sorted by ID)
    IntVec      P_Elements       ; // <* Contains the persona's main elemental attribute
    IntVec      MainElements     ; // <* Contains the game's main elements
    CharArr<16> MainElementsChar ; // <* Contains the game's main elements (const char*s)(Used for listing)

    Gamedata()  = default;
    ~Gamedata() = default;
    bool InitializeGameData (const char* title);
    bool InitializeGameData (const int& title);
    void DestroyAll ();

    // LOADING FILES FUNCTION
    bool LoadFusionData    (const char* path);   // Loads the fusion data of the game
    bool LoadPersonaData   (const char* path);   // Loads the persona data of the game
    bool LoadSkillData     (const char* path);   // Loads the skill data of the game

    // SET FUNCTIONS
    bool SetGameElements    ();

    // GET FUNCTIONS
    void  GetNormalSpecials       (PComboVec& outVec) const;                          
    void  GetTriangleSpecials     (PComboVec& outVec) const;                          
    void  GetNormArcanaCombos     (const char* arcana, FusionVec& outVec) const; 
    void  GetTriArcanaCombos      (const char* arcana, FusionVec& outVec) const; 
    void  GetSkillList            (SkillIDVec& outVec, GetSkillListFlag flags, int skill_element = -1, const char* arcana = nullptr) const;
    int   GetGameTitle            () const;
    int   GetArcanaIndex          (const std::string& arc) const;
    int   GetPersonaIndex         (const std::string& name) const ;   
    void  GetFilteredPersonaList  (GetPersonaFilter flags, StrVec& outVec, const char* arcana = nullptr) const;

private:
    int         GameTitle;       // The current game title
    StrIntMap   HashTableSkill;  // The hash table for accessing skill. Used only for searching skills for adding a skill to the persona

    void  m_AddSkillToPersona     (const int& p_id, const std::string& s_name, const int& s_level, const bool& is_skillcard);
    void  m_GetMaxPersonas        (StrVec& outVec, GetPersonaFilter flags) const;
    void  m_GetSpPersonas         (StrVec& outVec, GetPersonaFilter flags) const;
    void  m_GetPersonaWithArcana  (StrVec& outVec, const char* arcana, GetPersonaFilter filterFlags) const;
    void  m_GetSpecialCombos      (StrVec2& outVec) const;
};
