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

#include "qdengine/core/qd_precomp.h"
#include "qdengine/core/system/input/keyboard_input.h"

namespace QDEngine {
/* ---------------------------- INCLUDE SECTION ----------------------------- */


/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

keyboardDispatcher::keyboardDispatcher() : handler_(0) {
	for (int i = 0; i < 256; i ++) key_states_[i] = false;
}

keyboardDispatcher::~keyboardDispatcher() {
}

keyboardDispatcher *keyboardDispatcher::instance() {
	static keyboardDispatcher dsp;
	return &dsp;
}

} // namespace QDEngine
