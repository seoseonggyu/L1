#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

namespace SP
{
	
    class GetAllCharacterClass : public DBBind<0,7>
    {
    public:
    	GetAllCharacterClass(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetAllCharacterClass}") { }
    	void Out_ClassType(OUT int32& v) { BindCol(0, v); };
    	template<int32 N> void Out_ClassName(OUT WCHAR(&v)[N]) { BindCol(1, v); };
    	void Out_Strength(OUT float& v) { BindCol(2, v); };
    	void Out_Defense(OUT float& v) { BindCol(3, v); };
    	void Out_Vigor(OUT float& v) { BindCol(4, v); };
    	void Out_Agility(OUT float& v) { BindCol(5, v); };
    	void Out_ResourceFulness(OUT float& v) { BindCol(6, v); };

    private:
    };


     
};