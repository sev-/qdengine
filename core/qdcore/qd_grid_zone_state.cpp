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

/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "qdengine/core/qd_precomp.h"
#include "qdengine/core/qdcore/qd_grid_zone.h"
#include "qdengine/core/qdcore/qd_grid_zone_state.h"


namespace QDEngine {


/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

const char *const qdGridZoneState::ZONE_STATE_ON_NAME = "Вкл";
const char *const qdGridZoneState::ZONE_STATE_OFF_NAME = "Выкл";

qdGridZoneState::qdGridZoneState(bool st) : state_(st) {
	if (st)
		set_name(ZONE_STATE_ON_NAME);
	else
		set_name(ZONE_STATE_OFF_NAME);
}

qdGridZoneState::qdGridZoneState(const qdGridZoneState &st) : qdConditionalObject(st),
	state_(st.state_) {
}

qdGridZoneState::~qdGridZoneState() {
}

qdGridZoneState &qdGridZoneState::operator = (const qdGridZoneState &st) {
	if (this == &st) return *this;

	state_ = st.state_;

	return *this;
}

bool qdGridZoneState::load_script(const xml::tag *p) {
	return load_conditions_script(p);
}

bool qdGridZoneState::save_script(Common::SeekableWriteStream &fh, int indent) const {
	for (int i = 0; i <= indent; i ++) {
		fh.writeString("\t");
	}

	fh.writeString("<grid_zone_state");

	if (state_) {
		fh.writeString(" state=\"1\"");
	} else {
		fh.writeString(" state=\"0\"");
	};

	if (has_conditions()) {
		fh.writeString("/>\r\n");
		save_conditions_script(fh, indent);
		for (int i = 0; i <= indent; i ++) {
			fh.writeString("\t");
		}
		fh.writeString("</grid_zone_state>\r\n");
	} else {
		fh.writeString("/>\r\n");
	}

	return true;
}

bool qdGridZoneState::save_script(class XStream &fh, int indent) const {
	for (int i = 0; i < indent; i ++) fh < "\t";
	fh < "<grid_zone_state";

	if (state_) fh < " state=\"1\"";
	else fh < " state=\"0\"";

	if (has_conditions()) {
		fh < ">\r\n";

		save_conditions_script(fh, indent);

		for (int i = 0; i < indent; i ++) fh < "\t";
		fh < "</grid_zone_state>\r\n";
	} else
		fh < "/>\r\n";

	return true;
}

qdConditionalObject::trigger_start_mode qdGridZoneState::trigger_start() {
	if (!owner()) return qdConditionalObject::TRIGGER_START_FAILED;

	static_cast<qdGridZone *>(owner()) -> set_state(state());

	return qdConditionalObject::TRIGGER_START_ACTIVATE;
}
} // namespace QDEngine
