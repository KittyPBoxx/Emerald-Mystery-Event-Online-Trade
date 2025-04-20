#ifndef LIBWIIGUI_GBACONNECTIONS_H
#define LIBWIIGUI_GBACONNECTIONS_H

//!GUI For gba link connecitons
class GuiGbaConnections : public GuiElement {
public:
	GuiGbaConnections(char * serverName);
	~GuiGbaConnections();
    void Draw();
    void ConnectPlayer();
	void Update(GuiTrigger * t);
    void SetPlayerName(char * name);
    void SetServerName(char * name);
    void ResetLink();
    void ShowController();
protected:
    void HidePlayerTag();
    void ShowPlayerTag();

    GuiImageData * playerTag;
    GuiImageData * p1GBA;
    GuiImageData * p2GBA;
    GuiImageData * p1Link;
    GuiImageData * p2Link;
    GuiImageData * cube;
    GuiImageData * c1;
    GuiImageData * c1Link;

    GuiImage * playerTagImg;
    GuiImage * p1GBAImg;
    GuiImage * p2GBAImg;
    GuiImage * p1LinkImg;
    GuiImage * p2LinkImg;
    GuiImage * p1Screen;
    GuiImage * p2Screen;
    GuiImage * cubeImg;
    GuiImage * c1Img;
    GuiImage * c1LinkImg;

    GuiText * playerTagText;
    GuiText * serverTagText;
    GuiText * controllerInstructions;
    GuiText * resetLinkTxt;
    GuiText * gbaInstructions;
};

#endif