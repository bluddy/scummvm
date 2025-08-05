#include "gui/hamburgermenu.h"
#include "gui/gui-manager.h"
#include "common/translation.h"
#include "common/events.h"

namespace GUI {

enum {
	kMainMenuCmd,
	kOptionsCmd,
	kToggleKeyboardCmd,
	kQuitCmd
};

HamburgerMenuDialog::HamburgerMenuDialog() : Dialog("HamburgerMenu") {
	_mainMenuButton = new ButtonWidget(this, "MainMenuButton", _("Main Menu"), _("Return to the main menu"), kMainMenuCmd);
	_optionsButton = new ButtonWidget(this, "OptionsButton", _("Options"), _("Change game options"), kOptionsCmd);
	_keyboardButton = new ButtonWidget(this, "VirtualKeyboardButton", _("Keyboard"), _("Show/hide the virtual keyboard"), kToggleKeyboardCmd);
	_quitButton = new ButtonWidget(this, "QuitButton", _("Quit"), _("Quit ScummVM"), kQuitCmd);
}

HamburgerMenuDialog::~HamburgerMenuDialog() {
}

void HamburgerMenuDialog::handleCommand(CommandSender *sender, uint32 cmd, uint32 data) {
	switch (cmd) {
		case kMainMenuCmd:
			// TODO: Implement main menu functionality
			break;
		case kOptionsCmd:
			{
				Common::Event event;
				event.type = Common::EVENT_GLOBAL_OPTIONS;
				g_system->getEventManager()->pushEvent(event);
			}
			break;
		case kToggleKeyboardCmd:
			// TODO: Implement keyboard toggle
			break;
		case kQuitCmd:
			{
				Common::Event event;
				event.type = Common::EVENT_QUIT;
				g_system->getEventManager()->pushEvent(event);
			}
			break;
		default:
			Dialog::handleCommand(sender, cmd, data);
	}
}

void HamburgerMenuDialog::drawDialog(DrawLayer layerToDraw) {
	Dialog::drawDialog(layerToDraw);
}

}
