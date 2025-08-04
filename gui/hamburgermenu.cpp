#include "gui/hamburgermenu.h"

namespace GUI {

HamburgerMenuDialog::HamburgerMenuDialog() : Dialog("HamburgerMenu") {
}

HamburgerMenuDialog::~HamburgerMenuDialog() {
}

void HamburgerMenuDialog::drawDialog(DrawLayer layerToDraw) {
	Dialog::drawDialog(layerToDraw);
}

}
