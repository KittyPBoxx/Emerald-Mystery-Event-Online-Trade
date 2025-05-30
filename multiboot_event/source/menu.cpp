/****************************************************************************
 * Pokecom Channel
 * KittyPBoxx 2023
 *
 * menu.cpp
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#ifdef TARGET_WII
	#include <wiiuse/wpad.h>
#endif

#include "libwiigui/gui.h"
#include "menu.h"
#include "main.h"
#include "input.h"
#include "gettext.h"

extern "C" {
	#include "linkcableclient.h"
}

#define THREAD_SLEEP 10000

static GuiImageData * pointer[1];
static GuiImage * bgImg = nullptr;
static GuiSound * bgMusic = nullptr;
static GuiWindow * mainWindow = nullptr;
static GuiGbaConnections * connections = nullptr;
static GuiNumpad * numpad = nullptr;
static lwp_t guithread = LWP_THREAD_NULL;
static bool guiHalt = true;
static bool padButtonPressed = false;
static bool padShown = false;

s8 gAnalogStickX;
s8 gAnalogStickY;
int pointerX = 50;
int pointerY = 50;

char ipv4[32] = "127.0.0.1:9000";
char serverName[32] = "\0";
bool serverSet = false;

bool isPlayerConnected;
bool isPlayerNamed;

/****************************************************************************
 * ResumeGui
 *
 * Signals the GUI thread to start, and resumes the thread. This is called
 * after finishing the removal/insertion of new elements, and after initial
 * GUI setup.
 ***************************************************************************/
static void ResumeGui()
{
	guiHalt = false;
	LWP_ResumeThread (guithread);
}

/****************************************************************************
 * HaltGui
 *
 * Signals the GUI thread to stop, and waits for GUI thread to stop
 * This is necessary whenever removing/inserting new elements into the GUI.
 * This eliminates the possibility that the GUI is in the middle of accessing
 * an element that is being changed.
 ***************************************************************************/
static void HaltGui()
{
	guiHalt = true;

	// wait for thread to finish
	while(!LWP_ThreadIsSuspended(guithread))
		usleep(THREAD_SLEEP);
}

static const char * resolveNetworkMessage(u8 state)
{
	switch(state)
	{
		case CONNECTION_SUCCESS: 
		{
			int bufferSize = strlen(gettext("networkTest.CONNECTION_SUCCESS")) + strlen(getServerName()) + 1;
			char* concatString = new char[ bufferSize ];
			strcpy( concatString, gettext("networkTest.CONNECTION_SUCCESS") );
			strcat( concatString, getServerName());
			return concatString;
		}
		break;
		case CONNECTION_ERROR_INVALID_IP: 
			return gettext("networkTest.CONNECTION_ERROR_INVALID_IP");
		break;
		case CONNECTION_ERROR_COULD_NOT_RESOLVE_IPV4:
			return gettext("networkTest.CONNECTION_ERROR_COULD_NOT_RESOLVE_IPV4");
		break;
		case CONNECTION_ERROR_NO_NETWORK_DEVICE: 
			return gettext("networkTest.CONNECTION_ERROR_NO_NETWORK_DEVICE");
		break;
		case CONNECTION_ERROR_CONNECTION_FAILED: 
			return gettext("networkTest.CONNECTION_ERROR_CONNECTION_FAILED");
		break;
		case CONNECTION_ERROR_INVALID_RESPONSE: 
			return gettext("networkTest.CONNECTION_ERROR_INVALID_RESPONSE");
		break;
		case CONNECTION_STARTING: 
			return gettext("networkTest.CONNECTION_STARTING");
		case CONNECTION_INIT:	
		default:
			return gettext("networkTest.CONNECTION_INIT");
	}
}

/****************************************************************************
 * NetworkTestPopup
 *
 * Displays a window to the user while trying to connect to the network
 ***************************************************************************/
int NetworkTestPopup(const char *title, const char *msg, const char *btn1Label, GuiSound* bgMusic)
{
	int choice = -1;

	if (bgMusic->GetVolume() != 0)
	{
		bgMusic->SetVolume(15);
	}

	GuiWindow promptWindow(448,288);
	promptWindow.SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND::PCM);
	btnSoundOver.SetVolume(20);
	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiTrigger trigA;

	#ifdef TARGET_WII
		trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	#else 
		trigA.SetSimpleTrigger(-1, 0, PAD_BUTTON_A);	
	#endif
	

	GuiImageData dialogBox(dialogue_box_png);
	GuiImage dialogBoxImg(&dialogBox);

	GuiText titleTxt(title, 26, (GXColor){0, 0, 0, 255});
	titleTxt.SetAlignment(ALIGN_H::CENTRE, ALIGN_V::TOP);
	titleTxt.SetPosition(0,40);
	GuiText msgTxt(msg, 22, (GXColor){0, 0, 0, 255});
	msgTxt.SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	msgTxt.SetPosition(0,-20);
	msgTxt.SetWrap(true, 400);

	GuiText infoTxt(msg, 14, (GXColor){137, 207, 240, 255});
	infoTxt.SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	infoTxt.SetPosition(0,35);
	infoTxt.SetWrap(true, 400);
	infoTxt.SetVisible(false);
	infoTxt.SetText(gettext("networkTest.connectionOverrideNotification"));

	GuiText btn1Txt(btn1Label, 22, (GXColor){0, 0, 0, 255});
	GuiImage btn1Img(&btnOutline);
	GuiImage btn1ImgOver(&btnOutlineOver);
	GuiButton btn1(btnOutline.GetWidth(), btnOutline.GetHeight());

	GuiLoader loader;
	loader.SetPosition(20, -40);
	loader.SetAlignment(ALIGN_H::CENTRE, ALIGN_V::BOTTOM);

	btn1.SetAlignment(ALIGN_H::CENTRE, ALIGN_V::BOTTOM);
	btn1.SetPosition(0, -25);

	btn1.SetLabel(&btn1Txt);
	btn1.SetImage(&btn1Img);
	btn1.SetImageOver(&btn1ImgOver);
	btn1.SetSoundOver(&btnSoundOver);
	btn1.SetTrigger(&trigA);
	btn1.SetState(STATE::SELECTED);
	btn1.SetEffectGrow();

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&msgTxt);
	promptWindow.Append(&infoTxt);
	promptWindow.Append(&loader);
	promptWindow.Append(&btn1);

	promptWindow.SetEffect(EFFECT::SLIDE_TOP | EFFECT::SLIDE_IN, 40);
	HaltGui();
	mainWindow->SetState(STATE::DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	u32 connectionTestResult = testTCPConnection(ipv4);

	GuiSound connectingSound(connecting_pcm, connecting_pcm_size, SOUND::PCM);
	connectingSound.Play();

	u32 isStarting = 1;
	u8 connectionRetries = 0;

	// Make sure a minimum amount of refreshes has always happened before we display a result
	int updatesBeforeResult = 0;

	while(choice == -1)
	{
		usleep(THREAD_SLEEP);
		bgMusic->Play();

		if (isStarting)
		{
			connectingSound.Loop();
		}

		if(btn1.GetState() == STATE::CLICKED || PAD_ButtonsDown(0) & PAD_BUTTON_X)
		{
			choice = 1;
		}
		else if (updatesBeforeResult > 300)
		{

			if ((connectionTestResult >= CONNECTION_ERROR_INVALID_RESPONSE) && connectionRetries < 10)
			{
				updatesBeforeResult = 0;
				isStarting = 1;
				connectionRetries++;
				connectionTestResult = testTCPConnection(ipv4);
			} 
			else
			{
				if (connectionTestResult == CONNECTION_SUCCESS)
				{
					GuiSound connectingSuccessSound(success_pcm, success_pcm_size, SOUND::PCM);
					connectingSuccessSound.SetVolume(20);
					connectingSuccessSound.Play();
					infoTxt.SetVisible(true);
					setOverrideAddress(ipv4);
				}
				else
				{
					GuiSound connectingErrorSound(fail_pcm, fail_pcm_size, SOUND::PCM);
					connectingErrorSound.SetVolume(20);
					connectingErrorSound.Play();
				}

				isStarting = 0;
				connectingSound.Stop();
				loader.SetVisible(false);
				msgTxt.SetText(resolveNetworkMessage(connectionTestResult));
			} 
		} 
		else 
		{
			updatesBeforeResult++;
		}

	}

	connectingSound.Stop();

	if (bgMusic->GetVolume() != 0)
	{
		bgMusic->SetVolume(50);
	}

	promptWindow.SetEffect(EFFECT::SLIDE_TOP | EFFECT::SLIDE_OUT, 50);
	while(promptWindow.GetEffect() > 0) usleep(THREAD_SLEEP);
	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE::DEFAULT);
	ResumeGui();
	return choice;
}

/****************************************************************************
 * UpdateGUI
 *
 * Primary thread to allow GUI to respond to state changes, and draws GUI
 ***************************************************************************/
static void *UpdateGUI (void *arg)
{
	(void)arg;

	int i;

	while(1)
	{
		if(guiHalt)
		{
			LWP_SuspendThread(guithread);
		}
		else
		{
			SetupPads(); // Pads a re-setup each time, because otherwise data gets messed up by disconnect/reconnect cycles 
			UpdatePads();
			mainWindow->Draw();


			gAnalogStickX = PAD_StickX(0);
    		gAnalogStickY = PAD_StickY(0);

			if (PAD_ButtonsHeld(0) & PAD_BUTTON_RIGHT)
			{
				gAnalogStickX = 20;
				padButtonPressed = true;
			}
			else if (PAD_ButtonsHeld(0) & PAD_BUTTON_LEFT)
			{
				gAnalogStickX = -20;
				padButtonPressed = true;
			}

			if (PAD_ButtonsHeld(0) & PAD_BUTTON_UP)
			{
				gAnalogStickY = 20;
				padButtonPressed = true;
			}
			else if (PAD_ButtonsHeld(0) & PAD_BUTTON_DOWN)
			{
				gAnalogStickY = -20;
				padButtonPressed = true;
			}

			if (gAnalogStickX > 10 || gAnalogStickX < -10)
			{
				pointerX = MIN(608, MAX(10, pointerX + (2 * gAnalogStickX / 10)));
				padButtonPressed = true;
			}

			if (gAnalogStickY > 10 || gAnalogStickY < -10)
			{
				pointerY = MIN(435, MAX(10, pointerY + (-1 * 2 * gAnalogStickY / 10)));
				padButtonPressed = true;
			}

			userInput[0].padX = pointerX;
			userInput[0].padY = pointerY;

			#ifdef TARGET_WII
					if(userInput[0].wpad->ir.valid)
					{
						Menu_DrawImg(userInput[0].wpad->ir.x-48, userInput[0].wpad->ir.y-48,
							96, 96, pointer[0]->GetImage(), userInput[0].wpad->ir.angle, 1, 1, 255);

						userInput[0].padX = userInput[0].wpad->ir.x-48;
						userInput[0].padY = userInput[0].wpad->ir.y-48;
					}
					else if (padButtonPressed)
					{
						Menu_DrawImg(pointerX-48, pointerY-48, 96, 96, pointer[0]->GetImage(), 0, 1, 1, 255);
					}
					DoRumble(0);
			#else
				if (padButtonPressed)
					Menu_DrawImg(pointerX-48, pointerY-48, 96, 96, pointer[0]->GetImage(), 0, 1, 1, 255);
			#endif



			Menu_Render();

			mainWindow->Update(&userInput[0]);

			if(ExitRequested)
			{
				for(i = 0; i <= 255; i += 15)
				{
					mainWindow->Draw();
					Menu_DrawRectangle(0,0,screenwidth,screenheight,(GXColor){0, 0, 0, (u8)i},1);
					Menu_Render();
				}
				ExitApp();
			}
		}
	}
	return nullptr;
}


/****************************************************************************
 * InitGUIThread
 *
 * Startup GUI threads
 ***************************************************************************/
void InitGUIThreads()
{
	LWP_CreateThread (&guithread, UpdateGUI, nullptr, nullptr, 0, 20);
}

/****************************************************************************
 * MenuSettings
 ***************************************************************************/
static int MenuSettings(GuiSound* bgMusic, GuiGbaConnections * connections)
{
	int menu = MENU_NONE;

	GuiText titleTxt(gettext("main.title"), 24, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_H::LEFT, ALIGN_V::TOP);
	titleTxt.SetPosition(50,35);
	titleTxt.SetEffect(EFFECT::SLIDE_TOP | EFFECT::SLIDE_IN, 25);

	GuiText versionTxt(gettext("main.version"), 12, (GXColor){255, 255, 255, 100});
	versionTxt.SetAlignment(ALIGN_H::RIGHT, ALIGN_V::TOP);
	versionTxt.SetPosition(-20,20);

	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND::PCM);
	btnSoundOver.SetVolume(20);
	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiImageData btnLongOutline(button_long_png);
	GuiImageData btnLongOutlineOver(button_long_over_png);

	GuiTrigger trigA;
	#ifdef TARGET_WII
		trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	#else 
		trigA.SetSimpleTrigger(-1, 0, PAD_BUTTON_A);	
	#endif

	GuiSound btnClick(swish_pcm, swish_pcm_size, SOUND::PCM);
	btnClick.SetVolume(40);

	connections->SetPosition(-125, 20);
	connections->SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	connections->SetEffect(EFFECT::SLIDE_BOTTOM | EFFECT::SLIDE_IN, 40);

	GuiText networkBtnTxt(gettext("main.network"), 20, (GXColor){0, 0, 0, 255});
	networkBtnTxt.SetWrap(false, btnLongOutline.GetWidth()-30);
	GuiImage networkBtnImg(&btnLongOutline);
	GuiImage networkBtnImgOver(&btnLongOutlineOver);
	GuiButton networkBtn(btnLongOutline.GetWidth(), btnLongOutline.GetHeight());
	networkBtn.SetAlignment(ALIGN_H::LEFT, ALIGN_V::BOTTOM);
	networkBtn.SetPosition(50, -35);
	networkBtn.SetLabel(&networkBtnTxt);
	networkBtn.SetImage(&networkBtnImg);
	networkBtn.SetImageOver(&networkBtnImgOver);
	networkBtn.SetSoundOver(&btnSoundOver);
	networkBtn.SetSoundClick(&btnClick);
	networkBtn.SetTrigger(&trigA);
	networkBtn.SetEffectGrow();

	GuiText exitBtnTxt(gettext("main.exit"), 22, (GXColor){0, 0, 0, 255});
	GuiImage exitBtnImg(&btnOutline);
	exitBtnImg.Tint(50, -100, -100);
	GuiImage exitBtnImgOver(&btnOutlineOver);
	exitBtnImgOver.Tint(50, -50, -50);
	GuiButton exitBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	exitBtn.SetAlignment(ALIGN_H::RIGHT, ALIGN_V::BOTTOM);
	exitBtn.SetPosition(-50, -35);
	exitBtn.SetLabel(&exitBtnTxt);
	exitBtn.SetImage(&exitBtnImg);
	exitBtn.SetImageOver(&exitBtnImgOver);
	exitBtn.SetSoundOver(&btnSoundOver);
	exitBtn.SetSoundClick(&btnClick);
	exitBtn.SetTrigger(&trigA);
	exitBtn.SetEffectGrow();
	exitBtn.SetState(STATE::DISABLED);

	isPlayerConnected = isConnected();
	isPlayerNamed = false;

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&titleTxt);
	w.Append(&versionTxt);
	w.Append(connections);
	w.Append(&networkBtn);
	w.Append(&exitBtn);

	mainWindow->Append(&w);

	bgImg->ApplyBackgroundPattern();

	ResumeGui();

	int soundFadeLoop = 0;	
	bool isMuted = bgMusic->GetVolume() == 0;

	while(menu == MENU_NONE)
	{
		usleep(THREAD_SLEEP);
		bgMusic->Play();
		if(networkBtn.GetState() == STATE::CLICKED)
		{
			menu = MENU_SETTINGS_NETWORK;
		}
		else if(exitBtn.GetState() == STATE::CLICKED)
		{
			menu = MENU_EXIT;
		}
		else if (PAD_ButtonsDown(0) & PAD_BUTTON_X) 
		{
			menu = MENU_SETTINGS_NETWORK;
		}
		
		if (PAD_ButtonsHeld(0) == (PAD_TRIGGER_Z) && (exitBtn.GetState() == STATE::DISABLED || exitBtn.GetState() == STATE::DEFAULT)) 
		{
			exitBtn.SetState(STATE::DEFAULT);
		}
		else if (exitBtn.GetState() == STATE::DEFAULT)
		{
			exitBtn.SetState(STATE::DISABLED);
		}

		if (PAD_ButtonsDown(0) & PAD_BUTTON_START 
				#ifdef TARGET_WII
					|| WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS
				#endif	
			) 
		{
			connections->ResetLink();
			resetLink();
			isPlayerConnected = false;
			isPlayerNamed = false;
		}
		else 
		{
			/* Handle GBA connections in the ui */
			if (!isPlayerConnected && isConnected() > 0)
			{
				connections->ConnectPlayer();
				isPlayerConnected = true;
			} 

			if (isPlayerConnected && !isPlayerNamed && hasPlayerName())
			{
				connections->SetPlayerName(getPlayerName());
				isPlayerNamed = true;
			}
		}

		if (!padShown && padButtonPressed)
		{
			padShown = true;
			connections->ShowController();
		}	

		if (!serverSet)
		{
			if (hasServerName())
			{
				strcpy(serverName, getServerName());
				connections->SetServerName(serverName);
				serverSet = true;
			}
		}

		if (!isMuted && isPlayerConnected && bgMusic->GetVolume() > 0)
		{
			if (soundFadeLoop % 100000 == 0)
			{
				bgMusic->SetVolume((bgMusic->GetVolume() - 1));
			} 
			else 
			{
				soundFadeLoop++;
			}
			
		} 
		else if (bgMusic->GetVolume() == 0)
		{
			isMuted = true;
		}

	}

	HaltGui();

	w.Remove(&titleTxt);
	w.Remove(&versionTxt);
	w.Remove(connections);

	w.Remove(&exitBtn);
	w.Remove(&networkBtn);
	mainWindow->Remove(&w);
	return menu;
}

/****************************************************************************
 * NetworkConfigurationMenu
 ***************************************************************************/

static int NetworkConfigurationMenu(GuiSound* bgMusic, GuiNumpad * numpad)
{
	int menu = MENU_NONE;

	GuiText titleTxt(gettext("network.title"), 24, (GXColor){255, 255, 255, 255});
	titleTxt.SetAlignment(ALIGN_H::LEFT, ALIGN_V::TOP);
	titleTxt.SetPosition(50,35);
	titleTxt.SetEffect(EFFECT::SLIDE_TOP | EFFECT::SLIDE_IN, 25);

	GuiText versionTxt(gettext("main.version"), 12, (GXColor){255, 255, 255, 100});
	versionTxt.SetAlignment(ALIGN_H::RIGHT, ALIGN_V::TOP);
	versionTxt.SetPosition(-20,20);

	GuiSound btnSoundOver(button_over_pcm, button_over_pcm_size, SOUND::PCM);
	btnSoundOver.SetVolume(20);
	GuiImageData btnOutline(button_png);
	GuiImageData btnOutlineOver(button_over_png);
	GuiImageData btnLongOutline(button_long_png);
	GuiImageData btnLongOutlineOver(button_long_over_png);

	GuiSound btnClick(swish_pcm, swish_pcm_size, SOUND::PCM);
	btnClick.SetVolume(40);

	GuiTrigger trigA;
	#ifdef TARGET_WII
		trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	#else 
		trigA.SetSimpleTrigger(-1, 0, PAD_BUTTON_A);	
	#endif

	GuiText backBtnTxt(gettext("network.back"), 22, (GXColor){0, 0, 0, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_H::LEFT, ALIGN_V::BOTTOM);
	backBtn.SetPosition(50, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetSoundOver(&btnSoundOver);
	backBtn.SetSoundClick(&btnClick);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();

	GuiText testConnectionBtnTxt(gettext("network.test"), 22, (GXColor){0, 0, 0, 255});
	testConnectionBtnTxt.SetWrap(false, btnLongOutline.GetWidth()-30);
	GuiImage testConnectionBtnImg(&btnLongOutline);
	GuiImage testConnectionBtnImgOver(&btnLongOutlineOver);
	GuiButton testConnectionBtn(btnLongOutline.GetWidth(), btnLongOutline.GetHeight());
	testConnectionBtn.SetAlignment(ALIGN_H::RIGHT, ALIGN_V::BOTTOM);
	testConnectionBtn.SetPosition(-50, -35);
	testConnectionBtn.SetLabel(&testConnectionBtnTxt);
	testConnectionBtn.SetImage(&testConnectionBtnImg);
	testConnectionBtn.SetImageOver(&testConnectionBtnImgOver);
	testConnectionBtn.SetSoundOver(&btnSoundOver);
	testConnectionBtn.SetTrigger(&trigA);
	testConnectionBtn.SetEffectGrow();

	numpad->SetPosition(-107, -60);
	numpad->SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	numpad->SetEffect(EFFECT::SLIDE_BOTTOM | EFFECT::SLIDE_IN, 40);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(numpad);
	w.Append(&backBtn);
	w.Append(&testConnectionBtn);
	mainWindow->Append(&w);
	mainWindow->Append(&titleTxt);
	mainWindow->Append(&versionTxt);
	ResumeGui();

	while(menu == MENU_NONE)
	{
		usleep(THREAD_SLEEP);

		bgMusic->Play();

		if(backBtn.GetState() == STATE::CLICKED || PAD_ButtonsDown(0) & PAD_BUTTON_X)
		{
			menu = MENU_SETTINGS;
		}
		else if (testConnectionBtn.GetState() == STATE::CLICKED || PAD_ButtonsDown(0) & PAD_BUTTON_Y)
		{
			NetworkTestPopup(gettext("networkTest.title"),
				             gettext("networkTest.attemptingConnection"),
				             gettext("networkTest.close"), 
				             bgMusic);
		}
		else if ( PAD_ButtonsDown(0) & PAD_BUTTON_B) 
		{
			numpad->DeleteKey();
		}
	}
	HaltGui();

	w.Remove(numpad);
	w.Remove(&backBtn);
	w.Remove(&testConnectionBtn);
	mainWindow->Remove(&w);
	mainWindow->Remove(&titleTxt);
	mainWindow->Remove(&versionTxt);
	return menu;
}


/****************************************************************************
 * MainMenu
 ***************************************************************************/
void MainMenu()
{
	int currentMenu = MENU_SETTINGS;

	pointer[0] = new GuiImageData(player1_point_png);

	mainWindow = new GuiWindow(screenwidth, screenheight);

	bgImg = new GuiImage(screenwidth, screenheight, (GXColor){20, 20, 20, 255});
	bgImg->ApplyBackgroundPattern();
	mainWindow->Append(bgImg);

	ResumeGui();

	bgMusic = new GuiSound(bg_music_mp3, bg_music_mp3_size, SOUND::MP3);
	bgMusic->SetVolume(50);
	bgMusic->Play(); // startup music

	// The connections object has a lot of image elements. Constantly recreating it is asking for trouble 
	connections = new GuiGbaConnections(serverName);

	numpad = new GuiNumpad(gettext("network.prompt"), ipv4, 32);

	while(currentMenu != MENU_EXIT)
	{
		switch (currentMenu)
		{
			case MENU_SETTINGS:
				currentMenu = MenuSettings(bgMusic, connections);
				break;
			case MENU_SETTINGS_NETWORK:
				currentMenu = NetworkConfigurationMenu(bgMusic, numpad);
				break;
			default: // unrecognized menu
				currentMenu = MenuSettings(bgMusic, connections);
				break;
		}
	}

	ResumeGui();
	ExitRequested = 1;

	HaltGui();

	bgMusic->Stop();
	delete bgMusic;
	delete bgImg;
	delete mainWindow;

	delete pointer[0];

	mainWindow = nullptr;

	ExitApp();
}
