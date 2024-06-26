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
#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "common/stream.h"
#include "qdengine/core/qd_precomp.h"
#include "qdengine/core/parser/xml_tag_buffer.h"
#include "qdengine/core/parser/qdscr_parser.h"
#include "qdengine/core/qdcore/qd_camera_mode.h"


namespace QDEngine {


/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

qdCameraMode::qdCameraMode() : camera_mode_(MODE_UNASSIGNED),
	work_time_(0.0f),
	scrolling_speed_(100.0f),
	scrolling_distance_(100),
	smooth_switch_(false),
	center_offset_(0, 0) {
}

bool qdCameraMode::load_script(const xml::tag *p) {
	for (xml::tag::subtag_iterator it = p -> subtags_begin(); it != p -> subtags_end(); ++it) {
		xml::tag_buffer buf(*it);

		switch (it -> ID()) {
		case QDSCR_TYPE:
			set_camera_mode(camera_mode_t(buf.get_int()));
			break;
		case QDSCR_TIME:
			set_work_time(buf.get_float());
			break;
		case QDSCR_CAMERA_SCROLLING_SPEED:
			set_scrolling_speed(buf.get_float());
			break;
		case QDSCR_CAMERA_SCROLLING_DIST:
			set_scrolling_distance(buf.get_int());
			break;
		case QDSCR_CAMERA_SMOOTH_SWITCH:
			smooth_switch_ = (buf.get_int()) ? true : false;
			break;
		case QDSCR_CAMERA_SCREEN_CENTER:
			buf > center_offset_.x > center_offset_.y;
			break;
		}
	}

	return true;
}

bool qdCameraMode::save_script(XStream &fh, int indent) const {
	for (int i = 0; i < indent; i++) fh < "\t";

	fh < "<camera_mode type=\"" <= (int)camera_mode() < "\"";

	fh < " scrolling_speed=\"" <= scrolling_speed_ < "\"";
	fh < " scrolling_dist=\"" <= scrolling_distance_ < "\"";

	if (center_offset_.x || center_offset_.y)
		fh < " camera_screen_center=\"" <= center_offset_.x < " " <= center_offset_.y < "\"";

	if (has_work_time())
		fh < " time=\"" <= work_time() < "\"";

	if (smooth_switch())
		fh < " smooth_switch=\"1\"";

	fh < "/>\r\n";

	return true;
}

bool qdCameraMode::load_data(qdSaveStream &fh, int save_version) {
	int mode;
	fh > mode > work_time_ > scrolling_speed_ > scrolling_distance_ > center_offset_.x > center_offset_.y;

	char switch_flag;
	fh > switch_flag;
	smooth_switch_ = (switch_flag) ? true : false;

	return true;
}

bool qdCameraMode::save_data(Common::SeekableWriteStream &fh) const {
	fh.writeSint32LE((int)camera_mode_);
	fh.writeFloatLE(work_time_);
	fh.writeFloatLE(scrolling_speed_);
	fh.writeSint32LE(scrolling_distance_);
	fh.writeSint32LE(center_offset_.x);
	fh.writeSint32LE(center_offset_.y);


	if (smooth_switch_) {
		fh.writeByte(1);
	} else {
		fh.writeByte(0);
	}

	return true;
}

bool qdCameraMode::save_data(qdSaveStream &fh) const {
	fh < int(camera_mode_) < work_time_ < scrolling_speed_ < scrolling_distance_ < center_offset_.x < center_offset_.y;

	if (smooth_switch_) fh < char(1);
	else fh < char(0);

	return true;
}
} // namespace QDEngine
