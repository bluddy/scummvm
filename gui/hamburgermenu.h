#ifndef GUI_HAMBURGERMENU_H
#define GUI_HAMBURGERMENU_H

#include "gui/dialog.h"

namespace GUI {

class HamburgerMenuDialog : public Dialog {
public:
	HamburgerMenuDialog();
	~HamburgerMenuDialog() override;

	void drawDialog(DrawLayer layerToDraw) override;
};

}

#endif
