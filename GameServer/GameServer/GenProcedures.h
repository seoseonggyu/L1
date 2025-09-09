#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

namespace SP
{
	
    class GetAllCharacterClass : public DBBind<0,8>
    {
    public:
    	GetAllCharacterClass(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetAllCharacterClass}") { }
    	void Out_ClassType(OUT int32& v) { BindCol(0, v); };
    	template<int32 N> void Out_ClassName(OUT WCHAR(&v)[N]) { BindCol(1, v); };
    	void Out_Hp(OUT float& v) { BindCol(2, v); };
    	void Out_Mp(OUT float& v) { BindCol(3, v); };
    	void Out_Strength(OUT float& v) { BindCol(4, v); };
    	void Out_Defense(OUT float& v) { BindCol(5, v); };
    	void Out_Agility(OUT float& v) { BindCol(6, v); };
    	void Out_Intelligence(OUT float& v) { BindCol(7, v); };

    private:
    };

    class GetAllSkills : public DBBind<0,17>
    {
    public:
    	GetAllSkills(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetAllSkills}") { }
    	void Out_Skill_id(OUT int32& v) { BindCol(0, v); };
    	void Out_ClassType(OUT int32& v) { BindCol(1, v); };
    	void Out_SkillType(OUT int32& v) { BindCol(2, v); };
    	template<int32 N> void Out_Skill_code(OUT WCHAR(&v)[N]) { BindCol(3, v); };
    	template<int32 N> void Out_Skill_name(OUT WCHAR(&v)[N]) { BindCol(4, v); };
    	template<int32 N> void Out_Description(OUT WCHAR(&v)[N]) { BindCol(5, v); };
    	void Out_Damage_flat(OUT float& v) { BindCol(6, v); };
    	void Out_Stun_sec(OUT float& v) { BindCol(7, v); };
    	void Out_Movespeed_pct(OUT float& v) { BindCol(8, v); };
    	void Out_Attackspeed_pct(OUT float& v) { BindCol(9, v); };
    	void Out_Dmg_reduction_pct(OUT float& v) { BindCol(10, v); };
    	void Out_Attack_power_flat(OUT float& v) { BindCol(11, v); };
    	void Out_Cooldown_reduction_sec(OUT float& v) { BindCol(12, v); };
    	void Out_Lifesteal_pct(OUT float& v) { BindCol(13, v); };
    	void Out_Slow_pct(OUT float& v) { BindCol(14, v); };
    	void Out_Dot_maxhp_pct_per_sec(OUT float& v) { BindCol(15, v); };
    	void Out_Dot_duration_sec(OUT float& v) { BindCol(16, v); };

    private:
    };


     
};