#include <game.h>
#include <newer.h>
#include "levelinfo.h"

extern char CurrentLevel;
extern char CurrentWorld;

int dGameDisplay_c::newOnExecute() {
	int orig_val = this->onExecute_orig();
	
	//layout.findTextBoxByName("T_score_00")->SetString(L"nice score bro");
	
	dLevelInfo_c::entry_s *level = dLevelInfo_c::s_info.searchBySlot(CurrentWorld, CurrentLevel);
	if (level) {
		wchar_t convLevelName[160];
		const char *srcLevelName = dLevelInfo_c::s_info.getNameForLevel(level);
		int i = 0;
		while (i < 159 && srcLevelName[i]) {
			convLevelName[i] = srcLevelName[i];
			i++;
		}
		convLevelName[i] = 0;
		//layout.findTextBoxByName("T_score_00")->SetString(convLevelName);
		
		//LevelNameShadow->SetString(convLevelName);
		//LevelName->SetString(convLevelName);

		//wchar_t levelNumber[32];
		//wcscpy(levelNumber, L"World ");
		//getNewerLevelNumberString(level->displayWorld, level->displayLevel, &levelNumber[6]);
		
		//something something do an "if it's so-and-so level/world, make the shit happen :uhhuh:"
		

	} else {
		//LevelNameShadow->SetString(L"Not found in LevelInfo!");
		//LevelName->SetString(L"Not found in LevelInfo!");
	}
	
	return orig_val;
}
