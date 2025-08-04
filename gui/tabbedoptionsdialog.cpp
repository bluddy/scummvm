#include "gui/tabbedoptionsdialog.h"

namespace GUI {

TabbedOptionsDialog::TabbedOptionsDialog() : Dialog("GlobalOptions") {
}

TabbedOptionsDialog::~TabbedOptionsDialog() {
}

void TabbedOptionsDialog::drawDialog(DrawLayer layerToDraw) {
	Dialog::drawDialog(layerToDraw);
}

}
