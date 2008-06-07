/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "drascula/drascula.h"

namespace Drascula {

void DrasculaEngine::selectVerbFromBar() {
	for (int n = 0; n < 7; n++) {
		if (mouseX > verbBarX[n] && mouseX < verbBarX[n + 1] && n > 0) {
			selectVerb(n);
			return;
		}
	}

	// no verb selected
	withoutVerb();
}

void DrasculaEngine::selectVerb(int verbo) {
	int c = (menuScreen == 1) ? 0 : 171;

	if (currentChapter == 5) {
		if (takeObject == 1 && pickedObject != 16)
			addObject(pickedObject);
	} else {
		if (takeObject == 1)
			addObject(pickedObject);
	}

	copyBackground(OBJWIDTH * verbo, c, 0, 0, OBJWIDTH, OBJHEIGHT, backSurface, drawSurface3);

	takeObject = 1;
	pickedObject = verbo;
}

bool DrasculaEngine::confirmExit() {
	byte key;

	color_abc(kColorRed);
	updateRoom();
	centerText(_textsys[_lang][1], 160, 87);
	updateScreen();

	delay(100);
	for (;;) {
		key = getScan();
		if (key != 0)
			break;
	}

	if (key == Common::KEYCODE_ESCAPE) {
		stopMusic();
		return false;
	}

	return true;
}

void DrasculaEngine::showMenu() {
	int h, n, x;
	char textIcon[13];

	x = whichObject();
	strcpy(textIcon, iconName[x]);

	for (n = 1; n < 43; n++) {
		h = inventoryObjects[n];

		if (h != 0) {
			if (currentChapter == 6)
				copyBackground(x_pol[n], y_pol[n], itemLocations[n].x, itemLocations[n].y,
						OBJWIDTH, OBJHEIGHT, tableSurface, screenSurface);
			else
				copyBackground(x_pol[n], y_pol[n], itemLocations[n].x, itemLocations[n].y,
						OBJWIDTH, OBJHEIGHT, frontSurface, screenSurface);
		}
		copyRect(x1d_menu[h], y1d_menu[h], itemLocations[n].x, itemLocations[n].y,
				OBJWIDTH, OBJHEIGHT, backSurface, screenSurface);
	}

	if (x < 7)
		print_abc(textIcon, itemLocations[x].x - 2, itemLocations[x].y - 7);
}

void DrasculaEngine::clearMenu() {
	int n, verbActivated = 1;

	for (n = 0; n < 7; n++) {
		if (mouseX > verbBarX[n] && mouseX < verbBarX[n + 1])
			verbActivated = 0;
		copyRect(OBJWIDTH * n, OBJHEIGHT * verbActivated, verbBarX[n], 2,
						OBJWIDTH, OBJHEIGHT, backSurface, screenSurface);
		verbActivated = 1;
	}
}

void DrasculaEngine::enterName() {
	Common::KeyCode key;
	int v = 0, h = 0;
	char select2[23];
	strcpy(select2, "                      ");
	for (;;) {
		select2[v] = '-';
		copyBackground(115, 14, 115, 14, 176, 9, drawSurface1, screenSurface);
		print_abc(select2, 117, 15);
		updateScreen();
		key = getScan();
		delay(70);
		if (key != 0) {
			if (key >= 0 && key <= 0xFF && isalpha(key))
				select2[v] = tolower(key);
			else if ((key == Common::KEYCODE_LCTRL) || (key == Common::KEYCODE_RCTRL))
				select2[v] = '\164';
			else if (key >= Common::KEYCODE_0 && key <= Common::KEYCODE_9)
				select2[v] = key;
			else if (key == Common::KEYCODE_SPACE)
				select2[v] = '\167';
			else if (key == Common::KEYCODE_ESCAPE)
				break;
			else if (key == Common::KEYCODE_RETURN) {
				select2[v] = '\0';
				h = 1;
				break;
			} else if (key == Common::KEYCODE_BACKSPACE)
				select2[v] = '\0';
			else
				v--;

			if (key == Common::KEYCODE_BACKSPACE)
				v--;
			else
				v++;
		}
		if (v == 22)
			v = 21;
		else if (v == -1)
			v = 0;
	}
	if (h == 1) {
		strcpy(select, select2);
		selectionMade = 1;
	}
}

bool DrasculaEngine::checkMenuFlags() {
	for (int n = 0; n < 43; n++) {
		if (whichObject() == n) {
			if (inventoryObjects[n] != 0 && checkAction(inventoryObjects[n]))
				return true;
		}
	}

	return false;
}

void DrasculaEngine::showMap() {
	hasName = 0;

	for (int l = 0; l < numRoomObjs; l++) {
		if (mouseX > x1[l] && mouseY > y1[l]
				&& mouseX < x2[l] && mouseY < y2[l]
				&& visible[l] == 1) {
			strcpy(textName, objName[l]);
			hasName = 1;
		}
	}
}

void DrasculaEngine::grr() {
	int length = 30;

	color_abc(kColorDarkGreen);

	playFile("s10.als");

	updateRoom();
	copyBackground(253, 110, 150, 65, 20, 30, drawSurface3, screenSurface);

	if (withVoices == 0)
		centerText("groaaarrrrgghhhh!", 153, 65);

	updateScreen();

	while (!isTalkFinished(&length));

	updateRoom();
	updateScreen();
}

} // End of namespace Drascula
