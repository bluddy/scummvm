#include "gui/widgets/popup.h"
#include "gui/widgets/list.h"
#include "gui/gui-manager.h"
#include "common/translation.h"

namespace GUI {

PopUpDialog::PopUpDialog(Widget *boss, const Common::String &name)
	: Dialog(name), _boss(boss) {
	_list = new ListWidget(this, "PopUpList");
	_list->setEditable(false);
	_list->enableDictionarySelect(true);
	_list->setNumberingMode(kListNumberingOff);

	_lineHeight = g_gui.getFont(ThemeEngine::kFontStyleNormal).getFontHeight();
}

PopUpDialog::~PopUpDialog() {
}

void PopUpDialog::appendEntry(const Common::U32String &entry) {
	_list->append(entry);
}

void PopUpDialog::clearEntries() {
	_list->setList(Common::U32StringArray());
}

int PopUpDialog::runModal() {
	_list->setSelected(0);
	_list->scrollTo(0);

	return Dialog::runModal();
}

void PopUpDialog::handleCommand(CommandSender *sender, uint32 cmd, uint32 data) {
	switch (cmd) {
	case kListItemActivatedCmd:
	case kListItemDoubleClickedCmd:
		setResult(_list->getSelected());
		close();
		break;
	default:
		Dialog::handleCommand(sender, cmd, data);
	}
}

PopUpWidget::PopUpWidget(GuiObject *boss, const Common::String &name, const Common::U32String &tooltip, uint32 cmd)
	: ButtonWidget(boss, name, Common::U32String(), tooltip, cmd), _selected(-1) {
}

void PopUpWidget::reflowLayout() {
	ButtonWidget::reflowLayout();

	_leftPadding = g_gui.xmlEval()->getVar("Globals.PopUpWidget.Padding.Left", 0);
	_rightPadding = g_gui.xmlEval()->getVar("Globals.PopUpWidget.Padding.Right", 0);
}

void PopUpWidget::appendEntry(const Common::U32String &entry, int id) {
	PopUpEntry e;
	e.label = entry;
	e.id = id;
	_entries.push_back(e);
}

void PopUpWidget::clearEntries() {
	_entries.clear();
}

void PopUpWidget::setSelected(int sel) {
	_selected = sel;
	markAsDirty();
}

int PopUpWidget::getSelected() const {
	return _selected;
}

void PopUpWidget::setSelectedId(int id) {
	for (uint i = 0; i < _entries.size(); i++) {
		if (_entries[i].id == id) {
			setSelected(i);
			return;
		}
	}
}

int PopUpWidget::getSelectedId() const {
	if (_selected >= 0 && _selected < (int)_entries.size()) {
		return _entries[_selected].id;
	}
	return -1;
}

int PopUpWidget::getEntryCount() const {
	return _entries.size();
}

void PopUpWidget::handleMouseUp(int x, int y, int button, int clickCount) {
	if (isEnabled() && _duringPress && x >= 0 && x < _w && y >= 0 && y < _h) {
		PopUpDialog popupDialog(this, "PopUpDialog");

		for (uint i = 0; i < _entries.size(); i++) {
			popupDialog.appendEntry(_entries[i].label);
		}

		int newSel = popupDialog.runModal();
		if (newSel != -1) {
			setSelected(newSel);
			sendCommand(_cmd, _entries[newSel].id);
		}
	}
	setUnpressedState();
	_duringPress = false;
}

void PopUpWidget::drawWidget() {
	Common::U32String label;
	if (_selected >= 0 && _selected < (int)_entries.size()) {
		label = _entries[_selected].label;
	}

	g_gui.theme()->drawPopUp(Common::Rect(_x, _y, _x + _w, _y + _h), _leftPadding, _rightPadding, label, _state, (g_gui.useRTL() && _useRTL));
}

}