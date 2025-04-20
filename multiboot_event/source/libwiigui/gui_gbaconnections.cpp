/****************************************************************************
 * libwiigui
 *
 * KittyPBoxx 2023
 *
 * gui_gbaconnections.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "gui.h"
#include "gettext.h"

GuiGbaConnections::GuiGbaConnections(char * serverName)
{
    width = 300;
    height = 400;

	alignmentHor = ALIGN_H::CENTRE;
	alignmentVert = ALIGN_V::MIDDLE;

    playerTag = new GuiImageData(player_tag_png);
    p1GBA = new GuiImageData(p1_gba_png);
    p1Link = new GuiImageData(p1_link_png);
    cube = new GuiImageData(cube_png);
    c1 = new GuiImageData(c1_png);
    c1Link = new GuiImageData(c1_link_png);

    playerTagImg = new GuiImage(playerTag);
    playerTagImg->SetPosition(290, 260);
    playerTagImg->SetParent(this);
    playerTagImg->Tint(20,20,20);

    playerTagText = new GuiText(gettext("connectionsUI.playerWaiting"), 12, (GXColor){0, 0, 0, 0xff});
	playerTagText->SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	playerTagText->SetPosition(0, 0);
    playerTagText->SetParent(playerTagImg);

    HidePlayerTag();

    p1GBAImg = new GuiImage(p1GBA);
    p1GBAImg->SetPosition(300, 185);
    p1GBAImg->SetParent(this);
    p1GBAImg->SetAlpha(50);

    p1LinkImg = new GuiImage(p1Link);
    p1LinkImg->SetPosition(257, 128);
    p1LinkImg->SetParent(this);
    p1LinkImg->SetAlpha(50);

    p1Screen = new GuiImage(32, 25, (GXColor){240, 240, 240, 255});
    p1Screen->ApplyBackgroundPattern();
    p1Screen->SetPosition(28, 29);
    p1Screen->SetParent(p1GBAImg);
    p1Screen->Tint(120,120,120);
    p1Screen->SetAlpha(0);

    cubeImg = new GuiImage(cube);
    cubeImg->SetPosition(232, 75);
    cubeImg->SetAlpha(200);
    cubeImg->SetParent(this);

    c1Img = new GuiImage(c1);
    c1Img->SetPosition(158, 185);
    c1Img->SetParent(this);
    c1Img->SetAlpha(50);

    c1LinkImg = new GuiImage(c1Link);
    c1LinkImg->SetPosition(200, 130);
    c1LinkImg->SetParent(this);
    c1LinkImg->SetAlpha(50);


    if(serverName[0] == '\0' || strcmp(serverName,gettext("connectionsUI.noServer")) == 0) 
	{
        strcpy( serverName, gettext("connectionsUI.noServer"));
        serverTagText = new GuiText(serverName, 16, (GXColor){255, 195, 0, 0xff});
	}
    else
    {
        cubeImg->Tint(10, 10, 10);
        serverTagText = new GuiText(serverName, 16, (GXColor){70, 130, 180, 0xff});
    }
	serverTagText->SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	serverTagText->SetPosition(0, -60);
    serverTagText->SetParent(cubeImg);

    controllerInstructions = new GuiText(gettext("connectionsUI.controllerInstructions"), 16, (GXColor){70, 130, 180, 0xff});
    controllerInstructions->SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	controllerInstructions->SetPosition(0, 175);
    controllerInstructions->SetParent(cubeImg);
    controllerInstructions->SetEffect(EFFECT::PLUSE, -3, 100);

    resetLinkTxt = new GuiText(gettext("main.resetLinkInfo"), 16, (GXColor){70, 130, 180, 0xff});
    resetLinkTxt->SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	resetLinkTxt->SetPosition(0, 205);
    resetLinkTxt->SetParent(cubeImg);
    resetLinkTxt->SetVisible(false);

    gbaInstructions = new GuiText(gettext("connectionsUI.gbaInstructions"), 16, (GXColor){70, 130, 180, 0xff});
    gbaInstructions->SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	gbaInstructions->SetPosition(0, 195);
    gbaInstructions->SetParent(cubeImg);
    gbaInstructions->SetEffect(EFFECT::PLUSE, -3, 100);

}

void GuiGbaConnections::ShowController()
{
    c1Img->SetAlpha(255);
    c1LinkImg->SetAlpha(255);
    c1LinkImg->Tint(-10, -10, 20);
}

void GuiGbaConnections::ResetLink() 
{
    controllerInstructions->SetVisible(true);
    resetLinkTxt -> SetVisible(false);
    gbaInstructions->SetVisible(true);

    p1GBAImg->SetAlpha(50);
    p1Link->SetImage(p1_link_png, 0, 0);
    p1LinkImg->SetImage(p1Link);
    p1LinkImg->SetAlpha(50);
    HidePlayerTag();
    p1Screen->SetAlpha(0);
}


void GuiGbaConnections::ConnectPlayer() 
{
    controllerInstructions->SetVisible(false);
    resetLinkTxt -> SetVisible(true);
    gbaInstructions->SetVisible(false);
    p1GBAImg->SetAlpha(255);
    p1LinkImg->SetAlpha(255);
    p1LinkImg->Tint(-10, -10, 20);
    ShowPlayerTag();
    p1Screen->SetAlpha(220);
}

void GuiGbaConnections::SetPlayerName(char * name) 
{
    playerTagText->SetText(name);
}

void GuiGbaConnections::SetServerName(char * name)
{
    delete serverTagText;
    serverTagText = new GuiText(name, 16, (GXColor){70, 130, 180, 0xff});
    serverTagText->SetAlignment(ALIGN_H::CENTRE, ALIGN_V::MIDDLE);
	serverTagText->SetPosition(0, -60);
    serverTagText->SetParent(cubeImg);
}

void GuiGbaConnections::Draw()
{
	if(!this->IsVisible())
		return;

    cubeImg->Draw();    

    playerTagImg->Draw();

    playerTagText->Draw();

    p1GBAImg->Draw();

    p1LinkImg->Draw();

    p1Screen->Draw();


    c1Img->Draw();
    c1LinkImg->Draw();

    serverTagText->Draw();

    controllerInstructions->Draw();
    gbaInstructions->Draw();
    resetLinkTxt->Draw();

	this->UpdateEffects();
}

GuiGbaConnections::~GuiGbaConnections()
{
    delete playerTag;
    delete p1GBA;
    delete p1Link;
    delete cube;
    delete c1;
    delete c1Link;

    delete playerTagImg;

    delete p1GBAImg;

    delete p1LinkImg;

    delete p1Screen;

    delete cubeImg;

    delete c1Img;
    delete c1LinkImg;

}

void GuiGbaConnections::Update(GuiTrigger * t)
{
    (void) t;
}

void GuiGbaConnections::HidePlayerTag()
{
    playerTagImg->SetVisible(false);
    playerTagText->SetVisible(false);
}

void GuiGbaConnections::ShowPlayerTag()
{
    playerTagImg->SetVisible(true);
    playerTagText->SetVisible(true);

    controllerInstructions->SetVisible(false);
    gbaInstructions->SetVisible(false);
}