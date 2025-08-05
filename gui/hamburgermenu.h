#ifndef GUI_HAMBURGERMENU_H
#define GUI_HAMBURGERMENU_H

#include "gui/dialog.h"
#include "gui/widget.h"

namespace GUI {

class HamburgerMenuDialog : public Dialog {
	ButtonWidget *_mainMenuButton;
	ButtonWidget *_optionsButton;
	ButtonWidget *_keyboardButton;
	ButtonWidget *_quitButton;

public:
	HamburgerMenuDialog();
	~HamburgerMenuDialog() override;

	void handleCommand(CommandSender *sender, uint32 cmd, uint32 data) override;
	void drawDialog(DrawLayer layerToDraw) override;
};

}

#endif
