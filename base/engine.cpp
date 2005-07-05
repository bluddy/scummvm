/* ScummVM - Scumm Interpreter
 * Copyright (C) 2002-2005 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 */

#include "common/stdafx.h"
#if defined(WIN32)
#include <malloc.h>
#endif
#include "base/engine.h"
#include "base/gameDetector.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "common/timer.h"
#include "common/savefile.h"
#include "common/system.h"
#include "sound/mixer.h"
#include "gui/message.h"

/* FIXME - BIG HACK for MidiEmu */
Engine *g_engine = 0;

Engine::Engine(OSystem *syst)
	: _system(syst), _gameDataPath(ConfMan.get("path")) {
	g_engine = this;
	_mixer = GameDetector::createMixer();

	_timer = Common::g_timer;

	// Add default file directory
	Common::File::addDefaultDirectory(_gameDataPath);

	_saveFileMan = _system->getSavefileManager();
}

Engine::~Engine() {
	Common::File::resetDefaultDirectories();

	delete _mixer;
	delete _saveFileMan;

	g_engine = NULL;
}

void Engine::initCommonGFX(GameDetector &detector) {
	const bool useDefaultGraphicsMode =
		!ConfMan.hasKey("gfx_mode", detector._targetName) ||
		!scumm_stricmp(ConfMan.get("gfx_mode", detector._targetName).c_str(), "normal") ||
		!scumm_stricmp(ConfMan.get("gfx_mode", detector._targetName).c_str(), "default");

	// See if the game should default to 1x scaler
	if (useDefaultGraphicsMode && (detector._game.features & GF_DEFAULT_TO_1X_SCALER)) {
		// FIXME: As a hack, we use "1x" here. Would be nicer to use
		// getDefaultGraphicsMode() instead, but right now, we do not specify
		// whether that is a 1x scaler or not...
		_system->setGraphicsMode("1x");
	} else {
		// Override global scaler with any game-specific define
		if (ConfMan.hasKey("gfx_mode")) {
			_system->setGraphicsMode(ConfMan.get("gfx_mode").c_str());
		}
	}
	
	// (De)activate aspect-ratio correction as determined by the config settings
	if (ConfMan.hasKey("aspect_ratio", detector._targetName))
		_system->setFeatureState(OSystem::kFeatureAspectRatioCorrection, ConfMan.getBool("aspect_ratio"));
		
	// (De)activate fullscreen mode as determined by the config settings 
	if (ConfMan.hasKey("fullscreen", detector._targetName))
		_system->setFeatureState(OSystem::kFeatureFullscreenMode, ConfMan.getBool("fullscreen"));
}

void Engine::checkCD() {
#if defined (WIN32) && !defined(_WIN32_WCE) 
	// It is a known bug under Windows that games that play CD audio cause
	// ScummVM to crash if the data files are read from the same CD. Check
	// if this appears to be the case and issue a warning.

	// If we can find a compressed audio track, then it should be ok even
	// if it's running from CD.

#ifdef USE_VORBIS
	if (Common::File::exists("track1.ogg"))
		return;
#endif
#ifdef USE_FLAC
	if (Common::File::exists("track1.fla") || Common::File::exists("track1.flac"))
		return;
#endif
#ifdef USE_MAD
	if (Common::File::exists("track1.mp3"))
		return;
#endif

	char buffer[MAXPATHLEN];
	int i;

	if (strlen(getGameDataPath()) == 0) {
		// That's it! I give up!
		if (getcwd(buffer, MAXPATHLEN) == NULL)
			return;
	} else
		strncpy(buffer, getGameDataPath(), MAXPATHLEN);

	for (i = 0; i < MAXPATHLEN - 1; i++) {
		if (buffer[i] == '\\')
			break;
	}

	buffer[i + 1] = 0;

	if (GetDriveType(buffer) == DRIVE_CDROM) {
		GUI::MessageDialog dialog(
			"You appear to be playing this game directly\n"
			"from the CD. This is known to cause problems,\n"
			"and it's therefore recommended that you copy\n"
			"the data files to your hard disk instead.\n"
			"See the README file for details.", "OK");
		dialog.runModal();
	}
#endif
}

const char *Engine::getGameDataPath() const {
	return _gameDataPath.c_str();
}

void NORETURN CDECL error(const char *s, ...) {
	char buf_input[STRINGBUFLEN];
	char buf_output[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf_input, STRINGBUFLEN, s, va);
	va_end(va);

	if (g_engine) {
		g_engine->errorString(buf_input, buf_output);
	} else {
		strcpy(buf_output, buf_input);
	}

#ifdef __GP32__ //ph0x FIXME?
	printf("ERROR: %s\n", buf_output);
#else
#ifndef _WIN32_WCE
	fprintf(stderr, "%s!\n", buf_output);
#endif
#endif

#if defined( USE_WINDBG )
#if defined( _WIN32_WCE )
	TCHAR buf_output_unicode[1024];
	MultiByteToWideChar(CP_ACP, 0, buf_output, strlen(buf_output) + 1, buf_output_unicode, sizeof(buf_output_unicode));
	OutputDebugString(buf_output_unicode);
#else
	OutputDebugString(buf_output);
#endif
#endif

#if defined ( _WIN32_WCE )
	drawError(buf_output);
#endif

#ifdef __PALM_OS__
	PalmFatalError(buf_output);
#endif

	// Finally exit. quit() will terminate the program if g_system iss present
	if (g_system)
		g_system->quit();

	exit(1);
}

void CDECL warning(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
#ifdef __SYMBIAN32__
	vsprintf(buf, s, va);
#else
	vsnprintf(buf, STRINGBUFLEN, s, va);
#endif
	va_end(va);

#ifdef __GP32__ //ph0x FIXME: implement fprint?
	printf("WARNING: %s\n", buf);
#else
#if !defined (_WIN32_WCE) && !defined (__SYMBIAN32__)
	fprintf(stderr, "WARNING: %s!\n", buf);
#endif
#endif
#if defined( USE_WINDBG )
	strcat(buf, "\n");
#if defined( _WIN32_WCE )
	TCHAR buf_unicode[1024];
	MultiByteToWideChar(CP_ACP, 0, buf, strlen(buf) + 1, buf_unicode, sizeof(buf_unicode));
	OutputDebugString(buf_unicode);
#else
	OutputDebugString(buf);
#endif
#endif
}

void checkHeap() {
#if defined(_MSC_VER)
	if (_heapchk() != _HEAPOK) {
		error("Heap is invalid!");
	}
#endif
}
