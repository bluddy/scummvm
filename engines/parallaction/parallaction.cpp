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

#include "common/config-manager.h"
#include "common/events.h"
#include "common/file.h"
#include "common/util.h"

#include "sound/mididrv.h"
#include "sound/mixer.h"

#include "parallaction/parallaction.h"
#include "parallaction/debug.h"
#include "parallaction/sound.h"



namespace Parallaction {

// FIXME: remove this
Parallaction *_vm = NULL;

// public stuff

uint16	_mouseButtons = 0;


char		_saveData1[30] = { '\0' };
uint16		_language = 0;
char		_slideText[2][40];
uint32		_engineFlags = 0;

uint16		_score = 1;
char		_password[8];

Command *	_forwardedCommands[20] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

char		_forwardedAnimationNames[20][20];
uint16		_numForwards = 0;

uint32		_commandFlags = 0;
uint16		_introSarcData3 = 200;
uint16		_introSarcData2 = 1;

// private stuff

static Job	   *_jDrawInventory = NULL;
static Job	   *_jRunScripts = NULL;


Parallaction::Parallaction(OSystem *syst) :
	Engine(syst), _char(this) {

	// FIXME: Fingolfin asks: why is there a FIXME here? Please either clarify what
	// needs fixing, or remove it!
	// FIXME
	_vm = this;

	_mouseHidden = false;

	Common::File::addDefaultDirectory( _gameDataPath );

	Common::addSpecialDebugLevel(kDebugDialogue, "dialogue", "Dialogues debug level");
	Common::addSpecialDebugLevel(kDebugParser, "parser", "Parser debug level");
	Common::addSpecialDebugLevel(kDebugDisk, "disk", "Disk debug level");
	Common::addSpecialDebugLevel(kDebugWalk, "walk", "Walk debug level");
	Common::addSpecialDebugLevel(kDebugGraphics, "gfx", "Gfx debug level");
	Common::addSpecialDebugLevel(kDebugExec, "exec", "Execution debug level");
	Common::addSpecialDebugLevel(kDebugInput, "input", "Input debug level");
	Common::addSpecialDebugLevel(kDebugAudio, "audio", "Audio debug level");
	Common::addSpecialDebugLevel(kDebugMenu, "menu", "Menu debug level");
	Common::addSpecialDebugLevel(kDebugInventory, "inventory", "Inventory debug level");

	syst->getEventManager()->registerRandomSource(_rnd, "parallaction");
}


Parallaction::~Parallaction() {
	delete _debugger;

	freeBackground();
	delete _backgroundInfo;

	delete _globalTable;

	delete _callableNames;
	delete _localFlagNames;
	delete _zoneTypeNames;
	delete _zoneFlagNames;

	_animations.remove(&_char._ani);

	freeLocation();

	freeCharacter();
	destroyInventory();

	delete _gfx;
	delete _soundMan;
	delete _disk;
}


int Parallaction::init() {

	_engineFlags = 0;
	_objectsNames = NULL;
	_globalTable = NULL;
	_hasLocationSound = false;
	_transCurrentHoverItem = 0;
	_actionAfterWalk = false;  // actived when the character needs to move before taking an action
	_activeItem._index = 0;
	_activeItem._id = 0;
	_procCurrentHoverItem = -1;
	_baseTime = 0;
	_numLocations = 0;
	_location._startPosition.x = -1000;
	_location._startPosition.y = -1000;
	_location._startFrame = 0;
	_location._comment = NULL;
	_location._endComment = NULL;

	_backgroundInfo = 0;
	_pathBuffer = 0;
	_activeZone = 0;

	_screenSize = _screenWidth * _screenHeight;

	_backgroundInfo = new BackgroundInfo;

	strcpy(_characterName1, "null");
//	strcpy(_char._name, "dough");
	_char.setName("dough");

	memset(_locationNames, 0, NUM_LOCATIONS * 32);

	initInventory();	// needs to be pushed into subclass

	_jDrawLabel = NULL;
	_jEraseLabel = NULL;
	_hoverZone = NULL;

	_animations.push_front(&_char._ani);
	_gfx = new Gfx(this);

	_debugger = new Debugger(this);

	return 0;
}







// FIXME: the engine has 3 event loops. The following routine hosts the main one,
// and it's called from 8 different places in the code. There exist 2 more specialised
// loops which could possibly be merged into this one with some effort in changing
// caller code, i.e. adding condition checks.
//
uint16 Parallaction::updateInput() {

	Common::Event e;
	uint16 KeyDown = 0;

	_mouseButtons = kMouseNone;

	Common::EventManager *eventMan = _system->getEventManager();
	while (eventMan->pollEvent(e)) {

		switch (e.type) {
		case Common::EVENT_KEYDOWN:
			if (e.kbd.flags == Common::KBD_CTRL && e.kbd.keycode == 'd')
				_debugger->attach();
			if (getFeatures() & GF_DEMO) break;
			if (e.kbd.keycode == Common::KEYCODE_l) KeyDown = kEvLoadGame;
			if (e.kbd.keycode == Common::KEYCODE_s) KeyDown = kEvSaveGame;
			break;

		case Common::EVENT_LBUTTONDOWN:
			_mouseButtons = kMouseLeftDown;
			break;

		case Common::EVENT_LBUTTONUP:
			_mouseButtons = kMouseLeftUp;
			break;

		case Common::EVENT_RBUTTONDOWN:
			_mouseButtons = kMouseRightDown;
			break;

		case Common::EVENT_RBUTTONUP:
			_mouseButtons = kMouseRightUp;
			break;

		case Common::EVENT_MOUSEMOVE:
			_mousePos = e.mouse;
			break;

		case Common::EVENT_QUIT:
			_engineFlags |= kEngineQuit;
			g_system->quit();
			break;

		default:
			break;

		}

	}

	if (_debugger->isAttached())
		_debugger->onFrame();

	return KeyDown;

}

// FIXME: see comment for updateInput()
void waitUntilLeftClick() {

	Common::Event e;

	Common::EventManager *eventMan = g_system->getEventManager();
	for (;;) {
		eventMan->pollEvent(e);

		if (e.type == Common::EVENT_LBUTTONUP)
			break;

		if (e.type == Common::EVENT_QUIT) {
			_engineFlags |= kEngineQuit;
			break;
		}

		_vm->_gfx->updateScreen();
		g_system->delayMillis(30);
	}


	return;
}


void Parallaction::runGame() {

	addJob(kJobEraseAnimations, (void*)1, kPriority20);
	_jRunScripts = addJob(kJobRunScripts, 0, kPriority15);
	addJob(kJobDisplayAnimations, 0, kPriority3);

	_gfx->copyScreen(Gfx::kBitBack, Gfx::kBit2);

	if (_location._commands.size() > 0)
		runCommands(_location._commands);

	_gfx->copyScreen(Gfx::kBitBack, Gfx::kBitFront);

	if (_location._comment)
		doLocationEnterTransition();

	if (_hasLocationSound)
		_soundMan->playSfx(_locationSound, 0, true);

	_vm->setArrowCursor();

	if (_location._aCommands.size() > 0)
		runCommands(_location._aCommands);

	while ((_engineFlags & kEngineQuit) == 0) {
		_keyDown = updateInput();

		debugC(3, kDebugInput, "runGame: input flags (%i, %i, %i, %i)",
			!_mouseHidden,
			(_engineFlags & kEngineBlockInput) == 0,
			(_engineFlags & kEngineWalking) == 0,
			(_engineFlags & kEngineChangeLocation) == 0
		);

		// WORKAROUND: the engine doesn't check for displayed labels before performing a location
		// switch, thus crashing whenever a jobDisplayLabel/jEraseLabel pair is left into the
		// queue after the character enters a door.
		// Skipping input processing when kEngineChangeLocation is set solves the issue. It's
		// noteworthy that the programmers added this very check in Big Red Adventure's engine,
		// so it should be ok here in Nippon Safes too.
		if ((!_mouseHidden) && ((_engineFlags & kEngineBlockInput) == 0) && ((_engineFlags & kEngineWalking) == 0) && ((_engineFlags & kEngineChangeLocation) == 0)) {
			InputData *v8 = translateInput();
			if (v8) processInput(v8);
		}

		if (_activeZone) {
			Zone *z = _activeZone;	// speak Zone or sound
			_activeZone = NULL;
			if (runZone( z ) == 0)
				runCommands( z->_commands, z );
		}

		if (_engineFlags & kEngineChangeLocation) {
			_engineFlags &= ~kEngineChangeLocation;
			changeLocation(_location._name);
			continue;
		}

		g_system->delayMillis(30);

		runJobs();

		if ((_engineFlags & kEnginePauseJobs) == 0 || (_engineFlags & kEngineInventory)) {
			_gfx->swapBuffers();
			_gfx->animatePalette();
		}

	}

	return;
}

void Parallaction::showLabel(Label &label) {
	label.resetPosition();
	_jDrawLabel = addJob(kJobDisplayLabel, (void*)&label, kPriority0);
	_jEraseLabel = addJob(kJobEraseLabel, (void*)&label, kPriority20);
}

void Parallaction::hideLabel(uint priority) {

	if (!_jDrawLabel)
		return;

	removeJob(_jDrawLabel);
	_jDrawLabel = 0;

	if (priority == kPriority99) {
		// remove job immediately
		removeJob(_jEraseLabel);
		_jEraseLabel = 0;
	} else {
		// schedule job for deletion
		addJob(kJobWaitRemoveJob, _jEraseLabel, priority);
	}

}


void Parallaction::processInput(InputData *data) {
	Zone *z;

	switch (data->_event) {
	case kEvEnterZone:
		debugC(2, kDebugInput, "processInput: kEvEnterZone");
		showLabel(*data->_label);
		break;

	case kEvExitZone:
		debugC(2, kDebugInput, "processInput: kEvExitZone");
		hideLabel(kPriority15);
		break;

	case kEvAction:
		debugC(2, kDebugInput, "processInput: kEvAction");
		_procCurrentHoverItem = -1;
		_hoverZone = NULL;
		pauseJobs();
		z = data->_zone;
		if (runZone(z) == 0) {
			runCommands( z->_commands, z );
		}
		resumeJobs();
		break;

	case kEvOpenInventory:
		_procCurrentHoverItem = -1;
		_hoverZone = NULL;
		hideLabel(kPriority2);
		if (hitZone(kZoneYou, _mousePos.x, _mousePos.y) == 0) {
			setArrowCursor();
		}
		removeJob(_jRunScripts);
		_jDrawInventory = addJob(kJobShowInventory, 0, kPriority2);
		openInventory();
		break;

	case kEvCloseInventory: // closes inventory and possibly select item
		closeInventory();
		setInventoryCursor(data->_inventoryIndex);
		_jRunScripts = addJob(kJobRunScripts, 0, kPriority15);
		addJob(kJobHideInventory, 0, kPriority20);
		removeJob(_jDrawInventory);
		break;

	case kEvHoverInventory:
		highlightInventoryItem(_procCurrentHoverItem, 12);	// disable
		highlightInventoryItem(data->_inventoryIndex, 19);						// enable
		_procCurrentHoverItem = data->_inventoryIndex;
		break;

	case kEvWalk:
		debugC(2, kDebugInput, "processInput: kEvWalk");
		_hoverZone = NULL;
		setArrowCursor();
		_char.scheduleWalk(data->_mousePos.x, data->_mousePos.y);
		break;

	case kEvQuitGame:
		_engineFlags |= kEngineQuit;
		break;

	case kEvSaveGame:
		_hoverZone = NULL;
		saveGame();
		setArrowCursor();
		break;

	case kEvLoadGame:
		_hoverZone = NULL;
		loadGame();
		setArrowCursor();
		break;

	}

	return;
}



Parallaction::InputData *Parallaction::translateInput() {

	if (_keyDown == kEvQuitGame) {
		_input._event = kEvQuitGame;
		return &_input;
	}

	if (_keyDown == kEvSaveGame) {
		_input._event = kEvSaveGame;
		return &_input;
	}

	if (_keyDown == kEvLoadGame) {
		_input._event = kEvLoadGame;
		return &_input;
	}

	_input._mousePos = _mousePos;

	if (((_engineFlags & kEnginePauseJobs) == 0) && ((_engineFlags & kEngineInventory) == 0)) {

		if (_actionAfterWalk == true) {
			// if walking is over, then take programmed action
			_input._event = kEvAction;
			_actionAfterWalk = false;
			return &_input;
		}

		Zone *z = hitZone(_activeItem._id, _mousePos.x, _mousePos.y);

		if (_mouseButtons == kMouseRightDown) {
			// right button down shows inventory

			if (hitZone(kZoneYou, _mousePos.x, _mousePos.y) && (_activeItem._id != 0)) {
				_activeItem._index = (_activeItem._id >> 16) & 0xFFFF;
				_engineFlags |= kEngineDragging;
			}

			_input._event = kEvOpenInventory;
			_transCurrentHoverItem = -1;
			return &_input;
		}

		if (((_mouseButtons == kMouseLeftUp) && (_activeItem._id == 0) && ((_engineFlags & kEngineWalking) == 0)) && ((z == NULL) || ((z->_type & 0xFFFF) != kZoneCommand))) {
			_input._event = kEvWalk;
			return &_input;
		}

		if ((z != _hoverZone) && (_hoverZone != NULL)) {
			_hoverZone = NULL;
			_input._event = kEvExitZone;
//			_input._data= &z->_name;
			return &_input;
		}

		if (z == NULL) {
			return NULL;
		}

		if ((_hoverZone == NULL) && ((z->_flags & kFlagsNoName) == 0)) {
			_hoverZone = z;
			_input._event = kEvEnterZone;
			_input._label = &z->_label;
			return &_input;
		}

		if ((_mouseButtons == kMouseLeftUp) && ((_activeItem._id != 0) || ((z->_type & 0xFFFF) == kZoneCommand))) {

			_input._zone = z;
			if (z->_flags & kFlagsNoWalk) {
				// character doesn't need to walk to take specified action
				_input._event = kEvAction;

			} else {
				// action delayed: if Zone defined a moveto position the character is programmed to move there,
				// else it will move to the mouse position
				_input._event = kEvWalk;
				_actionAfterWalk = true;
				if (z->_moveTo.y != 0) {
					_input._mousePos = z->_moveTo;
				}
			}

			beep();
			setArrowCursor();
			return &_input;
		}

	}

	if ((_engineFlags & kEngineInventory) == 0) return NULL;

	// in inventory
	int16 _si = getHoverInventoryItem(_mousePos.x, _mousePos.y);

	if (_mouseButtons == kMouseRightUp) {
		// right up hides inventory

		_input._event = kEvCloseInventory;
		_input._inventoryIndex = getHoverInventoryItem(_mousePos.x, _mousePos.y);
		highlightInventoryItem(_transCurrentHoverItem, 12); 		// disable

		if ((_engineFlags & kEngineDragging) == 0) return &_input;

		_engineFlags &= ~kEngineDragging;
		Zone *z = hitZone(kZoneMerge, _activeItem._index, getInventoryItemIndex(_input._inventoryIndex));

		if (z != NULL) {
			dropItem(z->u.merge->_obj1);
			dropItem(z->u.merge->_obj2);
			addInventoryItem(z->u.merge->_obj3);
			runCommands(z->_commands);
		}

		return &_input;
	}

	if (_si == _transCurrentHoverItem) return NULL;

	_transCurrentHoverItem = _si;
	_input._event = kEvHoverInventory;
	_input._inventoryIndex = _si;
	return &_input;

}

uint32 Parallaction::getElapsedTime() {
	return g_system->getMillis() - _baseTime;
}

void Parallaction::resetTimer() {
	_baseTime = g_system->getMillis();
	return;
}


void Parallaction::waitTime(uint32 t) {

	uint32 v4 = 0;

	while (v4 < t * (1000 / 18.2)) {
		v4 = getElapsedTime();
	}

	resetTimer();

	return;
}


void Parallaction::showCursor(bool visible) {
	_mouseHidden = !visible;
	g_system->showMouse(visible);
}



void Parallaction::freeCharacter() {
	debugC(1, kDebugExec, "freeCharacter()");

	if (_objectsNames)
		delete _objectsNames;
	_objectsNames = NULL;

	_char.free();

	return;
}



/*
	helper function to provide *descending* ordering of the job list
	(higher priorities values comes first in the list)
*/
int compareJobPriority(const JobPointer &j1, const JobPointer &j2) {
	return (j1->_job->_tag >= j2->_job->_tag ? -1 : 1);
}

Job *Parallaction::addJob(uint functionId, void *parm, uint16 tag) {
	debugC(9, kDebugExec, "addJob(%i)", tag);

	Job *v8 = new Job;

	v8->_parm = parm;
	v8->_tag = tag;
	v8->_finished = 0;
	v8->_count = 0;

	JobOpcode *op = createJobOpcode(functionId, v8);

	_jobs.insertSorted(op, compareJobPriority);

	return v8;
}

void Parallaction::removeJob(Job *j) {
	debugC(9, kDebugExec, "addJob(%i)", j->_tag);

	j->_finished = 1;
	return;
}

void Parallaction::pauseJobs() {
	debugC(9, kDebugExec, "pausing jobs execution");

	_engineFlags |= kEnginePauseJobs;
	return;
}

void Parallaction::resumeJobs() {
	debugC(9, kDebugExec, "resuming jobs execution");

	_engineFlags &= ~kEnginePauseJobs;
	return;
}

void Parallaction::runJobs() {

	if (_engineFlags & kEnginePauseJobs) return;

	JobList::iterator it = _jobs.begin();
	while (it != _jobs.end()) {
		Job *job = (*it)->_job;
		if (job->_finished == 1)
			it = _jobs.erase(it);
		else
			it++;
	}

	it = _jobs.begin();
	while (it != _jobs.end()) {
		Job *job = (*it)->_job;
		debugC(9, kDebugExec, "runJobs: %i", job->_tag);
		(*(*it))();
		it++;
	}


	return;
}




void Parallaction::pushParserTables(OpcodeSet *opcodes, Table *statements) {
	_opcodes.push(_currentOpcodes);
	_statements.push(_currentStatements);

	_currentOpcodes = opcodes;
	_currentStatements = statements;
}

void Parallaction::popParserTables() {
	assert(_opcodes.size() > 0);

	_currentOpcodes = _opcodes.pop();
	_currentStatements = _statements.pop();
}

void Parallaction::parseStatement() {
	assert(_currentOpcodes != 0);

	_lookup = _currentStatements->lookup(_tokens[0]);

	debugC(9, kDebugParser, "parseStatement: %s (lookup = %i)", _tokens[0], _lookup);

	(*(*_currentOpcodes)[_lookup])();
}




Animation *Parallaction::findAnimation(const char *name) {

	for (AnimationList::iterator it = _animations.begin(); it != _animations.end(); it++)
		if (!scumm_stricmp((*it)->_label._text, name)) return *it;

	return NULL;
}

void Parallaction::freeAnimations() {
	_animations.clear();
	return;
}

int compareAnimationZ(const AnimationPointer &a1, const AnimationPointer &a2) {
	if (a1->_z == a2->_z) return 0;
	return (a1->_z < a2->_z ? -1 : 1);
}

void Parallaction::sortAnimations() {
	_char._ani._z = _char._ani.height() + _char._ani._top;
	_animations.sort(compareAnimationZ);
	return;
}


void Parallaction::allocateLocationSlot(const char *name) {
	// WORKAROUND: the original code erroneously incremented
	// _currentLocationIndex, thus producing inconsistent
	// savegames. This workaround modified the following loop
	// and if-statement, so the code exactly matches the one
	// in Big Red Adventure.
	_currentLocationIndex = -1;
	uint16 _di = 0;
	while (_locationNames[_di][0] != '\0') {
		if (!scumm_stricmp(_locationNames[_di], name)) {
			_currentLocationIndex = _di;
		}
		_di++;
	}

	if (_di == 120)
		error("No more location slots available. Please report this immediately to ScummVM team.");

	if (_currentLocationIndex  == -1) {
		strcpy(_locationNames[_numLocations], name);
		_currentLocationIndex = _numLocations;

		_numLocations++;
		_locationNames[_numLocations][0] = '\0';
		_localFlags[_numLocations] = 0;
	} else {
		_localFlags[_currentLocationIndex] |= kFlagsVisited;	// 'visited'
	}
}



void Parallaction::freeLocation() {
	debugC(2, kDebugExec, "freeLocation");

	_soundMan->stopSfx(0);
	_soundMan->stopSfx(1);
	_soundMan->stopSfx(2);
	_soundMan->stopSfx(3);

	_localFlagNames->clear();

	_location._walkNodes.clear();

	freeZones();
	freeAnimations();

	if (_location._comment) {
		free(_location._comment);
	}
	_location._comment = NULL;

	_location._commands.clear();
	_location._aCommands.clear();

	return;
}




void Parallaction::freeBackground() {

	if (!_backgroundInfo)
		return;

	_backgroundInfo->bg.free();
	_backgroundInfo->mask.free();
	_backgroundInfo->path.free();

	_pathBuffer = 0;

}

void Parallaction::setBackground(const char* name, const char* mask, const char* path) {

	_disk->loadScenery(*_backgroundInfo, name, mask, path);

	_gfx->setPalette(_backgroundInfo->palette);
	_gfx->_palette.clone(_backgroundInfo->palette);
	_gfx->setBackground(&_backgroundInfo->bg);

	if (_backgroundInfo->mask.data)
		_gfx->setMask(&_backgroundInfo->mask);

	if (_backgroundInfo->path.data)
		_pathBuffer = &_backgroundInfo->path;

	return;
}

void Parallaction::showLocationComment(const char *text, bool end) {

	_gfx->setFont(_dialogueFont);

	int16 w, h;
	_gfx->getStringExtent(const_cast<char*>(text), 130, &w, &h);

	Common::Rect r(w + (end ? 5 : 10), h + 5);
	r.moveTo(5, 5);

	_gfx->floodFill(Gfx::kBitFront, r, 0);
	r.grow(-2);
	_gfx->floodFill(Gfx::kBitFront, r, 1);
	_gfx->displayWrappedString(const_cast<char*>(text), 3, 5, 0, 130);

	_gfx->updateScreen();

	return;
}

void Parallaction::switchBackground(const char* background, const char* mask) {
//	printf("switchBackground(%s)", name);

	Palette pal;

	uint16 v2 = 0;
	if (!scumm_stricmp(background, "final")) {
		_gfx->clearScreen(Gfx::kBitBack);
		for (uint16 _si = 0; _si <= 32; _si++) {
			pal.setEntry(_si, v2, v2, v2);
			v2 += 4;
		}

		g_system->delayMillis(20);
		_gfx->setPalette(pal);
		_gfx->updateScreen();
	}

	setBackground(background, mask, mask);

	return;
}


void Parallaction::showSlide(const char *name) {

	BackgroundInfo info;

	_disk->loadSlide(info, name);

	// TODO: avoid using screen buffers for displaying slides. Using a generic buffer
	// allows for positioning of graphics as needed by Big Red Adventure.
	// The main problem lies with menu, which relies on multiple buffers, mainly because
	// it is crappy code.
	_gfx->setBackground(&info.bg);
	_gfx->setPalette(info.palette);
	_gfx->copyScreen(Gfx::kBitBack, Gfx::kBitFront);

	info.bg.free();
	info.mask.free();
	info.path.free();

	return;
}



//	displays transition before a new location
//
//	clears screen (in white??)
//	shows location comment (if any)
//	waits for mouse click
//	fades towards game palette
//
void Parallaction::doLocationEnterTransition() {
	debugC(2, kDebugExec, "doLocationEnterTransition");

    if (_localFlags[_currentLocationIndex] & kFlagsVisited) {
        debugC(2, kDebugExec, "skipping location transition");
        return; // visited
    }

	Palette pal(_gfx->_palette);
	pal.makeGrayscale();
	_gfx->setPalette(pal);

	jobRunScripts(NULL, NULL);
	jobEraseAnimations(NULL, NULL);
	jobDisplayAnimations(NULL, NULL);

	_gfx->swapBuffers();
	_gfx->copyScreen(Gfx::kBitFront, Gfx::kBitBack);

	showLocationComment(_location._comment, false);
	waitUntilLeftClick();

	_gfx->copyScreen(Gfx::kBitBack, Gfx::kBitFront );

	// fades maximum intensity palette towards approximation of main palette
	for (uint16 _si = 0; _si<6; _si++) {
		pal.fadeTo(_gfx->_palette, 4);
		_gfx->setPalette(pal);
		waitTime( 1 );
		_gfx->updateScreen();
	}

	debugC(2, kDebugExec, "doLocationEnterTransition completed");

	return;
}



Zone *Parallaction::findZone(const char *name) {

	for (ZoneList::iterator it = _zones.begin(); it != _zones.end(); it++) {
		if (!scumm_stricmp((*it)->_label._text, name)) return *it;
	}

	return findAnimation(name);
}


void Parallaction::freeZones() {
	debugC(2, kDebugExec, "freeZones: kEngineQuit = %i", _engineFlags & kEngineQuit);

	ZoneList::iterator it = _zones.begin();

	while ( it != _zones.end() ) {

		// NOTE : this condition has been relaxed compared to the original, to allow the engine
		// to retain special - needed - zones that were lost across location switches.
		Zone* z = *it;
		if (((z->_top == -1) || (z->_left == -2)) && ((_engineFlags & kEngineQuit) == 0)) {
			debugC(2, kDebugExec, "freeZones preserving zone '%s'", z->_label._text);
			it++;
		} else {
			it = _zones.erase(it);
		}
	}

	return;
}


const char Character::_prefixMini[] = "mini";
const char Character::_suffixTras[] = "tras";
const char Character::_empty[] = "\0";


Character::Character(Parallaction *vm) : _vm(vm), _builder(&_ani) {
	_talk = NULL;
	_head = NULL;
	_objs = NULL;

	_ani._left = 150;
	_ani._top = 100;
	_ani._z = 10;
	_ani._oldPos.x = -1000;
	_ani._oldPos.y = -1000;
	_ani._frame = 0;
	_ani._flags = kFlagsActive | kFlagsNoName;
	_ani._type = kZoneYou;
	_ani._label._cnv.pixels = NULL;
	_ani._label._text = strdup("yourself");
}

void Character::getFoot(Common::Point &foot) {
	foot.x = _ani._left + _ani.width() / 2;
	foot.y = _ani._top + _ani.height();
}

void Character::setFoot(const Common::Point &foot) {
	_ani._left = foot.x - _ani.width() / 2;
	_ani._top = foot.y - _ani.height();
}

void Character::scheduleWalk(int16 x, int16 y) {
	if ((_ani._flags & kFlagsRemove) || (_ani._flags & kFlagsActive) == 0) {
		return;
	}

	WalkNodeList *list = _builder.buildPath(x, y);
	_vm->addJob(kJobWalk, list, kPriority19 );

	_engineFlags |= kEngineWalking;
}

void Character::free() {

	if (_ani._cnv)
		delete _ani._cnv;
	if (_talk)
		delete _talk;
	if (_head)
		delete _head;
	if (_objs)
		delete _objs;

	_ani._cnv = NULL;
	_talk = NULL;
	_head = NULL;
	_objs = NULL;

	return;
}


// Various ways of detecting character modes used to exist
// inside the engine, so they have been unified in the two
// following macros.
// Mini characters are those used in far away shots, like
// the highway scenery, while Dummy characters are a mere
// workaround to keep the engine happy when showing slides.
// As a sidenote, standard sized characters' names start
// with a lowercase 'd'.
#define IS_MINI_CHARACTER(s) (((s)[0] == 'm'))
#define IS_DUMMY_CHARACTER(s) (((s)[0] == 'D'))

void Character::setName(const char *name) {
	const char *begin = name;
	const char *end = begin + strlen(name);

	_prefix = _empty;
	_suffix = _empty;

	if (IS_DUMMY_CHARACTER(name)) {
		begin = 0;
		end = 0;
	} else {

		const char *s = strstr(name, "tras");
		if (s) {
			_suffix = _suffixTras;
			end = s;
		}

		if (IS_MINI_CHARACTER(name)) {
			_prefix = _prefixMini;
			begin = name+4;
		}

	}

	memset(_baseName, 0, 30);
	strncpy(_baseName, begin, end - begin);
	sprintf(_name, "%s%s", _prefix, _baseName);
	sprintf(_fullName, "%s%s%s", _prefix, _baseName, _suffix);
}

void Character::transform() {
	if (scumm_stricmp("donna", _baseName)) {
		error("can't transform character %s", _baseName);
	}

	if (_suffix) {
		_suffix = _empty;
	} else {
		_suffix = _suffixTras;
	}

	_engineFlags ^= kEngineTransformedDonna;
	setName(_name);
}

const char *Character::getName() const {
	return _name;
}

const char *Character::getBaseName() const {
	return _baseName;
}

const char *Character::getFullName() const {
	return _fullName;
}


void Parallaction::beep() {
	_soundMan->playSfx("beep", 3, false);
}



} // namespace Parallaction
