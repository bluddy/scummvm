#ifndef GUI_HAMBURGERMENU_H
#define GUI_HAMBURGERMENU_H

#include "gui/dialog.h"
#include "gui/widgets/button.h"

namespace GUI {

class HamburgerMenuDialog : public Dialog {
	ButtonWidget *_mainMenuButton;
	ButtonWidget *_optionsButton;
	ButtonWidget *_keyboardButton;
	ButtonWidget *_quitButton;

public:
	HamburgerMenuDialog();
	~HamburgerMenuDialog() override;

	void drawDialog(DrawLayer layerToDraw) override;
};

}

#endif
