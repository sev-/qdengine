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
#include "qdengine/core/parser/xml_tag_buffer.h"
#include "qdengine/core/parser/qdscr_parser.h"
#include "qdengine/core/qdcore/qd_game_end.h"
#include "qdengine/core/qdcore/qd_game_dispatcher.h"


namespace QDEngine {

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

qdGameEnd::qdGameEnd() {
}

qdGameEnd::qdGameEnd(const qdGameEnd &end) : qdConditionalObject(end),
	interface_screen_(end.interface_screen_) {
}

qdGameEnd::~qdGameEnd() {
}

qdGameEnd &qdGameEnd::operator = (const qdGameEnd &end) {
	if (this == &end) return *this;

	*static_cast<qdConditionalObject *>(this) = end;

	interface_screen_ = end.interface_screen_;

	return *this;
}

qdConditionalObject::trigger_start_mode qdGameEnd::trigger_start() {
	if (qdGameDispatcher * p = qdGameDispatcher::get_dispatcher()) {
		p -> set_game_end(this);
		return qdConditionalObject::TRIGGER_START_ACTIVATE;
	}

	return qdConditionalObject::TRIGGER_START_FAILED;
}

bool qdGameEnd::load_script(const xml::tag *p) {
	load_conditions_script(p);

	for (xml::tag::subtag_iterator it = p -> subtags_begin(); it != p -> subtags_end(); ++it) {
		switch (it -> ID()) {
		case QDSCR_NAME:
			set_name(it -> data());
			break;
		case QDSCR_FLAG:
			set_flag(xml::tag_buffer(*it).get_int());
			break;
		case QDSCR_GAME_END_SCREEN:
			set_interface_screen(it -> data());
			break;
		}
	}

	return true;
}

bool qdGameEnd::save_script(Common::SeekableWriteStream &fh, int indent) const {
	for (int i = 0; i < indent; i++) {
		fh.writeString("\t");
	}

	fh.writeString("<game_end");

	if (name()) {
		fh.writeString(Common::String::format(" name=\"%s\"", qdscr_XML_string(name())));
	}

	if (flags()) {
		fh.writeString(Common::String::format(" flags=\"%d\"", flags()));
	}

	if (!interface_screen_.empty()) {
		fh.writeString(Common::String::format(" end_screen=\"%s\"", qdscr_XML_string(interface_screen_.c_str())));
	}

	if (has_conditions()) {
		fh.writeString(">\r\n");
		save_conditions_script(fh, indent);
		for (int i = 0; i < indent; i++) {
			fh.writeString("\t");
		}
		fh.writeString("</game_end>\r\n");
	}
	return true;
}

bool qdGameEnd::save_script(class XStream &fh, int indent) const {
	warning("STUB: qdGameEnd::save_script(XStream)");
	return true;
}

} // namespace QDEngine
