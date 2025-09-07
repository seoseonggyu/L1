#include "pch.h"
#include "InitManager.h"
#include "ClassManager.h"
#include "GameSessionManager.h"
#include "SkillManager.h"
#include "ClientPacketHandler.h"

#include "DBConnectionPool.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"


ClassManager*		GClassManager = nullptr;
GameSessionManager* GSessionManager = nullptr;
SkillManager*		GSkillManager = nullptr;


InitManager::InitManager()
{
	GClassManager	= new ClassManager();
	GSessionManager = new GameSessionManager();
	GSkillManager	= new SkillManager();

	ClientPacketHandler::Init();

	LoadFromDB();
}

InitManager::~InitManager()
{
	delete GClassManager;
	delete GSessionManager;
	delete GSkillManager;
}

void InitManager::LoadFromDB()
{
	ConnectAndSync();
	LoadCharacterClass();
	LoadSkills();
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

	int32 classType = 0;
	WCHAR className[40];
	float strength = 0.f;
	float defense = 0.f;
	float vigor = 0.f;
	float agility = 0.f;
	float resourceFulness = 0.f;

	getClass.Out_ClassType(classType);
	getClass.Out_ClassName(className);
	getClass.Out_Strength(strength);
	getClass.Out_Defense(defense);
	getClass.Out_Vigor(vigor);
	getClass.Out_Agility(agility);
	getClass.Out_ResourceFulness(resourceFulness);

	getClass.Execute();

	while (getClass.Fetch())
	{

		GConsoleLogger->WriteStdOut(
			Color::BLUE,
			L"classType[%d] className[%ls] str[%.2f] def[%.2f] vig[%.2f] agi[%.2f] res[%.2f]\n",
			classType, className, strength, defense, vigor, agility, resourceFulness
		);

		Protocol::CombatInfo info;
		info.set_strength(strength);
		info.set_defense(defense);
		info.set_vigor(vigor);
		info.set_agility(agility);
		info.set_resourcefulness(resourceFulness);
		GClassManager->Add(
			static_cast<Protocol::CharacterClassType>(classType),
			info
		);
	}
}

void InitManager::LoadSkills()
{
    DBConnectionPoolGuard dbConn(*GDBConnectionPool);
    SP::GetAllSkills getSkills(*dbConn);

    int32  skill_id = 0;
    int32  classType = 0;
    int32  skillType = 0;

    WCHAR  skill_code[64] = { 0 };
    WCHAR  skill_name[1000] = { 0 };
    WCHAR  description[1000] = { 0 };

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
    getSkills.Out_ClassType(classType);
    getSkills.Out_SkillType(skillType);
    getSkills.Out_Skill_code(skill_code);
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
            L"[Skill] classType[%d] skillType[%d] code[%ls] name[%ls]\n",
            classType, skillType, skill_code, skill_name
        );

        if (damage_flat != 0)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - damage_flat: %.2f\n", damage_flat);
        if (stun_sec > 0.f)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - stun_sec: %.2f\n", stun_sec);
        if (movespeed_pct != 0.f)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - movespeed_pct: %.2f%%\n", movespeed_pct);
        if (attackspeed_pct != 0.f)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - attackspeed_pct: %.2f%%\n", attackspeed_pct);
        if (dmg_reduction_pct != 0.f)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - dmg_reduction_pct: %.2f%%\n", dmg_reduction_pct);
        if (attack_power_flat != 0)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - attack_power_flat: %.2f\n", attack_power_flat);
        if (cooldown_reduction_sec != 0.f)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - cooldown_reduction_sec: %.2fs\n", cooldown_reduction_sec);
        if (lifesteal_pct != 0.f)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - lifesteal_pct: %.2f%%\n", lifesteal_pct);
        if (slow_pct != 0.f)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - slow_pct: %.2f%%\n", slow_pct);
        if (dot_maxhp_pct_per_sec != 0.f || dot_duration_sec != 0.f)
            GConsoleLogger->WriteStdOut(Color::GREEN, L"  - dot: %.2f%%/sec for %.2fs\n", dot_maxhp_pct_per_sec, dot_duration_sec);

        SkillRow row;
        row.classType = classType;
        row.skillType = skillType;
        row.skillCode = skill_code;
        row.skillName = skill_name;
        row.description = description;

        if (damage_flat != 0)                 row.damage_flat = damage_flat;
        if (stun_sec > 0.f)                   row.stun_sec = stun_sec;
        if (movespeed_pct != 0.f)             row.movespeed_pct = movespeed_pct;
        if (attackspeed_pct != 0.f)           row.attackspeed_pct = attackspeed_pct;
        if (dmg_reduction_pct != 0.f)         row.dmg_reduction_pct = dmg_reduction_pct;
        if (attack_power_flat != 0)           row.attack_power_flat = attack_power_flat;
        if (cooldown_reduction_sec != 0.f)    row.cooldown_reduction_sec = cooldown_reduction_sec;
        if (lifesteal_pct != 0.f)             row.lifesteal_pct = lifesteal_pct;
        if (slow_pct != 0.f)                  row.slow_pct = slow_pct;
        if (dot_maxhp_pct_per_sec != 0.f)     row.dot_maxhp_pct_per_sec = dot_maxhp_pct_per_sec;
        if (dot_duration_sec != 0.f)          row.dot_duration_sec = dot_duration_sec;

        GSkillManager->Add(row);

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
