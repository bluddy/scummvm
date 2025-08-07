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

#ifndef GUI_WIDGETS_POPUP_H
#define GUI_WIDGETS_POPUP_H

#include "gui/dialog.h"
#include "gui/widget.h"
#include "common/str.h"
#include "common/array.h"
#include "gui/widgets/list.h"

namespace GUI {

/**
 * Popup or dropdown widget which, when clicked, "pop up" a list of items and
 * lets the user pick on of them.
 *
 * Implementation wise, when the user selects an item, then the specified command
 * is broadcast, with data being equal to the tag value of the selected entry.
 */
struct PopUpEntry {
	Common::U32String label;
	int id;
};

class PopUpWidget : public ButtonWidget {
	int _leftPadding;
	int _rightPadding;

public:
	PopUpWidget(GuiObject *boss, const Common::String &name, const Common::U32String &tooltip = Common::U32String(), uint32 cmd = 0);

	void reflowLayout() override;

	void appendEntry(const Common::U32String &entry, int id = -1);
	void clearEntries();

	void setSelected(int sel);
	int getSelected() const;
	void setSelectedId(int id);
	int getSelectedId() const;
	int getEntryCount() const;

	void handleMouseUp(int x, int y, int button, int clickCount) override;

protected:
	void drawWidget() override;

private:
	Common::Array<PopUpEntry> _entries;
	int _selected;
};

/**
 * A small dialog showing a list of items and allowing the user to chose one of them
 *
 * Used by PopUpWidget and DropdownButtonWidget.
 */
class PopUpDialog : public Dialog {
protected:
	Widget		*_boss;
	int			_selection;
	int			_initialSelection;
	uint32		_openTime;

	int			_lineHeight;

	int			_lastRead;

	ListWidget *_list;

public:
	PopUpDialog(Widget *boss, const Common::String &name);

	void open() override;
	void reflowLayout() override;
	void drawDialog(DrawLayer layerToDraw) override;

	void handleMouseUp(int x, int y, int button, int clickCount) override;
	void handleMouseWheel(int x, int y, int direction) override;	// Scroll through entries with scroll wheel
	void handleMouseMoved(int x, int y, int button) override;	// Redraw selections depending on mouse position
	void handleMouseLeft(int button) override;
	void handleKeyDown(Common::KeyState state) override;	// Scroll through entries with arrow keys etc.

	void setPosition(int x, int y);
	void setPadding(int left, int right);
	void setLineHeight(int lineHeight);
	void setWidth(uint16 width);

	void appendEntry(const Common::U32String &entry);
	void clearEntries();
	void setSelection(int item);

protected:
	void drawMenuEntry(int entry, bool hilite);

	int findItem(int x, int y) const;
	bool isMouseDown();

	void moveUp();
	void moveDown();
	void read(const Common::U32String &str);
};

} // End of namespace GUI

#endif
