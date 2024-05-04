#include <Windows.h>
#include <shlwapi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "main.h"
#include "AutTriggers.h"

#include "AutPI.h"
#include "mod_loader.h"
#include "cave_story.h"

#include <yaml-cpp/yaml.h>
#include "lua/Lua_Trigger.h"

static STAGE_TABLE* mTMT = (STAGE_TABLE*)(*(unsigned*)0x420C2F); // This is a pointer to where it gets used, instead of the actual table, so that it has compatibility with mods.

TRIGGER gTrig[MAX_TRIGGER];

void RegisterTrigger(int id, int eve, int flag, int x, int y, OTHER_RECT rec, bool runEve, bool flagS, bool flagU)
{
	gTrig[id].code_event = eve;
	gTrig[id].code_flag = flag;
	gTrig[id].x = x * 0x10 * 0x200;
	gTrig[id].y = y * 0x10 * 0x200;
	// Set triggers hitbox
	gTrig[id].hit.front = rec.front * 0x200;
	gTrig[id].hit.top = rec.top * 0x200;
	gTrig[id].hit.back = rec.back * 0x200;
	gTrig[id].hit.bottom = rec.bottom * 0x200;
	gTrig[id].run_tsc_event = runEve;
	gTrig[id].work_on_flag_set = flagS;
	gTrig[id].work_on_flag_unset = flagU;
	if (gTrig[id].work_on_flag_set == true)
	{
		if (GetNPCFlag(gTrig[id].code_flag))
			gTrig[id].cond |= 0x80;
	}
	else if (gTrig[id].work_on_flag_unset == true)
	{
		if (!(GetNPCFlag(gTrig[id].code_flag)))
			gTrig[id].cond |= 0x80;
	}
	else
	{
		gTrig[id].cond = 0x80;
	}
}

void ResetTriggers()
{
	memset(gTrig, 0, sizeof(gTrig));
}

void DeleteTrigger(int trigNum)
{
	gTrig[trigNum].cond = 0;
}

void LoadTriggerYaml(const char* yamlPath)
{
	// Parse the YAML file
	YAML::Node data;
	try {
		data = YAML::LoadFile(yamlPath);
	}
	catch (const std::exception& e) {
		// Dont print this in release
		// printf("Error: Failed to open or parse YAML file: %s\n", e.what());
		return;
	}

	// Iterate over each trigger ID in the YAML data
	for (const auto& triggers : data["Triggers"]) {
		int id = triggers.first.as<int>();
		const YAML::Node& triggerNode = triggers.second;

		OTHER_RECT size; // the size of the trigger is a rect just like entities
		size.front = triggerNode["sizeTX"].as<int>();
		size.top = triggerNode["sizeTY"].as<int>();
		size.back = triggerNode["sizeBX"].as<int>();
		size.bottom = triggerNode["sizeBY"].as<int>();

		// Register the trigger data!
		RegisterTrigger(id, triggerNode["event"].as<int>(), triggerNode["flag"].as<int>(), triggerNode["x"].as<int>(), triggerNode["y"].as<int>(), size, triggerNode["runEvent"].as<bool>(), triggerNode["onFlagSet"].as<bool>(), triggerNode["onFlagUnset"].as<bool>());

		/*
		printf("ID: %d\n", id);
		printf("Event: %d\n", triggerNode["event"].as<int>());
		printf("x: %d\n", triggerNode["x"].as<int>());
		printf("y: %d\n", triggerNode["y"].as<int>());
		printf("sizeTX: %d\n", triggerNode["sizeTX"].as<int>());
		printf("sizeTY: %d\n", triggerNode["sizeTY"].as<int>());
		printf("sizeBX: %d\n", triggerNode["sizeBX"].as<int>());
		printf("sizeBY: %d\n", triggerNode["sizeBY"].as<int>());
		*/
	}
}

void LoadStageTriggerYaml(int no)
{
	char path[MAX_PATH];
	sprintf(path, "%s\\%s\\%s%s", gDataPath, "Stage", mTMT[no].map, ".trig.yaml");
	LoadTriggerYaml(path);
}

unsigned char JudgeHitMyCharTrig1(TRIGGER* trig)
{
	if (gMC.x + (2 * 0x200) > trig->x - trig->hit.front
	&& gMC.x - (2 * 0x200) < trig->x + trig->hit.back
	&& gMC.y + (2 * 0x200) > trig->y - trig->hit.top
	&& gMC.y - (2 * 0x200) < trig->y + trig->hit.bottom)
		return 1;

	return 0;
}

void HitMyCharTrigger(void)
{
	int i;
	int hit = 0;
	int result;
	char errormsg[256];

	if (!(gMC.cond & 0x80) || gMC.cond & 2)
		return;

	for (i = 0; i < MAX_TRIGGER; ++i)
	{
		if (!(gTrig[i].cond & 0x80))
			continue;

		hit = 0;

		hit = JudgeHitMyCharTrig1(&gTrig[i]);

		// Run event on contact
		if (!(g_GameFlags & 4) && hit != 0 && gTrig[i].run_tsc_event == true)
			StartTextScript(gTrig[i].code_event);

		// Run lua code
		if (!(g_GameFlags & 4) && hit != 0)
		{
			result = TriggerModScript(i);
			if (result == 0)
			{
				sprintf(errormsg, "Couldn't execute Act function of Trigger No. %d", i);
				MessageBoxA(ghWnd, errormsg, "ModScript Error", MB_OK);
				return;
			}
		}
	}
}

int trig_fx = 0;
int trig_fy = 0;

static int SubpixelToScreenCoord(int coord)
{
	return (coord / 0x200);
}

void DrawTriggerHitbox()
{
	static const long white_color = GetCortBoxColor(RGB(0xFF, 0xFF, 0xFF));
	GetFramePosition(&trig_fx, &trig_fy);
	RECT rect;
	int side, back, front, temp;

	for (int n = 0; n < MAX_TRIGGER; ++n)
	{
		rect.left = SubpixelToScreenCoord(gTrig[n].x) - SubpixelToScreenCoord(gTrig[n].hit.back) - SubpixelToScreenCoord(trig_fx);
		rect.top = SubpixelToScreenCoord(gTrig[n].y) - SubpixelToScreenCoord(gTrig[n].hit.top) - SubpixelToScreenCoord(trig_fy);
		rect.right = SubpixelToScreenCoord(gTrig[n].x) + SubpixelToScreenCoord(gTrig[n].hit.back) - SubpixelToScreenCoord(trig_fx);
		rect.bottom = SubpixelToScreenCoord(gTrig[n].y) + SubpixelToScreenCoord(gTrig[n].hit.bottom) - SubpixelToScreenCoord(trig_fy);

		CortBox(&rect, white_color);
	}
}

bool has_transferred_stage = false;

// Ran during the games "init" elements in ModeOpening and ModeAction
void Trigger_InitHook()
{
	ResetTriggers();
	has_transferred_stage = false;
}

// Ran at the end of the TransferStage() function
void Trigger_TransferStageHook()
{
	ResetTriggers();
	has_transferred_stage = true;
}

// Ran during the games "action" elements in ModeOpening and ModeAction
void Trigger_ActHook()
{
	if (has_transferred_stage)
	{
		LoadStageTriggerYaml(gStageNo);
		has_transferred_stage = false;
	}
}

void RegisterTriggerHooks()
{
	// all of this just to load triggers lol
	RegisterPreModeElement(Trigger_InitHook);
	RegisterOpeningInitElement(Trigger_InitHook);
	RegisterOpeningActionElement(Trigger_ActHook);
	RegisterInitElement(Trigger_InitHook);
	RegisterActionElement(Trigger_ActHook);
	RegisterTransferStageInitElement(Trigger_TransferStageHook);
	// RegisterActionElement(ActTrigger); // runs every frame to check if a flag has been set to kill the triggers functionality
	RegisterActionElement(HitMyCharTrigger); // run if the player hits a trigger
	RegisterAbovePlayerElement(DrawTriggerHitbox); // debug hitbox display
}