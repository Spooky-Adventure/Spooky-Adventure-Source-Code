// Some stuff to get tomorrow's me started out...

// Here's a snippet of code from the HUD to get the controller stuff working
/*
(in ctor i think): displayedControllerType = -1;

int cntType = RemoconMng->controllers[0]->controllerType;

if (cntType != displayedControllerType) {
	displayedControllerType = cntType;

	int beef = (cntType == 0) ? 0 : 1;
	GameMgrP->currentControllerType = beef;
}
*/

// Here's layout names:
// Wii Remote:  extensionControllerYokomochi
// Nunchuk:     extensionControllerNunchuk
// Classic:     extensionControllerClasic (also applies to all other unsupported controllers)

// TODO:
// - Figure out why Newer straight-up doesn't have the controller warning anymore