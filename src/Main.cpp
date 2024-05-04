#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "mod_loader.h"
#include "cave_story.h"
#include "AutPI.h"
#include "lua/Lua.h"
#include "AutTriggers.h"

void InitMod(void)
{
	LoadAutPiDll();
	RegisterTriggerHooks();
	RegisterLuaFuncElement(SetTriggersLua);
}
