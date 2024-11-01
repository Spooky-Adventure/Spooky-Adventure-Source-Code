#include <game.h>
#include <newer.h>
#include "levelinfo.h"

int lastLevelIDs[] = {
	-1, /*no world*/
	27,	// W1
	27,
	27,
	27,
	27,	// W5
	27,
	27,
	25,
	10,
	24,	// W10 (A)
	24,
	21,
	24,
	3	// W14 (E)
};


void WriteAsciiToTextBox(nw4r::lyt::TextBox *tb, const char *source) {
	int i = 0;
	wchar_t buffer[1024];
	while (i < 1023 && source[i]) {
		buffer[i] = source[i];
		i++;
	}
	buffer[i] = 0;

	tb->SetString(buffer);
}


void getNewerLevelNumberString(int world, int level, wchar_t *dest) {
	// Funnily enough, since this is empty, LYTs that use this just show "World   "
}


// These next two functions serve the purpose that getNewerLevelNumberString did, but since
// retail layouts have the Level ID as separate panes, this allows us to apply separate data to each pane
void getWorldNumber(int world, wchar_t *dest) {
	dest[0] = (world >= 10) ? (world-10+'A') : (world+'0');
	dest[1] = 0;
}

void getLevelNumber(int level, wchar_t *dest) {
	static const wchar_t *numberKinds[] = {
		// 0-19 are handled by code
		// To insert a character: \x0B\x0WYY\xZZZZ
		// W is the font ID, YY is the character code, ZZZZ is ignored
		// Font IDs are as follows: 0 - MarioFont, 1 - PictureFont, 2 - MessageFont, 3 - NumberFont
		L"A", 						// 20, Alternate
		L"\x0B\x0148\xBEEF", 		// 21, Tower
		L"\x0B\x0148\xBEEF" L"2", 	// 22, Tower 2
		L"\x0B\x012E\xBEEF", 		// 23, Castle
		L"\x0B\x012F\xBEEF", 		// 24, Fortress
		L"\x0B\x013D\xBEEF", 		// 25, Final Castle
		L"\x0B\x014D\xBEEF", 		// 26, Train
		L"\x0B\x0132\xBEEF", 		// 27, Airship
		L"Palace", 					// 28, Switch Palace
		L"\x0B\x0147\xBEEF", 		// 29, Yoshi's House
		L"\x0B\x014E\xBEEF" L"1", 	// 30, Key 1
		L"\x0B\x014E\xBEEF" L"2", 	// 31, Key 2
		L"\x0B\x014E\xBEEF" L"3", 	// 32, Key 3
		L"\x0B\x014E\xBEEF" L"4", 	// 33, Key 4
		L"\x0B\x014E\xBEEF" L"5", 	// 34, Key 5
		L"\x0B\x014E\xBEEF" L"6", 	// 35, Key 6
		L"\x0B\x0138\xBEEF", 		// 36, Music House
		L"\x0B\x0133\xBEEF", 		// 37, Shop
		L"\x0B\x0139\xBEEF", 		// 38, Challenge House
		L"\x0B\x0151\xBEEF", 		// 39, Red Switch Palace
		L"\x0B\x0152\xBEEF", 		// 40, Blue Switch Palace
		L"\x0B\x0153\xBEEF", 		// 41, Yellow Switch Palace
		L"\x0B\x0154\xBEEF", 		// 42, Green Switch Palace
		L"\x0B\x0146\xBEEF", 		// 43, Message Box
		L"\x0B\x0136\xBEEF",		// 44, Map Junction/Stopping Point
		L"\x0B\x0134\xBEEF",		// 45, Start Node Pointing Right
		L"\x0B\x0135\xBEEF",		// 46, Start Node Pointing Up
		L"\x0B\x0155\xBEEF",		// 47, Start Node Pointing Left
		L"\x0B\x0156\xBEEF",		// 48, Start Node Pointing Down
		L"\x0B\x0157\xBEEF",		// 49, Signboard
	};
	
	// This is for whether or not we'll use the pane specifically for icons
	showCoursePic = false;
	
	// If you're wondering WHY there's a special pane just for icons, it's
	// because the pane is at a smaller scale than the numbers, so
	// it won't look as bad or too large compared to the rest of the level ID
	
	// For a reference of how bad it looks on the regular pane, just look at Newer's icons

	if (level >= 20) {
		wcscpy(&dest[0], numberKinds[level-20]);
		showCoursePic = true;
	} else if (level >= 10) {
		showCoursePic = false;
		dest[0] = '1';
		dest[1] = ('0' - 10) + level;
		dest[2] = 0;
		
	} else {
		showCoursePic = false;
		dest[0] = '0' + level;
		dest[1] = 0;
	}
}

int getUnspentStarCoinCount() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	int coinsSpent = save->spentStarCoins;
	return getStarCoinCount() - coinsSpent;
}

int getStarCoinCount() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	int coinsEarned = 0;

	for (int w = 0; w < 10; w++) {
		for (int l = 0; l < 42; l++) {
			u32 conds = save->GetLevelCondition(w, l);

			if (conds & COND_COIN1) { coinsEarned++; }
			if (conds & COND_COIN2) { coinsEarned++; }
			if (conds & COND_COIN3) { coinsEarned++; }
		}
	}

	return coinsEarned;
}

struct GEIFS {
	int starCoins, exits;
};
extern "C" GEIFS *GrabExitInfoForFileSelect(GEIFS *out, SaveBlock *save) {
	out->starCoins = 0;
	out->exits = 0;

	for (int i = 0; i < dLevelInfo_c::s_info.sectionCount(); i++) {
		dLevelInfo_c::section_s *section = dLevelInfo_c::s_info.getSectionByIndex(i);

		for (int j = 0; j < section->levelCount; j++) {
			dLevelInfo_c::entry_s *l = &section->levels[j];
			if (l->flags & 2) {
				//OSReport("Checking %d-%d...\n", l->worldSlot+1, l->levelSlot+1);
				u32 cond = save->GetLevelCondition(l->worldSlot, l->levelSlot);
				if ((l->flags & 0x10) && (cond & COND_NORMAL))
					out->exits++;
				if ((l->flags & 0x20) && (cond & COND_SECRET))
					out->exits++;
				if (cond & COND_COIN1)
					out->starCoins++;
				if (cond & COND_COIN2)
					out->starCoins++;
				if (cond & COND_COIN3)
					out->starCoins++;
			}
		}
	}

	OSReport("Done, got %d coins and %d exits\n", out->starCoins, out->exits);

	return out;
}

// Replaces the ASM that wrote the Star Coin/Exit counters, and adds some new stuff
extern "C" void WriteDateFileInfo(m2d::EmbedLayout_c *layout, int totalStarCoins, int totalExits) {
	// Get panes
	nw4r::lyt::TextBox *StarCoinCount, *ExitCount;
	StarCoinCount = layout->findTextBoxByName("StarCoinCount");
	ExitCount = layout->findTextBoxByName("ExitCount");

	int stringLength = 3;

	// Write collection values
	WriteNumberToTextBox(&totalStarCoins, &stringLength, StarCoinCount, false);
	WriteNumberToTextBox(&totalExits, &stringLength, ExitCount, false);


	// TODO: Add 100% completion recolorings
	if (totalStarCoins >= 7) {
		StarCoinCount->colour1 = (GXColor){247,91,0,255};
		StarCoinCount->colour2 = (GXColor){255,219,112,255};
	} else {
		StarCoinCount->colour1 = (GXColor){255,255,255,255};
		StarCoinCount->colour2 = (GXColor){255,255,255,255};
	}

	if (totalExits >= 4) {

	} else {

	}
}
