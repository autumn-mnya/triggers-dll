#include <Windows.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "Lua.h"


#include "../Main.h"
#include "../mod_loader.h"
#include "../cave_story.h"

#include "../AutPI.h"

#include "Lua_Trigger.h"

#define gL GetLuaL()

void SetTriggersLua()
{
	PushFunctionTable(gL, "Trigger", TriggerFunctionTable, FUNCTION_TABLE_TRIGGER_SIZE, FALSE);
	lua_newtable(gL);
	lua_pushvalue(gL, -1);
	lua_setfield(gL, -3, "Act");
	lua_pop(gL, 2);
}