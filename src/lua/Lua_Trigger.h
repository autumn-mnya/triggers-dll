#pragma once

#include <Windows.h>
#include "../cave_story.h"

extern "C"
{
#include <lua.h>
}

#include "Lua.h"
#include "../AutPI.h"

#define FUNCTION_TABLE_TRIGGER_SIZE 4
extern FUNCTION_TABLE TriggerFunctionTable[FUNCTION_TABLE_TRIGGER_SIZE];

int TriggerModScript(int char_code);