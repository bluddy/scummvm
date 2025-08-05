#include "gui/tabbedoptionsdialog.h"
#include "gui/gui-manager.h"
#include "gui/options.h"

namespace GUI {

TabbedOptionsDialog::TabbedOptionsDialog() : Dialog("GlobalOptions") {
	_tabWidget = new TabWidget(this, "TabWidget");

	_tabWidget->addTab(_("Graphics"), new OptionsContainerWidget(this, "Graphics", "GlobalOptions_Graphics", "scummvm"));
	_tabWidget->addTab(_("Audio"), new OptionsContainerWidget(this, "Audio", "GlobalOptions_Audio", "scummvm"));
	_tabWidget->addTab(_("Volume"), new OptionsContainerWidget(this, "Volume", "GlobalOptions_Volume", "scummvm"));
	_tabWidget->addTab(_("Controls"), new OptionsContainerWidget(this, "Controls", "GlobalOptions_Control", "scummvm"));
	_tabWidget->addTab(_("MIDI"), new OptionsContainerWidget(this, "MIDI", "GlobalOptions_MIDI", "scummvm"));
	_tabWidget->addTab(_("Paths"), new OptionsContainerWidget(this, "Paths", "GlobalOptions_Paths", "scummvm"));
	_tabWidget->addTab(_("Updates"), new OptionsContainerWidget(this, "Updates", "GlobalOptions_Updates", "scummvm"));
	_tabWidget->addTab(_("Saves"), new OptionsContainerWidget(this, "Saves", "GlobalOptions_Saves", "scummvm"));
	_tabWidget->addTab(_("Cloud"), new OptionsContainerWidget(this, "Cloud", "GlobalOptions_Cloud", "scummvm"));
}

TabbedOptionsDialog::~TabbedOptionsDialog() {
}

void TabbedOptionsDialog::drawDialog(DrawLayer layerToDraw) {
	Dialog::drawDialog(layerToDraw);
}

}
