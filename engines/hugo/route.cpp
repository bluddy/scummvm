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

/*
 * This code is based on original Hugo Trilogy source code
 *
 * Copyright (c) 1989-1995 David P. Gray
 *
 */

// Find shortest route from hero to destination

#include "common/system.h"

#include "hugo/hugo.h"
#include "hugo/game.h"
#include "hugo/route.h"
#include "hugo/object.h"
#include "hugo/inventory.h"
#include "hugo/mouse.h"

namespace Hugo {
Route::Route(HugoEngine *vm) : _vm(vm) {
	_oldWalkDirection = 0;
	_routeIndex       = -1;                         // Hero not following a route
	_routeType        = kRouteSpace;                // Hero walking to space
	_routeObjId       = -1;                         // Hero not walking to anything
}

void Route::resetRoute() {
	_routeIndex = -1;
}

int16 Route::getRouteIndex() const {
	return _routeIndex;
}

/**
 * Face hero in new direction, based on cursor key input by user.
 */
void Route::setDirection(const uint16 keyCode) {
	debugC(1, kDebugRoute, "setDirection(%d)", keyCode);

	object_t *obj = _vm->_hero;                     // Pointer to hero object

	// Set first image in sequence
	switch (keyCode) {
	case Common::KEYCODE_UP:
	case Common::KEYCODE_KP8:
		obj->currImagePtr = obj->seqList[_UP].seqPtr;
		break;
	case Common::KEYCODE_DOWN:
	case Common::KEYCODE_KP2:
		obj->currImagePtr = obj->seqList[DOWN].seqPtr;
		break;
	case Common::KEYCODE_LEFT:
	case Common::KEYCODE_KP4:
		obj->currImagePtr = obj->seqList[LEFT].seqPtr;
		break;
	case Common::KEYCODE_RIGHT:
	case Common::KEYCODE_KP6:
		obj->currImagePtr = obj->seqList[RIGHT].seqPtr;
		break;
	case Common::KEYCODE_HOME:
	case Common::KEYCODE_KP7:
		obj->currImagePtr = obj->seqList[LEFT].seqPtr;
		break;
	case Common::KEYCODE_END:
	case Common::KEYCODE_KP1:
		obj->currImagePtr = obj->seqList[LEFT].seqPtr;
		break;
	case Common::KEYCODE_PAGEUP:
	case Common::KEYCODE_KP9:
		obj->currImagePtr = obj->seqList[RIGHT].seqPtr;
		break;
	case Common::KEYCODE_PAGEDOWN:
	case Common::KEYCODE_KP3:
		obj->currImagePtr = obj->seqList[RIGHT].seqPtr;
		break;
	}
}

/**
 * Set hero walking, based on cursor key input by user.
 * Hitting same key twice will stop hero.
 */
void Route::setWalk(const uint16 direction) {
	debugC(1, kDebugRoute, "setWalk(%d)", direction);

	object_t *obj = _vm->_hero;                     // Pointer to hero object

	if (_vm->getGameStatus().storyModeFl || obj->pathType != kPathUser) // Make sure user has control
		return;

	if (!obj->vx && !obj->vy)
		_oldWalkDirection = 0;                      // Fix for consistant restarts

	if (direction != _oldWalkDirection) {
		// Direction has changed
		setDirection(direction);                    // Face new direction
		obj->vx = obj->vy = 0;
		switch (direction) {                        // And set correct velocity
		case Common::KEYCODE_UP:
		case Common::KEYCODE_KP8:
			obj->vy = -kStepDy;
			break;
		case Common::KEYCODE_DOWN:
		case Common::KEYCODE_KP2:
			obj->vy =  kStepDy;
			break;
		case Common::KEYCODE_LEFT:
		case Common::KEYCODE_KP4:
			obj->vx = -kStepDx;
			break;
		case Common::KEYCODE_RIGHT:
		case Common::KEYCODE_KP6:
			obj->vx =  kStepDx;
			break;
		case Common::KEYCODE_HOME:
		case Common::KEYCODE_KP7:
			obj->vx = -kStepDx;
			// Note: in v1 Dos and v2 Dos, obj->vy is set to DY
			obj->vy = -kStepDy / 2;
			break;
		case Common::KEYCODE_END:
		case Common::KEYCODE_KP1:
			obj->vx = -kStepDx;
			// Note: in v1 Dos and v2 Dos, obj->vy is set to -DY
			obj->vy =  kStepDy / 2;
			break;
		case Common::KEYCODE_PAGEUP:
		case Common::KEYCODE_KP9:
			obj->vx =  kStepDx;
			// Note: in v1 Dos and v2 Dos, obj->vy is set to -DY
			obj->vy = -kStepDy / 2;
			break;
		case Common::KEYCODE_PAGEDOWN:
		case Common::KEYCODE_KP3:
			obj->vx =  kStepDx;
			// Note: in v1 Dos and v2 Dos, obj->vy is set to DY
			obj->vy =  kStepDy / 2;
			break;
		}
		_oldWalkDirection = direction;
		obj->cycling = kCycleForward;
	} else {
		// Same key twice - halt hero
		obj->vy = 0;
		obj->vx = 0;
		_oldWalkDirection = 0;
		obj->cycling = kCycleNotCycling;
	}
}

/**
 * Recursive algorithm!  Searches from hero to dest_x, dest_y
 * Find horizontal line segment about supplied point and recursively
 * find line segments for each point above and below that segment.
 * When destination point found in segment, start surfacing and leave
 * a trail in segment[] from destination back to hero.
 *
 * Note:  there is a bug which allows a route through a 1-pixel high
 * narrow gap if between 2 segments wide enough for hero.  To work
 * around this, make sure any narrow gaps are 2 or more pixels high.
 * An example of this was the blocking guard in Hugo1/Dead-End.
 */
void Route::segment(int16 x, int16 y) {
	debugC(1, kDebugRoute, "segment(%d, %d)", x, y);

	// Note: use of static - can't waste stack
	static image_pt   p;                            // Ptr to _boundaryMap[y]
	static segment_t *seg_p;                        // Ptr to segment

	// Bomb out if stack exhausted
	// Vinterstum: Is this just a safeguard, or actually used?
	//_fullStackFl = _stackavail () < 256;
	_fullStackFl = false;

	// Find and fill on either side of point
	p = _boundaryMap[y];
	int16 x1, x2;                                   // Range of segment
	for (x1 = x; x1 > 0; x1--) {
		if (p[x1] == 0) {
			p[x1] = kMapFill;
		} else {
			break;
		}
	}
	for (x2 = x + 1; x2 < kXPix; x2++) {
		if (p[x2] == 0) {
			p[x2] = kMapFill;
		} else {
			break;
		}
	}
	x1++;
	x2--;

	// Discard path if not wide enough for hero - dead end
	if (_heroWidth > x2 - x1 + 1)
		return;

	// Have we found the destination yet?
	if (y == _destY && x1 <= _destX && x2 >= _destX)
		_routeFoundFl = true;

	// Bounds check y in case no boundary around screen
	if (y <= 0 || y >= kYPix - 1)
		return;

	if (_vm->_hero->x < x1) {
		// Hero x not in segment, search x1..x2
		// Find all segments above current
		for (x = x1; !(_routeFoundFl | _fullStackFl | _fullSegmentFl) && x <= x2; x++) {
			if (_boundaryMap[y - 1][x] == 0)
				segment(x, y - 1);
		}

		// Find all segments below current
		for (x = x1; !(_routeFoundFl | _fullStackFl | _fullSegmentFl) && x <= x2; x++) {
			if (_boundaryMap[y + 1][x] == 0)
				segment(x, y + 1);
		}
	} else if (_vm->_hero->x + kHeroMaxWidth > x2) {
		// Hero x not in segment, search x1..x2
		// Find all segments above current
		for (x = x2; !(_routeFoundFl | _fullStackFl | _fullSegmentFl) && x >= x1; x--) {
			if (_boundaryMap[y - 1][x] == 0)
				segment(x, y - 1);
		}

		// Find all segments below current
		for (x = x2; !(_routeFoundFl | _fullStackFl | _fullSegmentFl) && x >= x1; x--) {
			if (_boundaryMap[y + 1][x] == 0)
				segment(x, y + 1);
		}
	} else {
		// Organize search around hero x position - this gives
		// better chance for more direct route.
		for (x = _vm->_hero->x; !(_routeFoundFl | _fullStackFl | _fullSegmentFl) && x <= x2; x++) {
			if (_boundaryMap[y - 1][x] == 0)
				segment(x, y - 1);
		}

		for (x = x1; !(_routeFoundFl | _fullStackFl | _fullSegmentFl) && x < _vm->_hero->x; x++) {
			if (_boundaryMap[y - 1][x] == 0)
				segment(x, y - 1);
		}

		for (x = _vm->_hero->x; !(_routeFoundFl | _fullStackFl | _fullSegmentFl) && x <= x2; x++) {
			if (_boundaryMap[y + 1][x] == 0)
				segment(x, y + 1);
		}

		for (x = x1; !(_routeFoundFl | _fullStackFl | _fullSegmentFl) && x < _vm->_hero->x; x++) {
			if (_boundaryMap[y + 1][x] == 0)
				segment(x, y + 1);
		}
	}

	// If found, surface, leaving trail back to hero
	if (_routeFoundFl) {
		// Bomb out if too many segments (leave one spare)
		if (_segmentNumb >= kMaxSeg - 1) {
			_fullSegmentFl = true;
		} else {
			// Create segment
			seg_p = &_segment[_segmentNumb];
			seg_p->y  = y;
			seg_p->x1 = x1;
			seg_p->x2 = x2;
			_segmentNumb++;
		}
	}
}

/**
 * Create and return ptr to new node.  Initialize with previous node.
 * Returns 0 if MAX_NODES exceeded
 */
Point *Route::newNode() {
	debugC(1, kDebugRoute, "newNode");

	_routeListIndex++;
	if (_routeListIndex >= kMaxNodes)               // Too many nodes
		return 0;                                   // Incomplete route - failure

	_route[_routeListIndex] = _route[_routeListIndex - 1];  // Initialize with previous node
	return &_route[_routeListIndex];
}

/**
 * Construct route to cx, cy.  Return TRUE if successful.
 * 1.  Copy boundary bitmap to local byte map (include object bases)
 * 2.  Construct list of segments segment[] from hero to destination
 * 3.  Compress to shortest route in route[]
 */
bool Route::findRoute(const int16 cx, const int16 cy) {
	debugC(1, kDebugRoute, "findRoute(%d, %d)", cx, cy);

	// Initialize for search
	_routeFoundFl  = false;                         // Path not found yet
	_fullStackFl = false;                           // Stack not exhausted
	_fullSegmentFl  = false;                        // Segments not exhausted
	_segmentNumb = 0;                               // Segment index
	_heroWidth = kHeroMinWidth;                     // Minimum width of hero
	_destY = cy;                                    // Destination coords
	_destX = cx;                                    // Destination coords

	int16 herox1 = _vm->_hero->x + _vm->_hero->currImagePtr->x1;        // Hero baseline
	int16 herox2 = _vm->_hero->x + _vm->_hero->currImagePtr->x2;        // Hero baseline
	int16 heroy  = _vm->_hero->y + _vm->_hero->currImagePtr->y2;        // Hero baseline

	// Store all object baselines into objbound (except hero's = [0])
	object_t  *obj;                                 // Ptr to object
	int i;
	for (i = 1, obj = &_vm->_object->_objects[i]; i < _vm->_object->_numObj; i++, obj++) {
		if ((obj->screenIndex == *_vm->_screen_p) && (obj->cycling != kCycleInvisible) && (obj->priority == kPriorityFloating))
			_vm->_object->storeBoundary(obj->oldx + obj->currImagePtr->x1, obj->oldx + obj->currImagePtr->x2, obj->oldy + obj->currImagePtr->y2);
	}

	// Combine objbound and boundary bitmaps to local byte map
	for (uint16 y = 0; y < kYPix; y++) {
		for (uint16 x = 0; x < kCompLineSize; x++) {
			uint16 boundIdx = y * kCompLineSize + x;
			for (i = 0; i < 8; i++)
				_boundaryMap[y][x * 8 + i] = ((_vm->_object->getObjectBoundary(boundIdx) | _vm->_object->getBoundaryOverlay(boundIdx)) & (0x80 >> i)) ? kMapBound : 0;
		}
	}

	// Clear all object baselines from objbound
	for (i = 0, obj = _vm->_object->_objects; i < _vm->_object->_numObj; i++, obj++) {
		if ((obj->screenIndex == *_vm->_screen_p) && (obj->cycling != kCycleInvisible) && (obj->priority == kPriorityFloating))
			_vm->_object->clearBoundary(obj->oldx + obj->currImagePtr->x1, obj->oldx + obj->currImagePtr->x2, obj->oldy + obj->currImagePtr->y2);
	}

	// Search from hero to destination
	segment(herox1, heroy);

	// Not found or not enough stack or MAX_SEG exceeded
	if (!_routeFoundFl || _fullStackFl || _fullSegmentFl) {
		return false;
	}

	// Now find the route of nodes from destination back to hero
	// Assign first node as destination
	_route[0].x = _destX;
	_route[0].y = _destY;

	// Make a final segment for hero's base (we left a spare)
	_segment[_segmentNumb].y  = heroy;
	_segment[_segmentNumb].x1 = herox1;
	_segment[_segmentNumb].x2 = herox2;
	_segmentNumb++;

	Point     *routeNode;                           // Ptr to route node
	// Look in segments[] for straight lines from destination to hero
	for (i = 0, _routeListIndex = 0; i < _segmentNumb - 1; i++) {
		if ((routeNode = newNode()) == 0)           // New node for new segment
			return false;                           // Too many nodes
		routeNode->y = _segment[i].y;

		// Look ahead for furthest straight line
		for (int16 j = i + 1; j < _segmentNumb; j++) {
			segment_t *seg_p = &_segment[j];
			// Can we get to this segment from previous node?
			if (seg_p->x1 <= routeNode->x && seg_p->x2 >= routeNode->x + _heroWidth - 1) {
				routeNode->y = seg_p->y;            // Yes, keep updating node
			} else {
				// No, create another node on previous segment to reach it
				if ((routeNode = newNode()) == 0)   // Add new route node
					return false;                   // Too many nodes

				// Find overlap between old and new segments
				int16 x1 = MAX(_segment[j - 1].x1, seg_p->x1);
				int16 x2 = MIN(_segment[j - 1].x2, seg_p->x2);

				// If room, add a little offset to reduce staircase effect
				int16 dx = kHeroMaxWidth >> 1;
				if (x2 - x1 < _heroWidth + dx)
					dx = 0;

				// Bear toward final hero position
				if (j == _segmentNumb - 1)
					routeNode->x = herox1;
				else if (herox1 < x1)
					routeNode->x = x1 + dx;
				else if (herox1 > x2 - _heroWidth + 1)
					routeNode->x = x2 - _heroWidth - dx;
				else
					routeNode->x = herox1;
				i = j - 2;                          // Restart segment (-1 to offset auto increment)
				break;
			}
		}

		// Terminate loop if we've reached hero
		if (routeNode->x == herox1 && routeNode->y == heroy)
			break;
	}
	return true;
}

/**
 * Process hero in route mode - called from Move_objects()
 */
void Route::processRoute() {
	debugC(1, kDebugRoute, "processRoute");

	static bool turnedFl = false;                   // Used to get extra cylce for turning

	if (_routeIndex < 0)
		return;

	// Current hero position
	int16 herox = _vm->_hero->x + _vm->_hero->currImagePtr->x1;
	int16 heroy = _vm->_hero->y + _vm->_hero->currImagePtr->y2;
	Point *routeNode = &_route[_routeIndex];

	// Arrived at node?
	if (abs(herox - routeNode->x) < kStepDx + 1 && abs(heroy - routeNode->y) < kStepDy) {
		// kStepDx too low
		// Close enough - position hero exactly
		_vm->_hero->x = _vm->_hero->oldx = routeNode->x - _vm->_hero->currImagePtr->x1;
		_vm->_hero->y = _vm->_hero->oldy = routeNode->y - _vm->_hero->currImagePtr->y2;
		_vm->_hero->vx = _vm->_hero->vy = 0;
		_vm->_hero->cycling = kCycleNotCycling;

		// Arrived at final node?
		if (--_routeIndex < 0) {
			// See why we walked here
			switch (_routeType) {
			case kRouteExit:                        // Walked to an exit, proceed into it
				setWalk(_vm->_mouse->getDirection(_routeObjId));
				break;
			case kRouteLook:                        // Look at an object
				if (turnedFl) {
					_vm->_object->lookObject(&_vm->_object->_objects[_routeObjId]);
					turnedFl = false;
				} else {
					setDirection(_vm->_object->_objects[_routeObjId].direction);
					_routeIndex++;                  // Come round again
					turnedFl = true;
				}
				break;
			case kRouteGet:                         // Get (or use) an object
				if (turnedFl) {
					_vm->_object->useObject(_routeObjId);
					turnedFl = false;
				} else {
					setDirection(_vm->_object->_objects[_routeObjId].direction);
					_routeIndex++;                  // Come round again
					turnedFl = true;
				}
				break;
			default:
				break;
			}
		}
	} else if (_vm->_hero->vx == 0 && _vm->_hero->vy == 0) {
		// Set direction of travel if at a node
		// Note realignment when changing to (thinner) up/down sprite,
		// otherwise hero could bump into boundaries along route.
		if (herox < routeNode->x) {
			setWalk(Common::KEYCODE_RIGHT);
		} else if (herox > routeNode->x) {
			setWalk(Common::KEYCODE_LEFT);
		} else if (heroy < routeNode->y) {
			setWalk(Common::KEYCODE_DOWN);
			_vm->_hero->x = _vm->_hero->oldx = routeNode->x - _vm->_hero->currImagePtr->x1;
		} else if (heroy > routeNode->y) {
			setWalk(Common::KEYCODE_UP);
			_vm->_hero->x = _vm->_hero->oldx = routeNode->x - _vm->_hero->currImagePtr->x1;
		}
	}
}

/**
 * Start a new route from hero to cx, cy
 * go_for is the purpose, id indexes the exit or object to walk to
 * Returns FALSE if route not found
 */
bool Route::startRoute(const go_t routeType, const int16 objId, int16 cx, int16 cy) {
	debugC(1, kDebugRoute, "startRoute(%d, %d, %d, %d)", routeType, objId, cx, cy);

	// Don't attempt to walk if user does not have control
	if (_vm->_hero->pathType != kPathUser)
		return false;

	// if inventory showing, make it go away
	if (_vm->_inventory->getInventoryState() != kInventoryOff)
		_vm->_inventory->setInventoryState(kInventoryUp);

	_routeType = routeType;                         // Purpose of trip
	_routeObjId  = objId;                           // Index of exit/object

	// Adjust destination to center hero if walking to cursor
	if (_routeType == kRouteSpace)
		cx -= kHeroMinWidth / 2;

	bool foundFl = false;                           // TRUE if route found ok
	if ((foundFl = findRoute(cx, cy))) {            // Found a route?
		_routeIndex = _routeListIndex;              // Node index
		_vm->_hero->vx = _vm->_hero->vy = 0;        // Stop manual motion
	}

	return foundFl;
}

} // End of namespace Hugo
