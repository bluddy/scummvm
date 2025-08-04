#ifndef GUI_TABBEDOPTIONSDIALOG_H
#define GUI_TABBEDOPTIONSDIALOG_H

#include "gui/dialog.h"

namespace GUI {

class TabbedOptionsDialog : public Dialog {
public:
	TabbedOptionsDialog();
	~TabbedOptionsDialog() override;

	void drawDialog(DrawLayer layerToDraw) override;
};

}

#endif
