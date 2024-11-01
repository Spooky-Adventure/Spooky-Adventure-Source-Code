#include "koopatlas/core.h"
#include "koopatlas/starcoin.h"
#include <game.h>

dWMStarCoin_c *dWMStarCoin_c::instance = 0;

dWMStarCoin_c *dWMStarCoin_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dWMStarCoin_c));
	dWMStarCoin_c *c = new(buffer) dWMStarCoin_c;

	instance = c;
	return c;
}

dWMStarCoin_c::dWMStarCoin_c() : state(this) {
	layoutLoaded = false;
	visible = false;
	state.setState(&StateID_Hidden);
}

CREATE_STATE(dWMStarCoin_c, Hidden);
CREATE_STATE(dWMStarCoin_c, ShowWait);
CREATE_STATE(dWMStarCoin_c, Wait);
CREATE_STATE(dWMStarCoin_c, ChangeWait);
CREATE_STATE(dWMStarCoin_c, HideWait);

int dWMStarCoin_c::onCreate() {

	if (!layoutLoaded) {
		bool gotFile = layout.loadArc("worldCollection.arc", false);
		if (!gotFile)
			return false;

		bool output = layout.build("worldCollection.brlyt");

		if (IsWideScreen()) {
			layout.layout.rootPane->scale.x = 0.735f;
		} else {
			layout.clippingEnabled = true;
			layout.clipX = 0;
			layout.clipY = 52;
			layout.clipWidth = 640;
			layout.clipHeight = 352;
			layout.layout.rootPane->scale.x = 0.731f;
			layout.layout.rootPane->scale.y = 0.7711f;
		}

		static const char *brlanNames[] = {
			"worldCollection_inWindow.brlan",
			"worldCollection_outWindow.brlan",
			"worldCollection_loopCheck.brlan",
			"worldCollection_screenBefore.brlan",
			"worldCollection_screenNext.brlan",
			"worldCollection_secretShow.brlan",
			"worldCollection_secretHide.brlan",
		};
		static const char *groupNames[] = {
			"A00_Window", "A00_Window", "D00_Check", "B00_Screen", "B00_Screen", "C00_Secret", "C00_Secret",
		};

		layout.loadAnimations(brlanNames, 7);
		layout.loadGroups(groupNames, (int[7]){0, 1, 2, 3, 4, 5, 6}, 7);
		layout.disableAllAnimations();

		layout.drawOrder = 1;

		static const char *tbNames[] = {
			"T_titleCoin_00", "T_back_00",
			"T_messageS_00", "T_message_00",
			"T_world_00", "T_world_01",
			"T_coinCurrent_00", "T_coinTotal_00"
		};
		layout.getTextBoxes(tbNames, &T_titleCoin_00, 8);

		// Get a bunch of different panes
		for (int i = 0; i < 10; i++) {
		// NULL PANES
			char coinBarPosName[15];
			sprintf(coinBarPosName, "N_coinBarPos_0%d", i);
			N_coinBarPos_0[i] = layout.findPaneByName(coinBarPosName);

			char filterName[15];
			sprintf(filterName, "W_fileter_0%d", i);
			W_fileter_0[i] = layout.findPaneByName(filterName);

		// TEXTBOXES
			char worldNumName[13];
			sprintf(worldNumName, "T_worldNum_0%d", i);
			T_worldNum_0[i] = layout.findTextBoxByName(worldNumName);

			char courseNumName[13];
			sprintf(courseNumName, "T_corseNum_0%d", i);
			T_worldNum_0[i] = layout.findTextBoxByName(courseNumName);

			char picFontName[16];
			sprintf(picFontName, "T_pictureFont_0%d", i);
			T_pictureFont_0[i] = layout.findTextBoxByName(picFontName);

		// PICTURES
			char topFilterName[15];
			sprintf(topFilterName, "P_topFileter_0%d", i);
			P_topFileter_0[i] = layout.findPictureByName(topFilterName);

			// All these can be done efficiently by removing the "_0", but I'd rather keep it
			char coinFilterName1[13];
			sprintf(coinFilterName1, "P_coinFileter_1_0%d", i);
			P_coinFileter_1_0[i] = layout.findPictureByName(coinFilterName1);

			char coinFilterName2[13];
			sprintf(coinFilterName2, "P_coinFileter_2_0%d", i);
			P_coinFileter_2_0[i] = layout.findPictureByName(coinFilterName2);

			char coinFilterName3[13];
			sprintf(coinFilterName3, "P_coinFileter_3_0%d", i);
			P_coinFileter_3_0[i] = layout.findPictureByName(coinFilterName3);

			char coinName1[11];
			sprintf(coinName1, "P_coin_1_0%d", i);
			P_coin_1_0[i] = layout.findPictureByName(coinName1);

			char coinName2[11];
			sprintf(coinName2, "P_coin_2_0%d", i);
			P_coin_2_0[i] = layout.findPictureByName(coinName2);

			char coinName3[11];
			sprintf(coinName3, "P_coin_3_0%d", i);
			P_coin_3_0[i] = layout.findPictureByName(coinName3);
		}

		/*static const char *picNames[] = {
			"DPadLeft", "DPadRight",
		};
		layout.getPictures(picNames, &DPadLeft, 2);

		DPadLeft->SetVisible(false);
		DPadRight->SetVisible(false);*/

		layoutLoaded = true;
	}

	return true;
}


int dWMStarCoin_c::onDelete() {
	return layout.free();
}


void dWMStarCoin_c::show() {
	if (state.getCurrentState() == &StateID_Hidden)
		state.setState(&StateID_ShowWait);
}


int dWMStarCoin_c::onExecute() {
	state.execute();

	if (visible) {
		layout.execAnimations();
		layout.update();
	}

	return true;
}

int dWMStarCoin_c::onDraw() {
	if (visible)
		layout.scheduleForDrawing();

	return true;
}


void dWMStarCoin_c::showLeftArrow() {
	if (!isLeftArrowVisible) {
		isLeftArrowVisible = true;
		layout.enableNonLoopAnim(SHOW_LEFT_ARROW);
		//DPadLeft->SetVisible(true);
	}
}

void dWMStarCoin_c::showRightArrow() {
	if (!isRightArrowVisible) {
		isRightArrowVisible = true;
		layout.enableNonLoopAnim(SHOW_RIGHT_ARROW);
		//DPadRight->SetVisible(true);
	}
}

void dWMStarCoin_c::hideLeftArrow() {
	if (isLeftArrowVisible) {
		isLeftArrowVisible = false;
		layout.enableNonLoopAnim(HIDE_LEFT_ARROW);
		//DPadLeft->SetVisible(false);
	}
}

void dWMStarCoin_c::hideRightArrow() {
	if (isRightArrowVisible) {
		isRightArrowVisible = false;
		layout.enableNonLoopAnim(HIDE_RIGHT_ARROW);
		//DPadRight->SetVisible(false);
	}
}

void dWMStarCoin_c::setLeftArrowVisible(bool value) {
	if (value)
		showLeftArrow();
	else
		hideLeftArrow();
}

void dWMStarCoin_c::setRightArrowVisible(bool value) {
	if (value)
		showRightArrow();
	else
		hideRightArrow();
}


bool dWMStarCoin_c::canScrollLeft() const {
	return (currentSectionIndex > 0);
}
bool dWMStarCoin_c::canScrollRight() const {
	return (currentSectionIndex < (availableSectionCount - 1));
}

void dWMStarCoin_c::loadInfo() {
	WriteBMGToTextBox(T_back_00, GetBMG(), 2, 58, 0);
	WriteBMGToTextBox(T_titleCoin_00, GetBMG(), 2, 61, 0);

	currentSection = -1;
	currentSectionIndex = -1;
	availableSectionCount = 0;

	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	int wantedSection = save->newerWorldID;

	// Figure out which sections should be available
	for (int i = 0; i < dLevelInfo_c::s_info.sectionCount(); i++) {
		dLevelInfo_c::section_s *section = dLevelInfo_c::s_info.getSectionByIndex(i);

		bool haveLevels = false;
		for (int j = 0; j < section->levelCount; j++) {
			dLevelInfo_c::entry_s *l = &section->levels[j];
			if (l->flags & 2) {
				if (save->GetLevelCondition(l->worldSlot, l->levelSlot) & COND_UNLOCKED) {
					haveLevels = true;
					break;
				}
			}
		}

		if (haveLevels) {
			if (i == wantedSection) {
				currentSection = wantedSection;
				currentSectionIndex = availableSectionCount;
			}
			sectionIndices[availableSectionCount++] = i;
		}
	}

	// If we didn't find the wanted one, use the first one available
	if (currentSectionIndex == -1) {
		currentSectionIndex = 0;
		currentSection = sectionIndices[0];
	}
}

void dWMStarCoin_c::loadSectionInfo() {
	dLevelInfo_c::entry_s *visibleLevels[10];

	// Hide entries
	for (int i = 0; i < 10; i++) {
		N_coinBarPos_0[i]->SetVisible(false);
	}

	// Get everything we'll need
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	dLevelInfo_c *levelInfo = &dLevelInfo_c::s_info;

	// Get the world name
	dLevelInfo_c::entry_s *names;
	WriteAsciiToTextBox(T_world_00, levelInfo->getNameForLevel(names));

	//dLevelInfo_c::entry_s *names[COLUMN_COUNT];
	//for (int i = 0; i < COLUMN_COUNT; i++)
	//	names[i] = levelInfo->searchByDisplayNum(currentSection, 100+i);

	//bool useSubworlds = (COLUMN_COUNT > 1) && names[1];

	//int currentPosition[COLUMN_COUNT];
	//int currentColumn = 0; // Only incremented in single-subworld mode

	//for (int i = 0; i < COLUMN_COUNT; i++)
	//	currentPosition[i] = 0;

	
	dLevelInfo_c::section_s *section = levelInfo->getSectionByIndex(currentSection);

	int collectedCoins = 0, totalCoins = 0;
	// collectedCoins is calculated later

	int totalLevels = 0;

	for (int i = 0; i < section->levelCount; i++) {
		dLevelInfo_c::entry_s *level = &section->levels[i];

		// Only pay attention to levels with Star Coins
		if (!(level->flags & 2))
			continue;

		totalCoins += 3;

		// Is this level unlocked?
		/*u32 conds = save->GetLevelCondition(level->worldSlot, level->levelSlot);

		if (!(conds & COND_UNLOCKED))
			continue;
		*/

		totalLevels++;
	}

	

	for (int i = 0; i < 10; i++) {
		dLevelInfo_c::entry_s *level = visibleLevels[totalLevels];

		if (!level)
			continue;

		u32 conds = save->GetLevelCondition(level->worldSlot, level->levelSlot);

		for (int j = 0; j < totalLevels; j++) {
			if (j < 10) return;
			N_coinBarPos_0[j]->SetVisible(true);
		}
	}
	
	// load all level info
	/*for (int col = 0; col < COLUMN_COUNT; col++) {
		for (int row = 0; row < ROW_COUNT; row++) {
			dLevelInfo_c::entry_s *level = visibleLevels[col][row];
			if (!level)
				continue;

			u32 conds = save->GetLevelCondition(level->worldSlot, level->levelSlot);

			if (!(row & 1)) {
				int shineID = row / 2;
				if (shineID < SHINE_COUNT)
					Shine[col][shineID]->SetVisible(true);
			}

			for (int coin = 0; coin < 3; coin++) {
				CoinOutline[col][row][coin]->SetVisible(true);

				if (conds & (COND_COIN1 << coin)) {
					Coin[col][row][coin]->SetVisible(true);
					earnedCoins++;
				}
			}

			LevelName[col][row]->SetVisible(true);
			WriteAsciiToTextBox(LevelName[col][row], linfo->getNameForLevel(level));
		}
	}*/

	// Write Star Coin amounts
	int length = 2;
	WriteNumberToTextBox(&collectedCoins, &length, T_coinCurrent_00, false);
	WriteNumberToTextBox(&totalCoins, &length, T_coinTotal_00, false);
}


void dWMStarCoin_c::beginState_Hidden() { }
void dWMStarCoin_c::executeState_Hidden() { }
void dWMStarCoin_c::endState_Hidden() { }

static const int secretCode[] = {
	WPAD_UP,WPAD_UP,WPAD_DOWN,WPAD_DOWN,
	WPAD_LEFT,WPAD_RIGHT,WPAD_LEFT,WPAD_RIGHT,
	WPAD_ONE,WPAD_TWO,0
};
static const int secretCodeButtons = WPAD_UP|WPAD_DOWN|WPAD_LEFT|WPAD_RIGHT|WPAD_ONE|WPAD_TWO;
static int secretCodeIndex = 0;
static int minusCount = 0;
extern bool enableHardMode;
extern bool enableDebugMode;
extern u8 isReplayEnabled;

// Secret Message strings, the ID is used to load a specific message from here
static const wchar_t *secretMessages[] = {
	/*0*/ L"Hard Mode has been enabled!\nIn Hard Mode, Mario will die any\n time hetakes damage, and the\ntimer will be more strict.",
	/*1*/ L"Hard Mode disabled.\nLevels will now function normally.",
	/*2*/ L"The Replay Recorder is active!\nYou'll need to be on a real\nWii console to record them.\nBe sure to save before a level,\njust in case it doesn't work.",
	/*3*/ L"Replay Recorder disabled.",
	/*4*/ L"You've enabled the secret\nCollision Debug Mode!\nModels will now draw lines to\nshow their hitboxes. It doesn't\nwork 100% perfectly, so some\nhitboxes might not show up.",
	/*5*/ L"The Collision Debug Mode\nhas been disabled.",
	/*6*/ L"This is a secret message, so\nyou won't see this in-game. If\nyou do, report it as a bug."
};


// Menu is first opened
void dWMStarCoin_c::beginState_ShowWait() {
	visible = true;
	loadInfo();
	loadSectionInfo();
	MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_IN, 1);
	layout.enableLoopAnim(LOOP_CHECK);
	layout.enableNonLoopAnim(IN_WINDOW);

	// Reset our secret code stuff
	secretCodeIndex = 0;
	minusCount = 0;
}
void dWMStarCoin_c::executeState_ShowWait() {
	if (!layout.isAnimOn(IN_WINDOW))
		state.setState(&StateID_Wait);
}
void dWMStarCoin_c::endState_ShowWait() { }



void dWMStarCoin_c::beginState_Wait() { }
void dWMStarCoin_c::executeState_Wait() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	// Hard Mode (A and Plus)
	if ((GetActiveRemocon()->heldButtons == 0x810) && (nowPressed & 0x810)) {

		if (!enableHardMode) {
			enableHardMode = true;
			T_messageS_00->SetString(secretMessages[0]);
			T_message_00->SetString(secretMessages[0]);
			OSReport("Hard Mode enabled!\n");
			MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_CS_COURSE_IN_HARD, 1);
		} else {
			enableHardMode = false;
			T_messageS_00->SetString(secretMessages[1]);
			T_message_00->SetString(secretMessages[1]);
			OSReport("Hard Mode disabled!\n");
		}
		return;
	}

	// Replay Recorder
	if (nowPressed & secretCodeButtons) {
		int nextKey = secretCode[secretCodeIndex];
		if (nowPressed & nextKey) {
			secretCodeIndex++;
			if (secretCode[secretCodeIndex] == 0) {
				secretCodeIndex = 0;
				MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_THANK_YOU, 1);
				
				if (isReplayEnabled != 100) {
					isReplayEnabled = 100;
					T_messageS_00->SetString(secretMessages[2]);
					T_message_00->SetString(secretMessages[2]);
					OSReport("Replay Recording enabled!\n");
				} else {
					isReplayEnabled = 0;
					T_messageS_00->SetString(secretMessages[3]);
					T_message_00->SetString(secretMessages[3]);
					OSReport("Replay Recording disabled!\n");
					
				}
			}
			return;
		} else {
			secretCodeIndex = 0;
		}
	}

	// Collision Debugger
	if (nowPressed & WPAD_MINUS) {
		minusCount++;
		if (minusCount >= 16) {
			minusCount = 0;

			enableDebugMode = !enableDebugMode;

			if (enableDebugMode) {
				T_message_00->SetString(secretMessages[4]);
				T_messageS_00->SetString(secretMessages[4]);
				MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_GET_PRIZE, 1);
			} else {
				T_message_00->SetString(secretMessages[5]);
				T_messageS_00->SetString(secretMessages[5]);
			}
		}
	}
	
	// Close menu
	else if (nowPressed & WPAD_ONE) {
		MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_OUT_AUTO, 1);
		state.setState(&StateID_HideWait);
	}
	
	// Change to previous page
	else if ((nowPressed & WPAD_LEFT) && canScrollLeft()) {
		currentSection = sectionIndices[--currentSectionIndex];
		layout.enableNonLoopAnim(SCREEN_BEFORE);
		state.setState(&StateID_ChangeWait);
		direction = 0;
	}
	
	// Change to next page
	else if ((nowPressed & WPAD_RIGHT) && canScrollRight()) {
		currentSection = sectionIndices[++currentSectionIndex];
		layout.enableNonLoopAnim(SCREEN_NEXT);
		state.setState(&StateID_ChangeWait);
	}
}
void dWMStarCoin_c::endState_Wait() { }



// Change pages
void dWMStarCoin_c::beginState_ChangeWait() {
	//setLeftArrowVisible(canScrollLeft());
	//setRightArrowVisible(canScrollRight());
}
void dWMStarCoin_c::executeState_ChangeWait() {
	if ((!layout.isAnimOn(SCREEN_BEFORE)) || (!layout.isAnimOn(SCREEN_NEXT))) {
		state.setState(&StateID_Wait);
	}
}
void dWMStarCoin_c::endState_ChangeWait() { }



// Close the menu
void dWMStarCoin_c::beginState_HideWait() {
	MapSoundPlayer(SoundRelatedClass, SE_SYS_BACK, 1);
	layout.enableNonLoopAnim(OUT_WINDOW);
}
void dWMStarCoin_c::executeState_HideWait() {
	if (!layout.isAnimOn(OUT_WINDOW))
		state.setState(&StateID_Hidden);
}
void dWMStarCoin_c::endState_HideWait() {
	visible = false;
}

