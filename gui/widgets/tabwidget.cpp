#include "gui/widgets/tabwidget.h"
#include "gui/gui-manager.h"

namespace GUI {

TabWidget::TabWidget(GuiObject *boss, const Common::String &name)
	: Widget(boss, name), _selectedTab(0) {
	setFlags(WIDGET_ENABLED | WIDGET_CLEARBG);
	_type = kTabWidget;
}

TabWidget::~TabWidget() {
}

void TabWidget::addTab(const Common::U32String &label, Widget *widget) {
	Tab tab;
	tab.label = label;
	tab.widget = widget;
	_tabs.push_back(tab);
}

void TabWidget::drawWidget() {
	g_gui.theme()->drawTabWidget(Common::Rect(_x, _y, _x + _w, _y + _h), _tabs, _selectedTab);
}

}
