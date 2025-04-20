/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * input.cpp
 * Wii/GameCube controller management
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ogcsys.h>
#include <unistd.h>

#include "menu.h"
#include "video.h"
#include "input.h"
#include "libwiigui/gui.h"

int rumbleRequest[4] = {0,0,0,0};
static int rumbleCount[4] = {0,0,0,0};
GuiTrigger userInput[1];

/****************************************************************************
 * UpdatePads
 *
 * Scans pad and wpad
 ***************************************************************************/
void UpdatePads()
{
	PAD_ScanPads();

	#ifdef TARGET_WII
		WPAD_ScanPads();
	#endif	

	userInput[0].pad.btns_d = PAD_ButtonsDown(0);
	userInput[0].pad.btns_u = PAD_ButtonsUp(0);
	userInput[0].pad.btns_h = PAD_ButtonsHeld(0);
	userInput[0].pad.stickX = PAD_StickX(0);
	userInput[0].pad.stickY = PAD_StickY(0);
	userInput[0].pad.substickX = PAD_SubStickX(0);
	userInput[0].pad.substickY = PAD_SubStickY(0);
	userInput[0].pad.triggerL = PAD_TriggerL(0);
	userInput[0].pad.triggerR = PAD_TriggerR(0);
}

/****************************************************************************
 * SetupPads
 *
 * Sets up userInput triggers for use
 ***************************************************************************/
void SetupPads()
{
	PAD_Init();

	#ifdef TARGET_WII
		WPAD_Init();
		WPAD_SetDataFormat(WPAD_CHAN_ALL,WPAD_FMT_BTNS_ACC_IR);
		WPAD_SetVRes(WPAD_CHAN_ALL, screenwidth, screenheight);
		userInput[0].chan = 0;
		userInput[0].wpad = WPAD_Data(0);
	#endif	
}



/****************************************************************************
 * ShutoffRumble
 ***************************************************************************/

 void ShutoffRumble()
 {
	WPAD_Rumble(0, 0);
	rumbleCount[0] = 0;
 }
 
 /****************************************************************************
  * DoRumble
  ***************************************************************************/
 
 void DoRumble(int i)
 {
	 if(rumbleRequest[i] && rumbleCount[i] < 3)
	 {
		 WPAD_Rumble(i, 1); // rumble on
		 rumbleCount[i]++;
	 }
	 else if(rumbleRequest[i])
	 {
		 rumbleCount[i] = 12;
		 rumbleRequest[i] = 0;
	 }
	 else
	 {
		 if(rumbleCount[i])
			 rumbleCount[i]--;
		 WPAD_Rumble(i, 0); // rumble off
	 }
 }
 