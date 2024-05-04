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
#include "Lua_Trigger.h"

#include "../Main.h"
#include "../mod_loader.h"
#include "../cave_story.h"

#include "../AutPI.h"

#include "../AutTriggers.h"

static int lua_RegisterTrigger(lua_State* L)
{
	OTHER_RECT trigRect;
	int id = (int)luaL_checknumber(L, 1);
	int eve = (int)luaL_checknumber(L, 2);
	int lu = (int)luaL_checknumber(L, 3);
	int flag = (int)luaL_checknumber(L, 4);
	int x = (int)luaL_checknumber(L, 5);
	int y = (int)luaL_checknumber(L, 6);
	trigRect.front = (int)luaL_checknumber(L, 7);
	trigRect.top = (int)luaL_checknumber(L, 8);
	trigRect.back = (int)luaL_checknumber(L, 9);
	trigRect.bottom = (int)luaL_checknumber(L, 10);
	bool runEve = lua_toboolean(L, 11);
	bool flagS = lua_toboolean(L, 12);
	bool flagU = lua_toboolean(L, 13);
	RegisterTrigger(id, eve, flag, lu, x, y, trigRect, runEve, flagS, flagU);
	return 0;
}

static int lua_DeleteTrigger(lua_State* L)
{
	int num = (int)luaL_checknumber(L, 1);
	DeleteTrigger(num);
	return 0;
}

static int lua_ResetTriggers(lua_State* L)
{
	ResetTriggers();
	return 0;
}

static int lua_LoadTriggerYaml(lua_State* L)
{
	const char* user_path = luaL_checkstring(L, 1);
	char path[MAX_PATH];
	sprintf(path, "%s\\%s\\%s%s", gDataPath, "Stage", user_path, ".trig.yaml");
	return 0;
}

FUNCTION_TABLE TriggerFunctionTable[FUNCTION_TABLE_TRIGGER_SIZE] =
{
	{"Create", lua_RegisterTrigger},
	{"ResetAll", lua_ResetTriggers},
	{"LoadYaml", lua_LoadTriggerYaml},
	{"Delete", lua_DeleteTrigger}
};

int TriggerModScript(int char_code)
{
	lua_getglobal(GetLuaL(), "ModCS");
	lua_getfield(GetLuaL(), -1, "Trigger");
	lua_getfield(GetLuaL(), -1, "Act");
	lua_geti(GetLuaL(), -1, char_code);

	if (lua_isnil(GetLuaL(), -1))
	{
		char triggerString[0x10];
		sprintf(triggerString, "Act%d", char_code);
		lua_getfield(GetLuaL(), -3, triggerString);

		if (lua_isnil(GetLuaL(), -1))
		{
			lua_settop(GetLuaL(), 0); // Clear stack
			return 1;
		}
	}

	if (lua_pcall(GetLuaL(), 0, 0, 0) != LUA_OK)
	{
		const char* error = lua_tostring(GetLuaL(), -1);

		ErrorLog(error, 0);
		printf("ERROR: %s\n", error);
		return FALSE;
	}

	lua_settop(GetLuaL(), 0); // Clear stack

	return 2;
}