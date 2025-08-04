#include "gui/hamburgermenu.h"
#include "gui/gui-manager.h"

namespace GUI {

HamburgerMenuDialog::HamburgerMenuDialog() : Dialog("HamburgerMenu") {
	_mainMenuButton = new ButtonWidget(this, "MainMenuButton", _("Main Menu"), _("Return to the main menu"), kMainMenuCmd);
	_optionsButton = new ButtonWidget(this, "OptionsButton", _("Options"), _("Change game options"), kOptionsCmd);
	_keyboardButton = new ButtonWidget(this, "VirtualKeyboardButton", _("Keyboard"), _("Show/hide the virtual keyboard"), kToggleKeyboardCmd);
	_quitButton = new ButtonWidget(this, "QuitButton", _("Quit"), _("Quit ScummVM"), kQuitCmd);
}

HamburgerMenuDialog::~HamburgerMenuDialog() {
}

void HamburgerMenuDialog::drawDialog(DrawLayer layerToDraw) {
	Dialog::drawDialog(layerToDraw);
}

}
