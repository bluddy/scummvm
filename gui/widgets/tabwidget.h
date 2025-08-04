#ifndef GUI_WIDGETS_TABWIDGET_H
#define GUI_WIDGETS_TABWIDGET_H

#include "gui/widget.h"

namespace GUI {

class TabWidget : public Widget {
public:
	TabWidget(GuiObject *boss, const Common::String &name);
	~TabWidget() override;

	void addTab(const Common::U32String &label, Widget *widget);

protected:
	void drawWidget() override;

private:
	struct Tab {
		Common::U32String label;
		Widget *widget;
	};

	Common::Array<Tab> _tabs;
	int _selectedTab;
};

}

#endif
