/* ScummVM - Scumm Interpreter
 * Copyright (C) 2004-2006 The ScummVM project
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

#ifndef KYRA3_H
#define KYRA3_H

#include "kyra/kyra.h"

namespace Kyra {

// maybe subclass KyraEngine_v2 later
class WSAMovieV2;

class KyraEngine_v3 : public KyraEngine {
public:
	KyraEngine_v3(OSystem *system);
	~KyraEngine_v3();
	
	Movie *createWSAMovie();
	
	SoundDigital *soundDigital() { return _soundDigital; }

	int setupGameFlags();
	
	int go();

	void playVQA(const char *name);
private:
	int init();

	void preinit();
	void realInit();

	SoundDigital *_soundDigital;
	
	int _lang;
	
	// sound specific
private:
	void playMenuAudioFile();
	
	int _musicSoundChannel;
	const char *_menuAudioFile;
	
	static const char *_soundList[];
	static const int _soundListSize;

	int _curMusicTrack;

	void playMusicTrack(int track, int force);
	void stopMusicTrack();

	int musicUpdate(int forceRestart);

	// gui/menu specific
private:
	static const char *_mainMenuStrings[];
	int handleMainMenu(WSAMovieV2 *logo);
	void drawMainMenu(const char * const *strings, int select);
	void drawMainBox(int x, int y, int w, int h, int fill);
	
	void gui_printString(const char *string, int x, int y, int col1, int col2, int flags, ...);

	// unknown
private:	
	uint8 *_unkPage1;
	uint8 *_unkPage2;
	
	// interface?
	uint8 *_interfaceCPS1;
	uint8 *_interfaceCPS2;
};

} // end of namespace Kyra

#endif
