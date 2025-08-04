/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/system.h"
#include "gui/gui-manager.h"
#include "gui/widgets/popup.h"

#include "gui/ThemeEval.h"

namespace GUI {

//
// PopUpDialog
//

PopUpDialog::PopUpDialog(Widget *boss, const Common::String &name)
	: Dialog(name),
	  _boss(boss),
	  _selection(-1),
	  _initialSelection(-1),
	  _openTime(0),
	  _lineHeight(kLineHeight),
	  _lastRead(-1) {
	_backgroundType = ThemeEngine::kDialogBackgroundNone;

	_list = new ListWidget(this, 0, 0, _w, _h);
	_list->setFlags(WIDGET_ENABLED | WIDGET_TRACK_MOUSE | WIDGET_CLEARBG | WIDGET_WANT_TICKLE);
}

void PopUpDialog::open() {
	// Time the popup was opened
	_openTime = g_system->getMillis();

	_initialSelection = _selection;

	// Calculate real popup dimensions
	_h = _list->numItems() * _lineHeight + 2;
	_w = 0;

	for (uint i = 0; i < _list->numItems(); i++) {
		int width = g_gui.getStringWidth(_list->getEntry(i));

		if (width > _w)
			_w = width;
	}

	Common::Rect safeArea = g_system->getSafeOverlayArea();

	_w = safeArea.width() - 1;
	_h = safeArea.height() - 1;
	_x = safeArea.left;
	_y = safeArea.top;

	_list->setSize(_w, _h);

	_lastRead = -1;

	Dialog::open();
}

void PopUpDialog::reflowLayout() {
}

void PopUpDialog::drawDialog(DrawLayer layerToDraw) {
	Dialog::drawDialog(layerToDraw);

	_list->draw();
}

void PopUpDialog::handleMouseUp(int x, int y, int button, int clickCount) {
	int item = _list->findItem(x, y);

	if (item >= 0 && _list->getEntry(item).size() == 0) {
		item = -1;
	}

	setResult(item);
	close();
}

void PopUpDialog::handleMouseWheel(int x, int y, int direction) {
	_list->handleMouseWheel(x, y, direction);
}

void PopUpDialog::handleMouseMoved(int x, int y, int button) {
	_list->handleMouseMoved(x, y, button);

	int item = _list->getSelected();

	if (item == -1 && !isMouseDown()) {
		setSelection(_initialSelection);
		return;
	}

	setSelection(item);
	if (_lastRead != item && _list->numItems() > 0 && item != -1) {
		read(_list->getEntry(item));
		_lastRead = item;
	}
}

void PopUpDialog::handleMouseLeft(int button) {
	_lastRead = -1;
}

void PopUpDialog::read(const Common::U32String &str) {
	if (ConfMan.hasKey("tts_enabled", "scummvm") &&
			ConfMan.getBool("tts_enabled", "scummvm")) {
		Common::TextToSpeechManager *ttsMan = g_system->getTextToSpeechManager();
		if (ttsMan != nullptr)
			ttsMan->say(str);
	}
}

void PopUpDialog::handleKeyDown(Common::KeyState state) {
	if (state.keycode == Common::KEYCODE_ESCAPE) {
		// Don't change the previous selection
		setResult(-1);
		close();
		return;
	}

	if (isMouseDown())
		return;

	_list->handleKeyDown(state);

	setSelection(_list->getSelected());
}

void PopUpDialog::setPosition(int x, int y) {
	_x = x;
	_y = y;
}

void PopUpDialog::setPadding(int left, int right) {
}

void PopUpDialog::setLineHeight(int lineHeight) {
	_lineHeight = lineHeight;
}

void PopUpDialog::setWidth(uint16 width) {
	_w = width;
}

void PopUpDialog::appendEntry(const Common::U32String &entry) {
	_list->appendEntry(entry);
}

void PopUpDialog::clearEntries() {
	_list->clearEntries();
}

int PopUpDialog::findItem(int x, int y) const {
	return _list->findItem(x, y);
}

void PopUpDialog::setSelection(int item) {
	_selection = item;
	_list->setSelected(item);
}

bool PopUpDialog::isMouseDown() {
	// TODO/FIXME - need a way to determine whether any mouse buttons are pressed or not.
	// Sure, we could just count mouse button up/down events, but that is cumbersome and
	// error prone. Would be much nicer to add an API to OSystem for this...

	return false;
}

void PopUpDialog::moveUp() {
	_list->moveUp();
	setSelection(_list->getSelected());
}

void PopUpDialog::moveDown() {
	_list->moveDown();
	setSelection(_list->getSelected());
}

void PopUpDialog::drawMenuEntry(int entry, bool hilite) {
}


#pragma mark -

//
// PopUpWidget
//

PopUpWidget::PopUpWidget(GuiObject *boss, const Common::String &name, const Common::U32String &tooltip, uint32 cmd)
	: Widget(boss, name, tooltip), CommandSender(boss) {
	setFlags(WIDGET_ENABLED | WIDGET_CLEARBG | WIDGET_RETAIN_FOCUS | WIDGET_IGNORE_DRAG);
	_type = kPopUpWidget;
	_cmd = cmd;

	_selectedItem = -1;
}

PopUpWidget::PopUpWidget(GuiObject *boss, int x, int y, int w, int h, const Common::U32String &tooltip, uint32 cmd)
	: Widget(boss, x, y, w, h, tooltip), CommandSender(boss) {
	setFlags(WIDGET_ENABLED | WIDGET_CLEARBG | WIDGET_RETAIN_FOCUS | WIDGET_IGNORE_DRAG);
	_type = kPopUpWidget;
	_cmd = cmd;

	_selectedItem = -1;
}

void PopUpWidget::handleMouseDown(int x, int y, int button, int clickCount) {
	if (isEnabled()) {
		PopUpDialog popupDialog(this, "DropdownDialog");

		for (uint i = 0; i < _entries.size(); i++) {
			popupDialog.appendEntry(_entries[i].name);
		}
		popupDialog.setSelection(_selectedItem);

		int newSel = popupDialog.runModal();
		if (newSel != -1 && _selectedItem != newSel) {
			_selectedItem = newSel;
			sendCommand(_cmd, _entries[_selectedItem].tag);
			markAsDirty();
		}
	}
}

void PopUpWidget::handleMouseWheel(int x, int y, int direction) {
	if (isEnabled()) {
		int newSelection = _selectedItem + direction;

		// Skip separator entries
		while ((newSelection >= 0) && (newSelection < (int)_entries.size()) &&
		       _entries[newSelection].name.empty()) {
			newSelection += direction;
		}

		// Just update the selected item when we're in range
		if ((newSelection >= 0) && (newSelection < (int)_entries.size()) &&
			(newSelection != _selectedItem)) {
			_selectedItem = newSelection;
			sendCommand(_cmd, _entries[_selectedItem].tag);
			markAsDirty();
		}
	}
}

void PopUpWidget::reflowLayout() {
	_leftPadding = g_gui.xmlEval()->getVar("Globals.PopUpWidget.Padding.Left", 0);
	_rightPadding = g_gui.xmlEval()->getVar("Globals.PopUpWidget.Padding.Right", 0);

	Widget::reflowLayout();
}

void PopUpWidget::appendEntry(const Common::U32String &entry, uint32 tag) {
	Entry e;
	e.name = entry;
	e.tag = tag;
	_entries.push_back(e);
}

void PopUpWidget::appendEntry(const Common::String &entry, uint32 tag) {
	appendEntry(Common::U32String(entry), tag);
}

void PopUpWidget::clearEntries() {
	_entries.clear();
	_selectedItem = -1;
}

void PopUpWidget::setSelected(int item) {
	if (item != _selectedItem) {
		if (item >= 0 && item < (int)_entries.size()) {
			_selectedItem = item;
		} else {
			_selectedItem = -1;
		}
	}
}

void PopUpWidget::setSelectedTag(uint32 tag) {
	uint item;
	for (item = 0; item < _entries.size(); ++item) {
		if (_entries[item].tag == tag) {
			setSelected(item);
			return;
		}
	}
}

void PopUpWidget::drawWidget() {
	Common::U32String sel;
	if (_selectedItem >= 0)
		sel = _entries[_selectedItem].name;

	int pad = _leftPadding;

	if (g_gui.useRTL() && _useRTL)
		pad = _rightPadding;

	g_gui.theme()->drawPopUpWidget(Common::Rect(_x, _y, _x + _w, _y + _h), sel, pad, _state, (g_gui.useRTL() && _useRTL));
}

} // End of namespace GUI
