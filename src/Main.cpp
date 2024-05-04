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

// This is rly rly janky
bool debug_trigger_hitbox_display = false;

// AutPI allowing stuff like this DLL to exist is genuinely so cool!!
// I'm so glad I made AutPI
void InitMod(void)
{
	LoadAutPiDll();
	debug_trigger_hitbox_display = ModLoader_GetSettingBool("Debug Trigger Hitbox display", false);
	RegisterTriggerHooks();
	RegisterLuaFuncElement(SetTriggersLua);
}