#include "koopatlas/shop.h"

CREATE_STATE(dWMShop_c, Hidden);
CREATE_STATE(dWMShop_c, ShowWait);
CREATE_STATE(dWMShop_c, ButtonActivateWait);
CREATE_STATE(dWMShop_c, CoinCountdown);
CREATE_STATE(dWMShop_c, Wait);
CREATE_STATE(dWMShop_c, HideWait);

void dWMShop_c::ShopModel_c::setupItem(float x, float y, ItemTypes type) {
	static const char* Produce[ITEM_TYPE_COUNT][4] = { 
		{ "I_kinoko", 		"g3d/I_kinoko.brres", 			"I_kinoko", 			"wait2" },
		{ "I_fireflower", 	"g3d/I_fireflower.brres", 		"I_fireflower", 		"wait2" },
		{ "I_propeller", 	"g3d/I_propeller.brres", 		"I_propeller_model", 	"wait2" },
		{ "I_iceflower", 	"g3d/I_iceflower.brres", 		"I_iceflower", 			"wait2" },
		{ "I_penguin", 		"g3d/I_penguin.brres", 			"I_penguin", 			"wait2" },
		{ "I_kinoko_bundle","g3d/I_mini_kinoko.brres", 		"I_mini_kinoko", 		"wait2" },
		{ "I_star", 		"g3d/I_star.brres", 			"I_star", 				"wait2" },
		{ "I_hammer", 		"g3d/I_fireflower.brres",		"I_fireflower",			"wait2" },
		{ "I_kinoko_bundle","g3d/I_life_kinoko.brres", 		"I_life_kinoko", 		"wait2" },
	};

	this->x = x;
	this->y = y;
	scaleFactor = 2.3f;

	int id = (int)type;
	isLakitu = false;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	res.data = getResource(Produce[id][0], Produce[id][1]);
	nw4r::g3d::ResMdl mdlRes = res.GetResMdl(Produce[id][2]);
	model.setup(mdlRes, &allocator, 0x224, 1, 0);

	SetupTextures_Item(&model, 1);

	nw4r::g3d::ResAnmChr anmChr = res.GetResAnmChr(Produce[id][3]);
	animation.setup(mdlRes, anmChr, &allocator, 0);
	playAnim(Produce[id][3], 1.0f, 0);

	allocator.unlink();
}

void dWMShop_c::ShopModel_c::setupLakitu(int id) {
	static const char* models[10] = { 
		"g3d/yoshi.brres", "g3d/desert.brres", "g3d/mountain.brres", "g3d/sakura.brres", "g3d/santa.brres", 
		"g3d/ghost.brres", "g3d/space.brres", "g3d/koopa.brres", "g3d/sewer.brres", "g3d/goldwood.brres" 
	};

	scaleFactor = 1.0f;
	scaleEase = 0.0f;
	this->isLakitu = true;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	res.data = getResource("lakitu", models[id]);
	nw4r::g3d::ResMdl mdlRes = res.GetResMdl("lakitu");
	model.setup(mdlRes, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&model, 1);

	nw4r::g3d::ResAnmChr anmChr = res.GetResAnmChr("idle");
	animation.setup(mdlRes, anmChr, &allocator, 0);

	playAnim("idle", 1.0f, 0);
	playingNotEnough = false;

	allocator.unlink();
}

void dWMShop_c::ShopModel_c::playAnim(const char *name, float rate, char loop) {
	nw4r::g3d::ResAnmChr anmChr = res.GetResAnmChr(name);
	animation.bind(&model, anmChr, loop);
	model.bindAnim(&animation, 0.0f);
	animation.setUpdateRate(rate);
}

void dWMShop_c::ShopModel_c::execute() {
	model._vf1C();

	if(this->animation.isAnimationDone()) {
		OSReport("Animaiton Complete");
		if (this->isLakitu) {
			OSReport("Setting animation to idle");
			playAnim("idle", 1.0f, 0);
			this->animation.setCurrentFrame(0.0);
			if (playingNotEnough) {
				OSReport("Detected Not Enough animation as being over\n");
				playingNotEnough = false;
			}
		}
	}	
}

void dWMShop_c::ShopModel_c::draw() {
	mMtx mtx;
	mtx.translation(x, y, 1000.0f);
	model.setDrawMatrix(mtx);

	float s = scaleFactor * scaleEase;

	Vec scale = {s, s, s};
	model.setScale(&scale);

	model.calcWorld(false);
	model.scheduleForDrawing();
}



dWMShop_c *dWMShop_c::instance = 0;

dWMShop_c *dWMShop_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dWMShop_c));
	dWMShop_c *c = new(buffer) dWMShop_c;

	instance = c;
	return c;
}

dWMShop_c::dWMShop_c() : state(this, &StateID_Hidden) {
	layoutLoaded = false;
	visible = false;
}

int dWMShop_c::onCreate() {
	if (!layoutLoaded) {
		bool gotFile = layout.loadArc("shop.arc", false);
		if (!gotFile)
			return false;

		selected = 0;
		lastTopRowChoice = 0;

		layout.build("shop.brlyt");

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
			"shop_inWindow.brlan",
			"shop_outWindow.brlan",
			"shop_ActivateButton.brlan",
			"shop_DeactivateButton.brlan",
			"shop_CountCoin.brlan",
			"shop_BuyItem.brlan"
		};
		static const char *groupNames[] = {
			"A00_Window", "A00_Window",
			"B00_BtnSmall", "B01_BtnSmall", "B02_BtnSmall", "B03_BtnSmall", "C00_BtnLarge", "C01_BtnLarge",
			"B00_BtnSmall", "B01_BtnSmall", "B02_BtnSmall", "B03_BtnSmall", "C00_BtnLarge", "C01_BtnLarge",
			"D00_CoinCount",
			"B00_BtnSmall", "B01_BtnSmall", "B02_BtnSmall", "B03_BtnSmall", "C00_BtnLarge", "C01_BtnLarge",
		};
		static const int brlanIDs[] = {
			0, 1,
			2, 2, 2, 2, 2, 2,
			3, 3, 3, 3, 3, 3,
			4,
			5, 5, 5, 5, 5, 5,
		};

		layout.loadAnimations(brlanNames, 6);
		layout.loadGroups(groupNames, brlanIDs, 21);
		layout.disableAllAnimations();

		layout.drawOrder = 1;

		static const char *tbNames[] = {
			"T_title_00", "T_titleS_00",
			"T_x_00", "T_coinTitle_00",
			"T_coinUnspent_00", "T_coinUnspentS_00",
			"T_back_00", "T_buy_00",
		};
		layout.getTextBoxes(tbNames, &T_title_00, 8);
		
		static const char *picNames[] = {
			"P_btnSmBG_00", "P_btnSmBG_01",
			"P_btnSmBG_02", "P_btnSmBG_03",
			"P_btnLgBG_00", "P_btnLgBG_01",
		};
		layout.getPictures(picNames, &P_btnSmBG_00, 6);

		// Button names
		static const char *btnNames[] = {
			"N_buttonSmall_00", "N_buttonSmall_01",
			"N_buttonSmall_02", "N_buttonSmall_03",
			"N_buttonLarge_00", "N_buttonLarge_01",
		};
		layout.getPanes(btnNames, &N_buttonSmall_00, 6);

		layoutLoaded = true;
	}

	return true;
}


int dWMShop_c::onDelete() {
	deleteModels();
	return layout.free();
}


int dWMShop_c::onExecute() {
	state.execute();

	if (visible) {
		lakituModel->execute();

		for (int i = 0; i < 12; i++)
			itemModels[i].execute();
	}

	layout.execAnimations();
	layout.update();

	return true;
}

int dWMShop_c::onDraw() {
	if (visible)
		layout.scheduleForDrawing();

	return true;
}

void dWMShop_c::specialDraw1() {
	if (visible) {
		lakituModel->scaleEase = scaleEase;
		lakituModel->draw();
		for (int i = 0; i < 12; i++) {
			itemModels[i].scaleEase = scaleEase;
			itemModels[i].draw();
		}
	}


//		if (wasOff) { effect.spawn("Wm_ob_greencoinkira", 0, &pos, &rot, &scale); wasOff = false; }

//		if(this->ska.isAnimationDone())
//			this->ska.setCurrentFrame(0.0);
}


void dWMShop_c::show(int shopNumber) {
	shopKind = shopNumber;
	state.setState(&StateID_ShowWait);
}


// Hidden
void dWMShop_c::beginState_Hidden() { }
void dWMShop_c::executeState_Hidden() { }
void dWMShop_c::endState_Hidden() { }

// ShowWait
void dWMShop_c::beginState_ShowWait() {
	MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_IN, 1);

	layout.disableAllAnimations();
	layout.enableNonLoopAnim(SHOW_ALL);
	visible = true;
	scaleEase = 0.0;

	loadInfo();
	loadModels();
}
void dWMShop_c::executeState_ShowWait() {
	if (!layout.isAnimOn(SHOW_ALL)) {
		selected = 0;
		layout.enableNonLoopAnim(ACTIVATE_BUTTON);
		state.setState(&StateID_ButtonActivateWait);
	}
}
void dWMShop_c::endState_ShowWait() {
	MapSoundPlayer(SoundRelatedClass, SE_OBJ_CLOUD_BLOCK_TO_JUGEM, 1);
	timer = 1;
}

// ButtonActivateWait
void dWMShop_c::beginState_ButtonActivateWait() { }
void dWMShop_c::executeState_ButtonActivateWait() {
	if (!layout.isAnyAnimOn())
		state.setState(&StateID_Wait);
}
void dWMShop_c::endState_ButtonActivateWait() { }

// Wait
void dWMShop_c::beginState_Wait() {
	showSelectCursor();
}
void dWMShop_c::executeState_Wait() {
	if (timer < 90) {
		scaleEase = -((cos(timer * 3.14 /20)-0.9)/timer*10)+1;
		timer++;
		return;
	}

	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	int newSelection = -1;

	if (nowPressed & WPAD_ONE) {
		// Hide the thing
		state.setState(&StateID_HideWait);

	} else if (nowPressed & WPAD_UP) {
		// Move up
		if (selected == 4)
			newSelection = lastTopRowChoice;
		else if (selected == 5)
			newSelection = 4;

	} else if (nowPressed & WPAD_DOWN) {
		// Move down
		if (selected <= 3)
			newSelection = 4;
		else if (selected == 4)
			newSelection = 5;

	} else if (nowPressed & WPAD_LEFT) {
		// Just use the previous choice
		if (selected > 0)
			newSelection = selected - 1;

	} else if (nowPressed & WPAD_RIGHT) {
		// Just use the next choice
		if (selected < 5)
			newSelection = selected + 1;

	} else if (nowPressed & WPAD_TWO) {
		if (!lakituModel->playingNotEnough)
			buyItem(selected);
		else
			OSReport("Not Enough is still playing!\n");
	}

	if (newSelection > -1) {
		MapSoundPlayer(SoundRelatedClass, SE_SYS_CURSOR, 1);

		layout.enableNonLoopAnim(DEACTIVATE_BUTTON+selected);
		layout.enableNonLoopAnim(ACTIVATE_BUTTON+newSelection);

		selected = newSelection;
		if (newSelection <= 3)
			lastTopRowChoice = newSelection;

		showSelectCursor();
	}
}
void dWMShop_c::endState_Wait() { }

// HideWait
void dWMShop_c::beginState_HideWait() {
	MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_OUT_AUTO, 1);
	layout.enableNonLoopAnim(HIDE_ALL);
	layout.enableNonLoopAnim(DEACTIVATE_BUTTON+selected);

	timer = 26;
	MapSoundPlayer(SoundRelatedClass, SE_OBJ_CS_KINOHOUSE_DISAPP, 1);

	HideSelectCursor(SelectCursorPointer, 0);
}
void dWMShop_c::executeState_HideWait() {
	if (timer > 0) {
		timer--;
		scaleEase = -((cos(timer * 3.14 /13.5)-0.9)/timer*10)+1;
		if (scaleEase < 0.0f)
			scaleEase = 0.0f;
	}

	if (!layout.isAnimOn(HIDE_ALL))
		state.setState(&StateID_Hidden);
}
void dWMShop_c::endState_HideWait() {
	deleteModels();
	visible = false;
}


// Powerup Listing:
// 0 = Mushroom		- 1
	// 1 = Fireflower	- 2
	// 2 = Iceflower	- 2
		// 3 = Penguin		- 3
		// 4 = Propeller	- 3
	// 5 = MiniShroom	- 2
	// 6 = Starman		- 2
		// 7 = Hammer		- 3
	// 8 = 1-ups		- 2
//
// Format: 1coin, 1coin, 2coins, 3coins, 5coins[3] (Value 6-9), 8coins[5] (Value 10-15)
//
// Possible 5 coin combos =  2,2,2  /  1,2,3  /  2,3,2  /  3,2,3  /  3,3,3
// Possible 8 coin combos =  1,1,2,3,3  /  1,2,2,3,3  /  1,2,3,3,3  /  2,2,2,3,3  /  2,2,3,3,3  /  1,3,3,3,3  /  2,3,3,3,3  /  3,3,3,3,3

const dWMShop_c::ItemTypes dWMShop_c::Inventory[10][12] = { 
	{ // AboveGround
		MUSHROOM, FIRE_FLOWER, ICE_FLOWER, HAMMER,
		FIRE_FLOWER, ICE_FLOWER, FIRE_FLOWER,
		MUSHROOM, MUSHROOM, ONE_UP, MUSHROOM, MUSHROOM
	},
	{ // SuperStar
		STARMAN, STARMAN, STARMAN, STARMAN,
		STARMAN, STARMAN, STARMAN,
		STARMAN, STARMAN, STARMAN, STARMAN, STARMAN
	},
};

void dWMShop_c::loadModels() {
	lakituModel = new ShopModel_c;
	lakituModel->setupLakitu(shopKind);
	lakituModel->x = 240.0f;
	lakituModel->y = 245.0f;
	if (!IsWideScreen()) {
		lakituModel->x = (0.731f * (lakituModel->x + 416.0f)) - 292.0f;
		lakituModel->y *= 0.7711f;
		lakituModel->y += 52.0f;
		lakituModel->scaleFactor = 0.77f;
	}

	static const float itemPos[ITEM_COUNT][2] = {
		{357.0f, 276.0f},
		{450.0f, 276.0f},
		{543.0f, 276.0f},
		{636.0f, 276.0f},

		{380.0f, 190.0f},
		{462.0f, 190.0f},
		{544.0f, 190.0f},

		{363.0f, 104.0f},
		{413.0f, 104.0f},
		{463.0f, 104.0f},
		{513.0f, 104.0f},
		{563.0f, 104.0f},
	};

	itemModels = new ShopModel_c[ITEM_COUNT];
	for (int i = 0; i < ITEM_COUNT; i++) {
		float effectiveX = itemPos[i][0];
		float effectiveY = itemPos[i][1];
		if (!IsWideScreen()) {
			effectiveX = (0.731f * (effectiveX + 416.0f)) - 296.0f;
			effectiveY *= 0.7711f;
			effectiveY += 50.0f;
			itemModels[i].scaleFactor = 1.6f;
		}
		itemModels[i].setupItem(effectiveX, effectiveY, Inventory[shopKind][i]);
	}
}
void dWMShop_c::deleteModels() {
	if (lakituModel)
		delete lakituModel;
	lakituModel = 0;

	if (itemModels)
		delete[] itemModels;
	itemModels = 0;
}


void dWMShop_c::loadInfo() {
	SaveBlock *save = GetSaveFile()->GetBlock(-1);

	// Find out the Shop name
	dLevelInfo_c::entry_s *shopNameEntry =
		dLevelInfo_c::s_info.searchBySlot(shopKind, 98);

	wchar_t shopName[100];
	// TODO: refactor this a bit
	const char *sourceName = dLevelInfo_c::s_info.getNameForLevel(shopNameEntry);
	int charCount = 0;
	
	while (*sourceName != 0 && charCount < 99) {
		shopName[charCount] = *sourceName;
		sourceName++;
		charCount++;
	}
	shopName[charCount] = 0;

	T_title_00->SetString(shopName);
	T_titleS_00->SetString(shopName);
	
	// Recolor the Shop name
	T_title_00->colour1 = save->hudTextColours[0];
	T_title_00->colour2 = save->hudTextColours[1];

	// Load the coin count
	int scCount = getUnspentStarCoinCount();
	WriteNumberToTextBox(&scCount, T_coinUnspent_00, false);
	WriteNumberToTextBox(&scCount, T_coinUnspentS_00, false);
	
	// Recolor it grey if there's no coins
	if (scCount == 0) {
		T_coinUnspent_00->colour1 = (GXColor){255,255,255,255};
		T_coinUnspent_00->colour2 = (GXColor){97,97,97,255};
		
		T_x_00->colour1 = (GXColor){255,255,255,255};
		T_x_00->colour2 = (GXColor){97,97,97,255};
	}

	WriteBMGToTextBox(T_back_00, GetBMG(), 2, 58, 0); // 570
	WriteBMGToTextBox(T_buy_00, GetBMG(), 302, 4, 0); // 77316
	WriteBMGToTextBox(T_coinTitle_00, GetBMG(), 302, 5, 0); // 77317
}


void dWMShop_c::buyItem(int item) {
	static int itemDefs[6][3] = {
		// Cost, Start Index, Count
		{1, 0, 1}, {2, 1, 1}, {2, 2, 1}, {3, 3, 1},
		{5, 4, 3}, {8, 7, 5}
	};

	int cost = itemDefs[item][0], cash = getUnspentStarCoinCount();

	if (cost > cash) {
		OSReport("Started playing Not Enough\n");
		lakituModel->playingNotEnough = true;
		lakituModel->playAnim("notenough", 1.0f, 1);
		MapSoundPlayer(SoundRelatedClass, SE_SYS_INVALID, 1);
		return;
	}

	layout.enableNonLoopAnim(BUY_ITEM+selected);
	MapSoundPlayer(SoundRelatedClass, SE_SYS_DECIDE, 1);

	SaveFile *file = GetSaveFile();
	SaveBlock *block = file->GetBlock(file->header.current_file);

	coinsRemaining = cost;

	// Work out what we need to apply
	int appliedItems[ITEM_TYPE_COUNT];
	for (int i = 0; i < ITEM_TYPE_COUNT; i++)
		appliedItems[i] = 0;

	int invStartIndex = itemDefs[item][1], invCount = itemDefs[item][2];
	for (int i = 0; i < invCount; i++)
		appliedItems[(int)Inventory[shopKind][invStartIndex+i]]++;

	for (int i = 0; i < 8; i++) {
		block->powerups_available[i] += appliedItems[i];

		if (block->powerups_available[i] > 99)
			block->powerups_available[i] = 99;

		dScKoopatlas_c::instance->stockItem->newCounts[i] = block->powerups_available[i];
	}

	// Apply lives to everyone
	for (int i = 0; i < 4; i++) {
		if (Player_Active[i]) {
			int id = Player_ID[i];
			Player_Lives[id] += appliedItems[(int)ONE_UP];
			if (Player_Lives[id] > 99)
				Player_Lives[id] = 99;
		}
	}

	if (appliedItems[(int)ONE_UP] > 0)
		MapSoundPlayer(SoundRelatedClass, SE_SYS_100COIN_ONE_UP, 1);

	state.setState(&StateID_CoinCountdown);
	HideSelectCursor(SelectCursorPointer, 0);
}


void dWMShop_c::beginState_CoinCountdown() {
	timerForCoinCountdown = 8;
}

void dWMShop_c::endState_CoinCountdown() { }

void dWMShop_c::executeState_CoinCountdown() {
	timerForCoinCountdown--;
	if (timerForCoinCountdown <= 0) {
		SaveBlock *save = GetSaveFile()->GetBlock(-1);
		save->spentStarCoins++;

		// load the coin count
		int scCount = getUnspentStarCoinCount();
		WriteNumberToTextBox(&scCount, T_coinUnspent_00, false);
		WriteNumberToTextBox(&scCount, T_coinUnspentS_00, false);
		
		// Recolor it grey if the amount is 0
		if (scCount == 0) {
			T_coinUnspent_00->colour1 = (GXColor){255,255,255,255};
			T_coinUnspent_00->colour2 = (GXColor){97,97,97,255};
			
			T_x_00->colour1 = (GXColor){255,255,255,255};
			T_x_00->colour2 = (GXColor){97,97,97,255};
		}

		layout.enableNonLoopAnim(COUNT_COIN);
		VEC3 efPos = {
			T_coinUnspent_00->effectiveMtx[0][3],
			T_coinUnspent_00->effectiveMtx[1][3],
			0.0f};

		// ARGHHHHHHHHHHHHHHHHh.
		if (IsWideScreen()) {
			float div = 5.0f;
			if (scCount < 100)
				div = 3.6f;
			if (scCount < 10)
				div = 2.7f;
			efPos.x -= (T_coinUnspent_00->size.x / div);
			efPos.y -= (T_coinUnspent_00->size.y / 2.0f);
		} else {
			float div = 5.8f;
			if (scCount < 100)
				div = 8.2f;
			if (scCount < 10)
				div = 14.5f;
			efPos.x += (T_coinUnspent_00->size.x / div);
			efPos.y -= (T_coinUnspent_00->size.y / 2.8f);
		}

		VEC3 efScale = {0.7f, 0.7f, 0.7f};
		SpawnEffect("Wm_2d_moviecoinvanish", 0, &efPos, 0, &efScale);

		coinsRemaining--;
		if (coinsRemaining <= 0) {
			MapSoundPlayer(SoundRelatedClass, SE_PLY_GET_ITEM_AGAIN, 1);
			state.setState(&StateID_Wait);
		} else {
			MapSoundPlayer(SoundRelatedClass, SE_SYS_STAR_COIN_PAY, 1);
			beginState_CoinCountdown();
		}
	}
}


void dWMShop_c::showSelectCursor() {
	switch (selected) {
		case 0: UpdateSelectCursor(P_btnSmBG_00, 0, false); break;
		case 1: UpdateSelectCursor(P_btnSmBG_01, 0, false); break;
		case 2: UpdateSelectCursor(P_btnSmBG_02, 0, false); break;
		case 3: UpdateSelectCursor(P_btnSmBG_03, 0, false); break;
		case 4: UpdateSelectCursor(P_btnLgBG_00, 0, false); break;
		case 5: UpdateSelectCursor(P_btnLgBG_01, 0, false); break;
	}
}

