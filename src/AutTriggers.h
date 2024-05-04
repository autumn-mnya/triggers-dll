#pragma once

#include <Windows.h>

#include "cave_story.h"

#define MAX_TRIGGER 0x800

struct TRIGGER
{
	int x;
	int y;
	int code_event;
	int code_flag;
	OTHER_RECT hit;
	unsigned char cond;
	bool run_tsc_event;
	bool work_on_flag_set;
	bool work_on_flag_unset;
};

extern TRIGGER gTrig[MAX_TRIGGER];

void RegisterTrigger(int id, int eve, int flag, int x, int y, OTHER_RECT rec, bool runEve, bool flagS, bool flagU);
void DeleteTrigger(int trigNum);
void RegisterTriggerHooks();