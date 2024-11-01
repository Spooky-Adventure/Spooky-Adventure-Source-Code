#ifndef __KOOPATLAS_MAPSIGN_H
#define __KOOPATLAS_MAPSIGN_H

#include <common.h>
#include <game.h>
#include "koopatlas/core.h"

class dWMSign_c : public dActor_c {
	public:
		static dWMSign_c *build();
		static dWMSign_c *instance;

		dWMSign_c();

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		bool visible;
		bool layoutLoaded;
		m2d::EmbedLayout_c layout;
		dDvdLoader_c msgDataLoader;

		nw4r::lyt::TextBox
			*T_title_00, *T_titleS_00,
			*T_message_00, *T_messageS_00;

		void show();
		void loadMessageInfo(); // Unused
		void showWMapMessage(int id);
		

		dStateWrapper_c<dWMSign_c> state;

		USING_STATES(dWMSign_c);
		DECLARE_STATE(LoadMsgFile);
		DECLARE_STATE(Hidden);
		DECLARE_STATE(ShowWait);
		DECLARE_STATE(Wait);
		DECLARE_STATE(HideWait);
	
	private:
		// Message loading
		struct entry_s {
			u32 id;
			u32 titleOffset;
			u32 msgOffset;
		};

		struct header_s {
			u32 count;
			entry_s entry[1];
		};
};

#endif

