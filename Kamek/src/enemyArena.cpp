#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

// Replaces: EN_GREENCOIN (Sprite 185; Profile ID 410 @ 80AECD48)

class dArenaBalloon_c : public dEn_c {
    mHeapAllocator_c mAllocator;
	m3d::mdl_c model;
    m3d::anmChr_c modelChr;
    m3d::anmClr_c modelClr;
    m3d::anmTexPat_c modelTexPat;

    mEf::es2 effect;
    
    int onCreate();
    int onExecute();
    int onDraw();
    int onDelete();

    void updateModelMatrices();
    void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
    void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);

    u64 eventFlag;
    bool collected;
    int totalBalloonsCollected;

    static dArenaBalloon_c *build();
};

dArenaBalloon_c *dArenaBalloon_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dArenaBalloon_c));
	return new(buffer) dArenaBalloon_c;
}


// Main collision type, this is what triggers when collided with
void dArenaBalloon_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	if (collected) {
		this->Delete(1);
		return;
    }

    //static int totalBalloonsCollected;
    if (totalBalloonsCollected < 8) {
        totalBalloonsCollected++;
    }

    OSReport("Balloon Count: %i\n", totalBalloonsCollected);

    if (totalBalloonsCollected > 8) {
        // Prevent going any higher than the max collection soundID
        PlaySoundAsync(this, 588);
    } else {
        // Balloon collection sounds 581 -> 588
        PlaySoundAsync(this, 580 + totalBalloonsCollected);
    }
	SpawnEffect("Wm_ob_greencoinkira_a", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.1, 1.1, 1.1});


	if (totalBalloonsCollected >= 8) {
		// TODO: ADD SUPPORT FOR EVENTS
        //dFlagMgr_c::instance->flags |= this->eventFlag;
	}
	
	collected = true;

	this->Delete(1);
}

// ...And force all these other collision types to behave the same as above
void dArenaBalloon_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); }
bool dArenaBalloon_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dArenaBalloon_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dArenaBalloon_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dArenaBalloon_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dArenaBalloon_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}


int dArenaBalloon_c::onCreate() {
    mAllocator.link(-1, GameHeaps[0], 0, 0x20);

    // Setup the model
    nw4r::g3d::ResFile modelRes(getResource("kinopio_balloon", "g3d/kinopio_balloon.brres"));
	nw4r::g3d::ResMdl blMdl = modelRes.GetResMdl("kinopio_balloon");
	model.setup(blMdl, &mAllocator, 0x32C, 1, 0);
    SetupTextures_MapObj(&model, 0);

	// Setup CHR animation
    nw4r::g3d::ResAnmChr blChrRes = modelRes.GetResAnmChr("wait");
	modelChr.setup(blMdl, blChrRes, &mAllocator, 0);
	modelChr.bind(&model, blChrRes, false);
	model.bindAnim(&modelChr);

    // Setup CLR animation
    nw4r::g3d::ResAnmClr blClrRes = modelRes.GetResAnmClr("wait");
	modelClr.setup(modelRes.GetResMdl("kinopio_balloon"), blClrRes, &mAllocator, 0, 1);
	modelClr.bind(&model, blClrRes, 0, 0);
	model.bindAnim(&modelClr, 0.0);

    // Setup TexPat animation
    nw4r::g3d::ResAnmTexPat blTexPatRes = modelRes.GetResAnmTexPat("wait");
    modelTexPat.setup(modelRes.GetResMdl("kinopio_balloon"), blTexPatRes, &mAllocator, 0, 1);
	modelTexPat.bindEntry(&model, &blTexPatRes, 0, 0);
    model.bindAnim(&modelTexPat, 0.0f);
	// Do I actually need the two below lines? Or are these default values?
    // Test this at some point
    modelTexPat.setUpdateRateForEntry(1.0f, 0);
	modelTexPat.setFrameForEntry(0, 0);

	mAllocator.unlink();

    ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.xDistToEdge = 12.0;
	HitMeBaby.yDistToEdge = 12.0;
	HitMeBaby.category1 = 0x5;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x200;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

    this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

    this->pos.y += 8.0;

    this->scale.x = 1.0;
	this->scale.y = 1.0;
	this->scale.z = 1.0;

    this->onExecute();
    return true;
}

int dArenaBalloon_c::onDraw() {
	model.scheduleForDrawing();
	return true;
}


void dArenaBalloon_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	//matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);
}

int dArenaBalloon_c::onExecute() {
	updateModelMatrices();

    // Play our anims
    modelChr.process();
    modelClr.process();
    modelTexPat.process();

	effect.spawn("Wm_ob_keyget02_kira", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.1, 1.1, 1.1});
	return true;
}

int dArenaBalloon_c::onDelete() {
	return true;
}