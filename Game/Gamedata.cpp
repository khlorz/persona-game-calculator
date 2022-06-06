#include "Gamedata.h"
#include "xxstr.h"
#include <fstream>
#include <unordered_set>
#include <algorithm>

void Gamedata::DestroyAll()
{
	IntVec().swap    (MainElements);
	StrVec().swap    (Arcana_List);
	StrVec().swap    (Persona_List);
    PDataVec().swap  (Personas);
	SkillVec().swap  (GameSkills);
    FusionVec().swap (NFusionData);
    FusionVec().swap (TFusionData);
    SpDataVec().swap (SpFusionData);
}





//--------------------------------------------------
// SET FUNCTIONS
//--------------------------------------------------
// Typical set functions for a few values
// Setting the Game Title can be in two ways, through the enum values (i.e. GameTitle_Persona3Portable) or the string of the title itself
// TO-DO:
// * Nothing yet...

bool Gamedata::InitializeGameData(const char* title)
{
	if (title == "Persona 3 FES")        
		return InitializeGameData(GameTitle_Persona3FES);

	if (title == "Persona 3 Portable")   
		return InitializeGameData(GameTitle_Persona3Portable);

	if (title == "Persona 4")            
		return InitializeGameData(GameTitle_Persona4);

	if (title == "Persona 4 Golden")     
		return InitializeGameData(GameTitle_Persona4Golden);

	if (title == "Persona 5")            
		return InitializeGameData(GameTitle_Persona5);

	return InitializeGameData(GameTitle_NoTitle);
}

bool Gamedata::InitializeGameData(const int& title)
{
	GameTitle = title;
	return SetGameElements();
}

bool Gamedata::SetGameElements()
{
	if (GameTitle & (GameTitle_Persona3FES | GameTitle_Persona3Portable))
	{
		P_Elements  = { 
					    PersonaElement_Slash, 
						PersonaElement_Strike,
						PersonaElement_Pierce,
						PersonaElement_Fire,
						PersonaElement_Ice,
						PersonaElement_Elec,
						PersonaElement_Wind,
					    PersonaElement_Light, 
					    PersonaElement_Dark 
					  };
		MainElementsChar = {
				           "Slash",
				           "Strike",
				           "Pierce",
				           "Fire",
				           "Ice",
				           "Elec",
				           "Wind",
				           "Light",
				           "Dark",
						   "Almighty",
						   "Ailment",
						   "Healing",
						   "Support",
						   "Passive",
						   "N/A",     
						   "Undefined"
					     };
		MainElements = {
						 PersonaElement_Slash,
						 PersonaElement_Strike,
						 PersonaElement_Pierce,
						 PersonaElement_Fire,
						 PersonaElement_Ice,
						 PersonaElement_Elec,
						 PersonaElement_Wind,
						 PersonaElement_Light,
						 PersonaElement_Dark,
						 PersonaElement_Almighty,
						 PersonaElement_Ailment,
						 PersonaElement_Healing,
						 PersonaElement_Support,
						 PersonaElement_Passive,
						 PersonaElement_Undefined
					   };

		return true;
	}

	if (GameTitle & (GameTitle_Persona4 | GameTitle_Persona4Golden))
	{
		P_Elements  = { 
					    PersonaElement_Physical, 
					    PersonaElement_Fire, 
					    PersonaElement_Ice, 
					    PersonaElement_Elec, 
					    PersonaElement_Wind, 
					    PersonaElement_Light, 
					    PersonaElement_Dark 
					  };
		MainElementsChar = {
						   "N/A",
				           "Physical",
						   "N/A",
				           "Fire",
				           "Ice",
				           "Elec",
				           "Wind",
				           "Light",
				           "Dark",
						   "Almighty",
						   "Ailment",
						   "Healing",
						   "Support",
						   "Passive",
						   "N/A",     // No Nuke
						   "Undefined"
					     };
		MainElements = {
						 PersonaElement_Physical,
						 PersonaElement_Fire,
						 PersonaElement_Ice,
						 PersonaElement_Elec,
						 PersonaElement_Wind,
						 PersonaElement_Light,
						 PersonaElement_Dark,
						 PersonaElement_Almighty,
						 PersonaElement_Ailment,
						 PersonaElement_Healing,
						 PersonaElement_Support,
						 PersonaElement_Passive,
						 PersonaElement_Undefined
					   };

		return true;
	}

	if (GameTitle & GameTitle_Persona5)
	{
		P_Elements  = { 
					    PersonaElement_Physical,
						PersonaElement_Gun,
					    PersonaElement_Fire, 
					    PersonaElement_Ice, 
					    PersonaElement_Elec, 
					    PersonaElement_Wind, 
					    PersonaElement_Light, 
					    PersonaElement_Dark 
					  };
		MainElementsChar = {
						   "N/A",
				           "Physical",
						   "Gun",
				           "Fire",
				           "Ice",
				           "Elec",
				           "Wind",
				           "Light",
				           "Dark",
						   "Almighty",
						   "Ailment",
						   "Healing",
						   "Support",
						   "Passive",
						   "Nuke",
						   "Undefined"
					     };
		MainElements = {
						 PersonaElement_Physical,
						 PersonaElement_Gun,
						 PersonaElement_Fire,
						 PersonaElement_Ice,
						 PersonaElement_Elec,
						 PersonaElement_Wind,
						 PersonaElement_Light,
						 PersonaElement_Dark,
						 PersonaElement_Almighty,
						 PersonaElement_Ailment,
						 PersonaElement_Healing,
						 PersonaElement_Support,
						 PersonaElement_Passive,
						 PersonaElement_Nuke,
						 PersonaElement_Undefined
					   };

		return true;
	}
	
	// If all else fails return false and undefined elements
	P_Elements = { PersonaElement_Undefined };
	MainElementsChar = { "Undefined" };

	return false;
}





//-------------------------------------------------------
// GET FUNCTIONS
//-------------------------------------------------------
// Typical get functions for certain values
// AddSkillToPersona is just GetSkillData with added lines of code to suit the function's purpose
// TO-DO:
// * GetSkillData can be quite slow when it comes to performance when done in a loop because of copying and creating vectors of PersonaSkills which can be really expensive
//   Must find a way to make a better code for this one. Pointers can be the only way to make it easier
// * Make a faster algorithm for finding the specific persona or arcana in the list instead of brute forcing it. 
//   Hashing would just be a waste of space and not really suitable when used for calculating personas and creating recipes
//

void Gamedata::GetNormalSpecials(PComboVec& outVec) const
{
	outVec.clear();

	for (const auto& SP : SpFusionData)
		if (SP.PersonaCombos.size() == 2)
			outVec.emplace_back(SP.PersonaCombos[0], SP.PersonaCombos[1], -1, GetPersonaIndex(SP.ResultantPersona));
}

void Gamedata::GetTriangleSpecials(PComboVec& outVec) const
{
	outVec.clear();

	for (const auto& SP : SpFusionData)
		if (SP.PersonaCombos.size() == 3)
			outVec.emplace_back(SP.PersonaCombos[0], SP.PersonaCombos[1], SP.PersonaCombos[2], GetPersonaIndex(SP.ResultantPersona));
}

int Gamedata::GetGameTitle() const
{
	return GameTitle;
}

int Gamedata::GetPersonaIndex (const std::string& name) const
{
	const int& sz = Personas.size();

	for (int i = 0; i < sz; i++)
	{
		if (Personas[i].Name == name)
			return i;
	}
	
	return -1;
}

int Gamedata::GetArcanaIndex(const std::string& arcana) const
{
	const int& sz = Personas.size();

	for (int i = 0; i < sz; i++)
	{
		if (Personas[i].Arcana == arcana)
			return i;
	}
			
	return - 1;
}

void Gamedata::m_AddSkillToPersona(const int& p_id, const std::string& s_name, const int& s_level, const bool& is_skillcard)
{
	// auto = std::unordered_map<> iterator
	// skill->first  = const char* name of the skill
	// skill->second = int ID of the skill
	auto skill = HashTableSkill.find(s_name);
	if (skill == HashTableSkill.end())
	{
		Personas[p_id].Skills.emplace_back( (int)GameSkills.size() - 1, s_level ); 
		return;
	}

	Personas[p_id].Skills.emplace_back( skill->second, s_level );                             // Add the skill to the vector of skills
	GameSkills[skill->second].AddPersonaWithSkill(&(Personas[p_id]), s_level, is_skillcard);  // Add the persona_id to the vector of persona with the same skill

	if (is_skillcard)
		Personas[p_id].GameEx.SkillCard.SetSkillCard(GameSkills[skill->second].ID, s_level);
}

void Gamedata::GetNormArcanaCombos(const char* arcana, FusionVec& outVec) const
{
	outVec.clear();

	for (const auto& NF : NFusionData)
		if (NF.ResultantArcana == arcana)
			outVec.emplace_back(NF.Arcana1, NF.Arcana2);
}

void Gamedata::GetTriArcanaCombos(const char* arcana, FusionVec& outVec) const
{
	outVec.clear();

	for (const auto& TF : TFusionData)
		if (TF.ResultantArcana == arcana)
			outVec.emplace_back(TF.Arcana1, TF.Arcana2);
}

void Gamedata::m_GetSpecialCombos(StrVec2& outVec) const
{
	const int& sp_sz = SpFusionData.size();
	outVec.resize(sp_sz);

	for (int i = 0; i < sp_sz; i++)
		for (auto& P : SpFusionData[i].PersonaCombos)
			outVec[i].push_back(Personas[P].Name);
}

//------------------------------------------------------------------------------------------------
// GET PERSONA FUNCTIONS
//------------------------------------------------------------------------------------------------

typedef bool(*MaxSpCondition)(const PersonaData&);
static constexpr bool IsPersonaMax(const PersonaData& P)           { return P.Max; }
static constexpr bool IsPersonaSpecial(const PersonaData& P)       { return P.Special; }
static constexpr bool IsPersonaMaxAndSpecial(const PersonaData& P) { return P.Max && P.Special; }

void Gamedata::m_GetMaxPersonas(StrVec& outVec, GetPersonaFilter FilterFlags) const
{
	MaxSpCondition FilterCondition = IsPersonaMax;
	
	if (FilterFlags & GetPersonaFlag_Special)
	{
		FilterCondition = IsPersonaMaxAndSpecial;
	}

	for (const auto& P : Personas)
		if (FilterCondition(P))
			outVec.emplace_back(P.Name);
}


void Gamedata::m_GetSpPersonas(StrVec& outVec, GetPersonaFilter FilterFlags) const
{
	if (FilterFlags & GetPersonaFlag_Max)
	{
		for (const auto& SP : SpFusionData)
		{
			const int& idx = GetPersonaIndex(SP.ResultantPersona);
			if (Personas[idx].Max)
				outVec.emplace_back(Personas[idx].Name);
		}

		return;
	}

	for (const auto& SP : SpFusionData)
		outVec.emplace_back(SP.ResultantPersona);
}

void Gamedata::m_GetPersonaWithArcana(StrVec& outVec, const char* arcana, GetPersonaFilter flags) const
{
	const int& start_idx = GetArcanaIndex(arcana);
	if (start_idx < 0)
		return;

	const int& p_sz = Personas.size();
	MaxSpCondition FilterCondition;

	if ((flags & (GetPersonaFlag_Max | GetPersonaFlag_Special)) == (GetPersonaFlag_Max | GetPersonaFlag_Special))
	{
		FilterCondition = IsPersonaMaxAndSpecial;
	}
	else if ((flags & GetPersonaFlag_Max))
	{
		FilterCondition = IsPersonaMax;
	}
	else if ((flags & GetPersonaFlag_Special))
	{
		FilterCondition = IsPersonaSpecial;
	}
	else // Therefore, there is no filter or an invalid filter input which is still equivalent to no filter
	{
		for (int i = start_idx; i < p_sz && Personas[i].Arcana == arcana; i++)
			outVec.emplace_back(Personas[i].Name);

		return;
	}

	for (int i = start_idx; i < p_sz && Personas[i].Arcana == arcana; i++)
	{
		if (FilterCondition(Personas[i]))
		{
			outVec.emplace_back(Personas[i].Name);
		}
	}
}

void Gamedata::GetFilteredPersonaList(GetPersonaFilter flags, StrVec& outVec, const char* arcana) const
{
	if (flags & GetPersonaFlag_Arcana)
	{
		if (arcana == nullptr)
			return;

		m_GetPersonaWithArcana(outVec, arcana, flags);

		return;
	}

	if (flags & GetPersonaFlag_Max)
	{
		m_GetMaxPersonas(outVec, flags);

		return;
	}

	if (flags & GetPersonaFlag_Special)
	{
		m_GetSpPersonas(outVec, flags);

		return;
	}

	// If no flags passed the condition, then no filter is set
	// so the whole persona list is then assigned to outVec
	outVec = Persona_List;
}





//----------------------------------------------------------------------------------------
// GET SKILL FUNCTIONS
//----------------------------------------------------------------------------------------

static constexpr bool IsOfArcana(const SkillUser& P, const char* a2)
{
	return P.Persona->Arcana == a2;
}

static constexpr bool IsInnateOfArcana(const SkillUser& P, const char* a2)
{
	if(a2 == nullptr)
		return P.SkillLevel == 0;

	return P.Persona->Arcana == a2 && P.SkillLevel == 0;
}

static constexpr bool IsNonInnateOfArcana(const SkillUser& P, const char* a2)
{
	if (a2 == nullptr)
		return P.SkillLevel > 0;

	return P.Persona->Arcana == a2 && P.SkillLevel > 0;
}

static constexpr bool IsInnateNonInnateOfArcana(const SkillUser& P, const char* a2)
{
	if (a2 == nullptr)
		return P.SkillLevel >= 0;

	return P.Persona->Arcana == a2 && P.SkillLevel >= 0;
}

static constexpr bool IsSkillCardOfArcana(const SkillUser& P, const char* a2)
{
	if (a2 == nullptr)
		return P.SkillCard;

	return P.Persona->Arcana == a2 && P.SkillCard;
}

//-----------------------------------------------------------------------------------------------------------
// FIX-ME!!!
//-----------------------------------------------------------------------------------------------------------
// This function is still not really that good because while it filters out the skills that doesn't pass the filter condition,
// it does not filter out the other personas in the SkillUser vector that does not pass the condition. Still need more
// work on this and probably will need a new way to store the filtered vector in better way.

void Gamedata::GetSkillList(SkillIDVec& outSkillIDVec, GetPersonaFilter flags, int skill_element, const char* arcana) const
{
	// Clear the vector. Not doing the swap method so the capacity remains for reuse since reusing the vector is quite often
	outSkillIDVec.clear();

	bool(*FilterCondition)(const SkillUser&, const char*);

	switch (flags)
	{
	case GetSkillListFlag_PersonaOfArcana:            
		FilterCondition = IsOfArcana;                   
		break;

	case GetSkillListFlag_InnateOnlyOfArcana:         
		FilterCondition = IsInnateOfArcana;             
		break;

	case GetSkillListFlag_NonInnateOnlyOfArcana:      
		FilterCondition = IsNonInnateOfArcana;          
		break;

	case GetSkillListFlag_InnateNonInnateOfArcana:    
		FilterCondition = IsInnateNonInnateOfArcana;	
		break;

	case GetSkillListFlag_SkillCardOfArcana:          
		FilterCondition = IsSkillCardOfArcana; 
		break;

	default:
		if (skill_element < 0)
		{
			for (const SkillData& S : GameSkills)
				outSkillIDVec.emplace_back(S.ID);

			return;
		}

		for (const SkillData& S : GameSkills)
			if (S.Element == skill_element)
				outSkillIDVec.emplace_back(S.ID);

		return;
	}

	if (skill_element < 0)
	{
		for (const SkillData& S : GameSkills) {
			for (const SkillUser& P : S.Personas) {
				if (FilterCondition(P, arcana))
				{
					outSkillIDVec.emplace_back(S.ID);
					break;
				}
			}
		}

		return;
	}

	for (const SkillData& S : GameSkills) {
		if (S.Element == skill_element)
		{
			for (const SkillUser& P : S.Personas) {
				if (FilterCondition(P, arcana))
				{
					outSkillIDVec.emplace_back(S.ID);
					break;
				}
			}
		}
	}
}





//-----------------------------------------------------------
// LOADING FILES FUNCTIONS
//-----------------------------------------------------------
// Functions that loads the specific files for specific data. Separated for easier reading.
// Loading Fusion Data is already as optimized as I possibly can
// TO-DO: 
// * Loading Persona Data is considerably slow due to setting elemental affinities and hashing skills. If possible to make this faster, it will be done in the future
// * Still haven't added a rehashing of skills in case the total amount of skills is more than 512 (Might not be neccesary but who know if Atlus make a game with a total number of skills of more than 512 in the future)

bool Gamedata::LoadFusionData(const char* path)
{
    std::ifstream DataFile( path );

    if( !DataFile.is_open() || DataFile.fail() )
        return false;

	int end_flags = 0;
	uint64_t p_sz = 1ULL << 9;

	Personas.reserve     (p_sz);
	Persona_List.reserve (p_sz);
	Arcana_List.reserve  (p_sz);
	NFusionData.reserve  (p_sz);
	TFusionData.reserve  (p_sz);
	SpFusionData.reserve (p_sz);

	StrVec token_str; token_str.resize(20);
	IntVec spIndex  ; spIndex.reserve(12);
    std::string data; data.reserve(p_sz);

	while( getline(DataFile, data) && !DataFile.fail() )
	{
		if (data.size() < 2 || std::string_view(data.c_str(), 2) == "//")
			continue;

		if (std::string_view(data.c_str(), 3) == "End") // Increments end_flag to switch the data to fill up
		{
			end_flags++;
			continue;
		}

		int vecsize = 0;
		xxstr::token_repeat(data.c_str(), token_str, vecsize);

		switch (end_flags)
		{
		case 0:
		{
			int level;
			bool max = false;
			bool special = false;

			if (vecsize == 4)
			{
				if (token_str[3] == "Max")
					max = true;
				else
					special = true;
			}
			else if (vecsize == 5)
			{
				max     = true;
				special = true;
			}

			level = std::stoi(token_str[2]); // Extracts the number
			Personas.emplace_back( token_str[0], token_str[1], level, max, special );
			Persona_List.emplace_back( token_str[0] );
			break;
		}

		case 1:
			Arcana_List.emplace_back( token_str[0] );
			break;

		case 2:
			NFusionData.emplace_back( token_str[0], token_str[1], token_str[2] );
			break;

		case 3:
			TFusionData.emplace_back( token_str[0], token_str[1], token_str[2] );
			break;

		case 4:
			spIndex.clear();

			for (int i = 1; i < vecsize; i++)
			{
				spIndex.emplace_back(GetPersonaIndex(token_str[i]));
			}

			SpFusionData.emplace_back(token_str[0], spIndex);
			break;

		default:
			break;
		}
	}
    DataFile.close();

	Personas.shrink_to_fit();
	Persona_List.shrink_to_fit();
	Arcana_List.shrink_to_fit();
	NFusionData.shrink_to_fit();
	TFusionData.shrink_to_fit();
	SpFusionData.shrink_to_fit();

    return true;
}

bool Gamedata::LoadSkillData(const char* path)
{
	std::ifstream DataFile(path);

	if (!DataFile.is_open() || DataFile.fail())
		return false;

	int SkillID     = 0;
	int SkillType   = 0;
	GameSkills.reserve(1 << 9);
	std::string data; data.reserve(1 << 9);
	StrVec token_str; token_str.resize(20);

	while (getline(DataFile, data) && !DataFile.fail())
	{
		if (data.size() < 2 || std::string_view(data.c_str(), 2) == "//")
			continue;

		int vecsize = 0;
		xxstr::token_repeat(data.c_str(), token_str, vecsize);

		if (token_str[0] == "Type")
		{
			SkillType = std::stoi(token_str[1]);
			continue;
		}

		// token_str[0] = Skill Name
		// token_str[1] = Skill Description
		// if not a Passive Skill
		//    token_str[2] = Skill Cost Type (HP or SP)
		//    token_str[3] = Skill Cost
		//    if SkillType has no chance value but has power and accuracy
		//	     token_str[4] = Base Power
		//	     token_str[5] = Accuracy
		//    else
		//       token_str[4] = Chance based skill type (Crit or Insta-kill chance)
		//       token_str[5] = Chance % value
		//       token_str[6] = Base Power Value for physical & magical skills ; token_str[6] = Accuracy of the Insta-Kill skills 
		//       token_str[7] = Accuracy
		// At the end of the vector, if the string is 'non-inherit' set the inherit to false
		bool inherit = true;

		if (token_str[vecsize - 1] == "non-inherit")
			inherit = false;

		if (SkillType == PersonaElement_Passive || SkillType == PersonaElement_Undefined)
		{
			GameSkills.emplace_back(SkillID, SkillType, token_str[0], token_str[1], inherit);
			HashTableSkill[GameSkills[SkillID].Name] = SkillID;
		}
		else
		{
			int cost     = std::stoi(token_str[3]);
			int chance   = 0; // Can be crit chance or insta-kill chance
			int power    = 0;
			int accuracy = 0;
			if (SkillType == PersonaElement_Light || SkillType == PersonaElement_Dark || SkillType <= PersonaElement_Pierce)
			{
				if (vecsize > 5 && token_str[5] != "non-inherit")
					chance = stoi(token_str[5]);
			}
			if (SkillType <= PersonaElement_Wind || SkillType == PersonaElement_Almighty)
			{
				if (SkillType <= PersonaElement_Pierce)
				{
					power    = std::stoi(token_str[6]);
					accuracy = std::stoi(token_str[7]);
				}
				else if (vecsize > 4 && token_str[4] != "non-inherit")
				{
					power    = std::stoi(token_str[4]);
					accuracy = std::stoi(token_str[5]);
				}
			}
			GameSkills.emplace_back(SkillID, SkillType, token_str[0], token_str[1], inherit, token_str[2], cost, chance, power, accuracy);
			HashTableSkill[GameSkills[SkillID].Name] = SkillID;
		}

		SkillID++;
	}

	DataFile.close();

	GameSkills.shrink_to_fit();

	return true;
}

bool Gamedata::LoadPersonaData(const char* path)
{
	std::ifstream DataFile(path);

	if (!DataFile.is_open() || DataFile.fail())
		return false;

	int  p_ID = -1;
	bool file_stop = false;
	int  CurrentPersonaData = 0;
	std::string data;
	data.reserve(1 << 9);

	enum PersonaOtherData_
	{
		Abilities,
		Stats,
		ElementalAffinity,
		GameEx
	};

	StrVec temp; temp.resize(20);

	while (getline(DataFile, data) && !file_stop)
	{
		if (data.size() < 2 || std::string_view(data.c_str(), 2) == "//")
			continue;

		int vecsize = 0;
		xxstr::token_repeat(data.c_str(), temp, vecsize);

		if (temp[0] == "Persona")
		{
			p_ID = GetPersonaIndex(temp[1]);
			CurrentPersonaData = 0;
			Personas[p_ID].SetElements(P_Elements);

			continue;
		}

		if (CurrentPersonaData == Abilities)
		{
			if (temp[0] == "Stats")
			{
				CurrentPersonaData = Stats;
			}
			else if (temp[0] != "Skills")
			{
				// token_str[0] = skill name
				// token_str[1] = skill level
				// token_str[2] = skill card flag
				int  level;
				bool is_skillcard = false;
				level = std::stoi(temp[1]);
				if (vecsize == 3)
					is_skillcard = true;

				m_AddSkillToPersona(p_ID, temp[0], level, is_skillcard);
			}
		}
		else if (CurrentPersonaData == Stats)
		{
			if (temp[0] == "Elemental Affinity")
			{
				CurrentPersonaData = ElementalAffinity;
			}
			else
			{
				int str, mag, agi, endu, lck;
				str	 = std::stoi(temp[0]);
				mag	 = std::stoi(temp[1]);
				agi	 = std::stoi(temp[2]);
				endu = std::stoi(temp[3]);
				lck  = std::stoi(temp[4]);
				Personas[p_ID].SetStats(str, mag, agi, endu, lck);
			}
		}
		else if (CurrentPersonaData == ElementalAffinity)
		{
			if (temp[0] == "Game Ex")
			{
				CurrentPersonaData = GameEx;
			}
			
			if (vecsize % 2)
			{
				return false;
			}
			else
			{
				int elem, aff;
				for (int i = 0; i < vecsize; i += 2)
				{
					// Find the corresponding element
					// Then set the affinity
					for (const auto& PE : P_Elements)
					{
						if (MainElementsChar[PE] == temp[i])
						{
							elem = PE;
							aff  = stoi(temp[i + 1]);
							Personas[p_ID].SetAffinity(elem, aff);
							break;
						}
					}
				}
			}
		}
		else if (CurrentPersonaData == GameEx)
		{
			//PersonaData_2[p_ID].SetGameEx( GetGameTitle(), token_str );
		}
	}

	DataFile.close();

	// In case of error, return false for early exit
	if (file_stop)
		return false;

	std::unordered_map<std::string, int>().swap(HashTableSkill);

	return true;
}



