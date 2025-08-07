#include "gui/tabbedoptionsdialog.h"
#include "gui/gui-manager.h"
#include "common/translation.h"

namespace GUI {

class OptionsTab : public OptionsContainerWidget {
public:
	OptionsTab(GuiObject *owner, const Common::String &name, const Common::String &tabName, const Common::String &domain)
		: OptionsContainerWidget(owner, name, tabName, domain) {}

	void load() override {}
	bool save() override { return false; }
};

TabbedOptionsDialog::TabbedOptionsDialog() : OptionsDialog("scummvm", "GlobalOptions") {
	_tabWidget = new TabWidget(this, "TabWidget");

	OptionsTab *graphicsTab = new OptionsTab(this, "Graphics", "GlobalOptions_Graphics", "scummvm");
	addGraphicControls(graphicsTab, "GlobalOptions_Graphics");
	_tabWidget->addTab(_("Graphics"), graphicsTab);

	OptionsTab *audioTab = new OptionsTab(this, "Audio", "GlobalOptions_Audio", "scummvm");
	addAudioControls(audioTab, "GlobalOptions_Audio");
	_tabWidget->addTab(_("Audio"), audioTab);

	OptionsTab *volumeTab = new OptionsTab(this, "Volume", "GlobalOptions_Volume", "scummvm");
	addVolumeControls(volumeTab, "GlobalOptions_Volume");
	_tabWidget->addTab(_("Volume"), volumeTab);

	OptionsTab *controlsTab = new OptionsTab(this, "Controls", "GlobalOptions_Control", "scummvm");
	addControlControls(controlsTab, "GlobalOptions_Control");
	_tabWidget->addTab(_("Controls"), controlsTab);

	OptionsTab *midiTab = new OptionsTab(this, "MIDI", "GlobalOptions_MIDI", "scummvm");
	addMIDIControls(midiTab, "GlobalOptions_MIDI");
	_tabWidget->addTab(_("MIDI"), midiTab);

	OptionsTab *pathsTab = new OptionsTab(this, "Paths", "GlobalOptions_Paths", "scummvm");
	//addPathsControls(pathsTab, "GlobalOptions_Paths");
	_tabWidget->addTab(_("Paths"), pathsTab);

	OptionsTab *updatesTab = new OptionsTab(this, "Updates", "GlobalOptions_Updates", "scummvm");
	//addMiscControls(updatesTab, "GlobalOptions_Updates");
	_tabWidget->addTab(_("Updates"), updatesTab);

	OptionsTab *savesTab = new OptionsTab(this, "Saves", "GlobalOptions_Saves", "scummvm");
	//addMiscControls(savesTab, "GlobalOptions_Saves");
	_tabWidget->addTab(_("Saves"), savesTab);

	OptionsTab *cloudTab = new OptionsTab(this, "Cloud", "GlobalOptions_Cloud", "scummvm");
	//addCloudControls(cloudTab, "GlobalOptions_Cloud");
	_tabWidget->addTab(_("Cloud"), cloudTab);
}

TabbedOptionsDialog::~TabbedOptionsDialog() {
}

void TabbedOptionsDialog::drawDialog(DrawLayer layerToDraw) {
	Dialog::drawDialog(layerToDraw);
}

}