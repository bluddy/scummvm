#ifndef GUI_TABBEDOPTIONSDIALOG_H
#define GUI_TABBEDOPTIONSDIALOG_H

#include "gui/dialog.h"
#include "gui/widgets/tabwidget.h"

#include "gui/options.h"

namespace GUI {

class TabbedOptionsDialog : public OptionsDialog {
	TabWidget *_tabWidget;

public:
	TabbedOptionsDialog();
	~TabbedOptionsDialog() override;

	void drawDialog(DrawLayer layerToDraw) override;
};

}

#endif
