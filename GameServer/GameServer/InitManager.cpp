#include "pch.h"
#include "InitManager.h"
#include "ClassManager.h"
#include "GameSessionManager.h"
#include "SkillManager.h"
#include "MonsterManager.h"
#include "ItemManager.h"
#include "ClientPacketHandler.h"

#include "DBConnectionPool.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"

ClassManager*		GClassManager = nullptr;
GameSessionManager* GSessionManager = nullptr;
SkillManager*		GSkillManager = nullptr;
MonsterManager*     GMonsterManager = nullptr;
ItemManager*        GItemManager = nullptr;


InitManager::InitManager()
{
	GClassManager	= new ClassManager();
	GSessionManager = new GameSessionManager();
	GSkillManager	= new SkillManager();
    GMonsterManager = new MonsterManager();
    GItemManager =  new ItemManager();

	ClientPacketHandler::Init();

	LoadFromDB();
}

InitManager::~InitManager()
{
	delete GClassManager;
	delete GSessionManager;
	delete GSkillManager;
	delete GMonsterManager;
	delete GItemManager;
}

void InitManager::LoadFromDB()
{
	ConnectAndSync();
	LoadCharacterClass();
	LoadSkills();
    LoadMonsters();
    LoadItems();
}

void InitManager::ConnectAndSync()
{
	// SSG: Connect을 1개가 아니라 여러 개로 바꿔야 함(성능 문제)
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={SQL Server};Server=Master\\SQLEXPRESS;Database=GameDB;Trusted_Connection=Yes;"));

	DBConnectionPoolGuard dbConn(*GDBConnectionPool);
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");
}

void InitManager::LoadCharacterClass()
{
	DBConnectionPoolGuard dbConn(*GDBConnectionPool);
	SP::GetAllCharacterClass getClass(*dbConn);

	WCHAR className[40];
    float hp = 0.f;
    float mp = 0.f;
	float strength = 0.f;
	float defense = 0.f;
	float agility = 0.f;
	float intelligence = 0.f;

	getClass.Out_ClassName(className);
    getClass.Out_Hp(hp);
    getClass.Out_Mp(mp);
	getClass.Out_Strength(strength);
	getClass.Out_Defense(defense);
	getClass.Out_Agility(agility);
	getClass.Out_Intelligence(intelligence);

	getClass.Execute();

	while (getClass.Fetch())
	{
		GConsoleLogger->WriteStdOut(
			Color::BLUE,
			L"className[%ls], hp[%.2f], mp[%.2f] str[%.2f] def[%.2f] agi[%.2f] inte[%.2f]\n",
			className, hp, mp, strength, defense, agility, intelligence
        );

        Protocol::VitalInfo vitalInfo;
        vitalInfo.set_hp(hp);
        vitalInfo.set_max_hp(hp);
        vitalInfo.set_mp(mp);
        vitalInfo.set_max_mp(mp);

		Protocol::StatInfo statInfo;
		statInfo.set_strength(strength);
		statInfo.set_defense(defense);
		statInfo.set_agility(agility);
        statInfo.set_intelligence(intelligence);

        Protocol::CharacterClassType classType = CompareUtils::StringToClassName(className);
        
        GClassManager->AddVital(classType, vitalInfo);
        GClassManager->AddStat(classType, statInfo);
	}
}

void InitManager::LoadSkills()
{
    DBConnectionPoolGuard dbConn(*GDBConnectionPool);
    SP::GetAllSkills getSkills(*dbConn);

    int32  skill_id = 0;
    WCHAR  className[40]{};
    WCHAR  skillType[40]{};
    WCHAR  skill_name[100]{};
    WCHAR  description[1000]{};

    float  damage_flat = 0;
    float  stun_sec = 0.f;
    float  movespeed_pct = 0.f;
    float  attackspeed_pct = 0.f;
    float  dmg_reduction_pct = 0.f;
    float  attack_power_flat = 0;
    float  cooldown_reduction_sec = 0.f;
    float  lifesteal_pct = 0.f;
    float  slow_pct = 0.f;
    float  dot_maxhp_pct_per_sec = 0.f;
    float  dot_duration_sec = 0.f;

    getSkills.Out_Skill_id(skill_id);
    getSkills.Out_ClassName(className);
    getSkills.Out_SkillType(skillType);
    getSkills.Out_Skill_name(skill_name);
    getSkills.Out_Description(description);

    getSkills.Out_Damage_flat(damage_flat);
    getSkills.Out_Stun_sec(stun_sec);
    getSkills.Out_Movespeed_pct(movespeed_pct);
    getSkills.Out_Attackspeed_pct(attackspeed_pct);
    getSkills.Out_Dmg_reduction_pct(dmg_reduction_pct);
    getSkills.Out_Attack_power_flat(attack_power_flat);
    getSkills.Out_Cooldown_reduction_sec(cooldown_reduction_sec);
    getSkills.Out_Lifesteal_pct(lifesteal_pct);
    getSkills.Out_Slow_pct(slow_pct);
    getSkills.Out_Dot_maxhp_pct_per_sec(dot_maxhp_pct_per_sec);
    getSkills.Out_Dot_duration_sec(dot_duration_sec);

    getSkills.Execute();

    while (getSkills.Fetch())
    {
        GConsoleLogger->WriteStdOut(
            Color::GREEN,
            L"className[%ls] skillType[%ls]  name[%ls]\n",
            className, skillType, skill_name
        );
        SkillValue value;
        value.skillID = skill_id;
        value.className = className;
        value.skillType = skillType;
        value.skillName = skill_name;
        value.description = description;

        if (damage_flat != 0)                 value.damage_flat = damage_flat;
        if (stun_sec > 0.f)                   value.stun_sec = stun_sec;
        if (movespeed_pct != 0.f)             value.movespeed_pct = movespeed_pct;
        if (attackspeed_pct != 0.f)           value.attackspeed_pct = attackspeed_pct;
        if (dmg_reduction_pct != 0.f)         value.dmg_reduction_pct = dmg_reduction_pct;
        if (attack_power_flat != 0)           value.attack_power_flat = attack_power_flat;
        if (cooldown_reduction_sec != 0.f)    value.cooldown_reduction_sec = cooldown_reduction_sec;
        if (lifesteal_pct != 0.f)             value.lifesteal_pct = lifesteal_pct;
        if (slow_pct != 0.f)                  value.slow_pct = slow_pct;
        if (dot_maxhp_pct_per_sec != 0.f)     value.dot_maxhp_pct_per_sec = dot_maxhp_pct_per_sec;
        if (dot_duration_sec != 0.f)          value.dot_duration_sec = dot_duration_sec;

        Protocol::CharacterClassType classType = CompareUtils::StringToClassName(className);

        GSkillManager->Add(classType, value);

        // DB에서 NULL 값을 허용해서 0으로 초기화
        damage_flat = 0;
        stun_sec = 0.f;
        movespeed_pct = 0.f;
        attackspeed_pct = 0.f;
        dmg_reduction_pct = 0.f;
        attack_power_flat = 0;
        cooldown_reduction_sec = 0.f;
        lifesteal_pct = 0.f;
        slow_pct = 0.f;
        dot_maxhp_pct_per_sec = 0.f;
        dot_duration_sec = 0.f;

    }
    GConsoleLogger->WriteStdOut(Color::BLUE, L"[Skill] total cached: %d\n", GSkillManager->GetSize());
}

void InitManager::LoadMonsters()
{
    DBConnectionPoolGuard dbConn(*GDBConnectionPool);
    SP::GetAllMonsters getMonsters(*dbConn);

    int32 monsterID = 0;
    WCHAR monsterName[40]{};
    float hp = 0;
    float damage = 0;

    getMonsters.Out_Monster_id(monsterID);
    getMonsters.Out_MonsterName(monsterName);
    getMonsters.Out_Hp(hp);
    getMonsters.Out_Damage(damage);

    getMonsters.Execute();

    while (getMonsters.Fetch())
    {
        GConsoleLogger->WriteStdOut(
            Color::WHITE,
            L"monsterName[%ls], hp[%.2f], damage[%.2f] \n",
            monsterName, hp, damage
        );

        MonsterInfo monsterInfo;
        monsterInfo.monsterID = monsterID;
        monsterInfo.monsterName = monsterName;
        monsterInfo.hp = hp;
        monsterInfo.damage = damage;

        Protocol::MonsterType monsterType = CompareUtils::StringToMonsterName(monsterName);
        GMonsterManager->Add(monsterType, monsterInfo);
    }
}

void InitManager::LoadItems()
{
    DBConnectionPoolGuard dbConn(*GDBConnectionPool);
    SP::GetAllItems getItems(*dbConn);

    int32 item_id = 0;
    WCHAR itemName[40]{};

    float damage = 0.f;
    float strength = 0.f;
    float defense = 0.f;
    float agility = 0.f;
    float intelligence = 0.f;


    getItems.Out_Item_id(item_id);
    getItems.Out_ItemName(itemName);
    getItems.Out_Damage(damage);
    getItems.Out_Strength(strength);
    getItems.Out_Defense(defense);
    getItems.Out_Agility(agility);
    getItems.Out_Intelligence(intelligence);

    getItems.Execute();

    while (getItems.Fetch())
    {
        /*GConsoleLogger->WriteStdOut(
            Color::GREEN,
            L"itemName[%ls] damage[%.2f]  strength[%.2f]  defense[%.2f]  agility[%.2f]  intelligence[%.2f]\n",
            itemName, damage, strength, defense, agility, intelligence
        );*/

        ItemInfo itemInfo;
        itemInfo.itemID = item_id;
        itemInfo.itemName = itemName;

        itemInfo.damage = damage;
        itemInfo.strength = strength;
        itemInfo.defense = defense;
        itemInfo.agility = agility;
        itemInfo.intelligence = intelligence;

        if (damage != 0)            itemInfo.damage = damage;
        if (strength > 0.f)         itemInfo.strength = strength;
        if (defense != 0.f)         itemInfo.defense = defense;
        if (agility != 0.f)         itemInfo.agility = agility;
        if (intelligence != 0.f)    itemInfo.intelligence = intelligence;

        damage = 0.f;
        strength = 0.f;
        defense = 0.f;
        agility = 0.f;
        intelligence = 0.f;

        GItemManager->Add(item_id, itemInfo);
    }
}