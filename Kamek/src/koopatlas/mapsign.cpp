#include "koopatlas/mapsign.h"

CREATE_STATE(dWMSign_c, Hidden);
CREATE_STATE(dWMSign_c, ShowWait);
CREATE_STATE(dWMSign_c, Wait);
CREATE_STATE(dWMSign_c, HideWait);


dWMSign_c *dWMSign_c::instance = 0;

dWMSign_c *dWMSign_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dWMSign_c));
	dWMSign_c *c = new(buffer) dWMSign_c;

	instance = c;
	return c;
}

dWMSign_c::dWMSign_c() : state(this, &StateID_LoadMsgFile) {
	layoutLoaded = false;
	visible = false;
}

// Give BRLANs a numerical ID
#define ANIM_IN_WINDOW 0
#define ANIM_OUT_WINDOW 1
#define ANIM_LOOP_BUTTON 2

int dWMSign_c::onCreate() {
	if (!layoutLoaded) {
		bool gotFile = layout.loadArc("wmapSign.arc", false);
		if (!gotFile)
			return false;


		layout.build("wmapSign.brlyt");

		if (IsWideScreen()) {
			layout.layout.rootPane->scale.x = 0.7711f;
		}

		static const char *brlanNames[3] = {
			"wmapSign_inWindow.brlan",
			"wmapSign_outWindow.brlan",
			"wmapSign_loopButton.brlan",
		};

		static const char *groupNames[3] = {
			"A00_Window", "A00_Window", "B00_Button",
		};
		
		layout.loadAnimations(brlanNames, 3);
		layout.loadGroups(groupNames, (int[3]){0,1,2}, 3);
		layout.disableAllAnimations();

		layout.drawOrder = 1;
		
		static const char *tbNames[] = {
			"T_title_00", "T_titleS_00",
			"T_message_00", "T_messageS_00",
		};
		layout.getTextBoxes(tbNames, &T_title_00, 4);

		layoutLoaded = true;
	}

	return true;
}


int dWMSign_c::onDelete() {
	msgDataLoader.unload();
	return layout.free();
}


int dWMSign_c::onExecute() {
	state.execute();

	layout.execAnimations();
	layout.update();
	
	// Report the Node ID if B is pressed
	// Used for the old message loading shit
	/*int nowPressed = Remocon_GetPressed(GetActiveRemocon());
	SaveBlock *save = GetSaveFile()->GetBlock(-1);

	if (nowPressed & WPAD_B) {
		OSReport("Current Path Node ID is: %u\n", save->current_path_node);
	}*/

	return true;
}


int dWMSign_c::onDraw() {
	if (visible)
		layout.scheduleForDrawing();

	return true;
}


void dWMSign_c::show() {
	state.setState(&StateID_ShowWait);
}



// Load the message file
CREATE_STATE_E(dWMSign_c, LoadMsgFile);

void dWMSign_c::executeState_LoadMsgFile() {
	// Load an alternative version of "Messages.bin" that only has sign messages in them
	if (msgDataLoader.load("/SpookyRes/MapSignMessages.bin")) {
		OSReport("Successfully loaded message file\n");
		state.setState(&StateID_Hidden);
	}
}



// Hidden
void dWMSign_c::beginState_Hidden() { }
void dWMSign_c::executeState_Hidden() { }
void dWMSign_c::endState_Hidden() { }



// ShowWait
void dWMSign_c::beginState_ShowWait() {
	MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_IN, 1);

	layout.disableAllAnimations();
	layout.enableLoopAnim(ANIM_LOOP_BUTTON);
	layout.enableNonLoopAnim(ANIM_IN_WINDOW);
	
	visible = true;

	// Get our current node
	dKPNode_s *node = dScKoopatlas_c::instance->pathManager.currentNode;
	
	// We get a message ID from the SIGN node info
	if (node->type == dKPNode_s::SIGN) {
		showWMapMessage(node->messageID + 256); // We add 256 so the user doesn't have to add it to the messageID in Koopatlas
	}
}
void dWMSign_c::executeState_ShowWait() {
	state.setState(&StateID_Wait);
}
void dWMSign_c::endState_ShowWait() { }



// Wait
void dWMSign_c::beginState_Wait() { }
void dWMSign_c::executeState_Wait() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if (nowPressed & WPAD_TWO) {
		// Player pressed 2, now we're gonna start closing the LYT
		state.setState(&StateID_HideWait);
	}
}
void dWMSign_c::endState_Wait() { }



// HideWait
void dWMSign_c::beginState_HideWait() {
	MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_OUT_AUTO, 1);
	layout.enableNonLoopAnim(ANIM_OUT_WINDOW);
}
void dWMSign_c::executeState_HideWait() {
	if (!layout.isAnimOn(ANIM_OUT_WINDOW))
		state.setState(&StateID_Hidden);
}
void dWMSign_c::endState_HideWait() {
	visible = false;
}



void dWMSign_c::showWMapMessage(int id) {
	// Get the data file
	header_s *data = (header_s*)msgDataLoader.buffer;

	const wchar_t *title = 0, *msg = 0;

	for (int i = 0; i < data->count; i++) {
		if (data->entry[i].id == id) {
			title = (const wchar_t*)((u32)data + data->entry[i].titleOffset);
			msg = (const wchar_t*)((u32)data + data->entry[i].msgOffset);
			break;
		}
	}

	// Couldn't find a message at that ID
	if (title == 0) {
		OSReport("Map Sign: Message %08x not found\n", id);
		return;
	}

	// Main textboxes
	T_title_00->SetString(title);
	T_message_00->SetString(msg);
	
	// Text Shadows
	T_titleS_00->SetString(title);
	T_messageS_00->SetString(msg);
}


// Old way of loading sign messages, I'm keeping it here because of how batshit insane it was
/*void dWMSign_c::loadMessageInfo() {
	//SaveBlock *save = GetSaveFile()->GetBlock(-1);
	
	// Loading messages:
	// When loading a message, we first need to make sure the player is at a sign node
	// To do this, we check for the current path node ID (press B while the layout is open, there's code in the onExecute() function that'll print it to the console)
	// While doing that, we also check which world we're currently in, this prevents the issues that could arise from 2 separate worlds happening to have a sign at the same Node ID
	// If we get both of those values back, and they equal the same values as listed below, we load that message
	
	// NOTE: Message IDs here have 256 added to the ID seen in Newer Messages Editor
	
	if (save->newerWorldID == 1) {
		switch (save->current_path_node) {
			case 41:
				showWMapMessage(256);
				break;
			
			case 42:
				showWMapMessage(257);
				break;
			
			case 43:
				showWMapMessage(258);
				break;
			
			case 44:
				showWMapMessage(259);
				break;
			
			case 45:
				showWMapMessage(260);
				break;
			
			case 46:
				showWMapMessage(261);
				break;
			
			case 47:
				showWMapMessage(262);
				break;
		}
	}
	
}*/

