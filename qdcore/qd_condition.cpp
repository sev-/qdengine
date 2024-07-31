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

#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "common/debug.h"

#include "qdengine/qdengine.h"
#include "qdengine/qd_precomp.h"
#include "qdengine/parser/qdscr_parser.h"
#include "qdengine/parser/xml_tag_buffer.h"
#include "qdengine/qdcore/qd_rnd.h"
#include "qdengine/qdcore/qd_condition.h"
#include "qdengine/qdcore/qd_game_dispatcher.h"

namespace Common {
class WriteStream;
}

namespace QDEngine {

bool qdCondition::successful_click_ = false;
bool qdCondition::successful_object_click_ = false;

qdCondition::qdCondition() : type_(CONDITION_FALSE), is_inversed_(false)
#ifndef _QUEST_EDITOR
	, is_in_group_(false)
#endif // _QUEST_EDITOR
{
}

qdCondition::qdCondition(qdCondition::ConditionType tp) : is_inversed_(false)
#ifndef _QUEST_EDITOR
	, is_in_group_(false)
#endif // _QUEST_EDITOR
{
	set_type(tp);
}

qdCondition::qdCondition(const qdCondition &cnd) : type_(cnd.type_),
	owner_(cnd.owner_),
	data_(cnd.data_),
	objects_(cnd.objects_),
	is_inversed_(cnd.is_inversed_)
#ifndef _QUEST_EDITOR
	, is_in_group_(false)
#endif // _QUEST_EDITOR
{
}

qdCondition &qdCondition::operator = (const qdCondition &cnd) {
	if (this == &cnd) return *this;

	type_ = cnd.type_;
	owner_ = cnd.owner_;

	data_ = cnd.data_;
	objects_ = cnd.objects_;

	is_inversed_ = cnd.is_inversed_;

	return *this;
}

qdCondition::~qdCondition() {
}

void qdCondition::set_type(ConditionType tp) {
	type_ = tp;

	switch (type_) {
	case CONDITION_TRUE:
	case CONDITION_FALSE:
		break;
	case CONDITION_MOUSE_CLICK:
		data_.resize(1);
		objects_.resize(1);
		init_data(0, qdConditionData::DATA_STRING);
		break;
	case CONDITION_MOUSE_OBJECT_CLICK:
		data_.resize(2);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		break;
	case CONDITION_OBJECT_IN_ZONE:
		data_.resize(2);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		break;
	case CONDITION_PERSONAGE_WALK_DIRECTION:
	case CONDITION_PERSONAGE_STATIC_DIRECTION:
		data_.resize(2);
		objects_.resize(1);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_FLOAT, 1);
		break;
	case CONDITION_TIMER:
		data_.resize(2);
		init_data(0, qdConditionData::DATA_FLOAT, 2);
		init_data(1, qdConditionData::DATA_INT, 2);
		break;
	case CONDITION_MOUSE_DIALOG_CLICK:
		break;
	case CONDITION_MINIGAME_STATE:
		data_.resize(2);
		objects_.resize(1);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		break;
	case CONDITION_OBJECT_STATE:
	case CONDITION_OBJECT_PREV_STATE:
		data_.resize(2);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		break;
	case CONDITION_OBJECT_NOT_IN_STATE:
		inverse();
		type_ = CONDITION_OBJECT_STATE;

		data_.resize(2);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		break;
	case CONDITION_MOUSE_ZONE_CLICK:
		data_.resize(1);
		objects_.resize(1);
		init_data(0, qdConditionData::DATA_STRING);
		break;
	case CONDITION_MOUSE_OBJECT_ZONE_CLICK:
		data_.resize(2);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		break;
	case CONDITION_OBJECT_STATE_WAS_ACTIVATED:
		data_.resize(2);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		break;
	case CONDITION_OBJECT_STATE_WAS_NOT_ACTIVATED:
		inverse();
		type_ = CONDITION_OBJECT_STATE_WAS_ACTIVATED;

		data_.resize(2);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		break;
	case CONDITION_OBJECTS_DISTANCE:
		data_.resize(3);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		init_data(2, qdConditionData::DATA_FLOAT, 1);
		break;
	case CONDITION_PERSONAGE_ACTIVE:
		data_.resize(1);
		objects_.resize(1);
		init_data(0, qdConditionData::DATA_STRING);
		break;
	case CONDITION_OBJECT_STATE_WAITING:
		data_.resize(2);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		break;
	case CONDITION_OBJECT_STATE_ANIMATION_PHASE:
		data_.resize(3);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_STRING);
		init_data(1, qdConditionData::DATA_STRING);
		init_data(2, qdConditionData::DATA_FLOAT, 2);
		break;
	case CONDITION_STATE_TIME_GREATER_THAN_VALUE:
		data_.resize(1);
		objects_.resize(1);
		init_data(0, qdConditionData::DATA_FLOAT, 1);
		break;
	case CONDITION_STATE_TIME_GREATER_THAN_STATE_TIME:
		objects_.resize(2);
		break;
	case CONDITION_STATE_TIME_IN_INTERVAL:
		data_.resize(1);
		objects_.resize(1);
		init_data(0, qdConditionData::DATA_FLOAT, 2);
		break;
	case CONDITION_COUNTER_GREATER_THAN_VALUE:
	case CONDITION_COUNTER_LESS_THAN_VALUE:
		data_.resize(1);
		objects_.resize(1);
		init_data(0, qdConditionData::DATA_INT, 1);
		break;
	case CONDITION_COUNTER_GREATER_THAN_COUNTER:
		objects_.resize(2);
		break;
	case CONDITION_COUNTER_IN_INTERVAL:
		data_.resize(1);
		objects_.resize(1);
		init_data(0, qdConditionData::DATA_INT, 2);
		break;
	case CONDITION_OBJECT_ON_PERSONAGE_WAY:
		data_.resize(1);
		objects_.resize(2);
		init_data(0, qdConditionData::DATA_FLOAT, 1);
		break;
	case CONDITION_KEYPRESS:
		data_.resize(1);
		init_data(0, qdConditionData::DATA_INT, 1);
		break;
	case CONDITION_ANY_PERSONAGE_IN_ZONE:
		objects_.resize(1);
		break;
	case CONDITION_MOUSE_RIGHT_CLICK:
		objects_.resize(1);
		break;
	case CONDITION_MOUSE_RIGHT_OBJECT_CLICK:
		objects_.resize(2);
		break;
	case CONDITION_MOUSE_RIGHT_ZONE_CLICK:
		objects_.resize(1);
		break;
	case CONDITION_MOUSE_RIGHT_OBJECT_ZONE_CLICK:
		objects_.resize(2);
		break;
	case CONDITION_OBJECT_HIDDEN:
		objects_.resize(1);
		break;
	case CONDITION_MOUSE_HOVER:
		objects_.resize(1);
		break;
	case CONDITION_MOUSE_OBJECT_HOVER:
		objects_.resize(2);
		break;
	case CONDITION_MOUSE_HOVER_ZONE:
		objects_.resize(1);
		break;
	case CONDITION_MOUSE_OBJECT_HOVER_ZONE:
		objects_.resize(2);
		break;
	case CONDITION_MOUSE_CLICK_FAILED:
	case CONDITION_MOUSE_OBJECT_CLICK_FAILED:
	case CONDITION_MOUSE_CLICK_EVENT:
	case CONDITION_MOUSE_RIGHT_CLICK_EVENT:
		break;
	case CONDITION_MOUSE_OBJECT_CLICK_EVENT:
	case CONDITION_MOUSE_RIGHT_OBJECT_CLICK_EVENT:
	case CONDITION_MOUSE_STATE_PHRASE_CLICK:
		objects_.resize(1);
		break;
	case CONDITION_OBJECT_IS_CLOSER:
		objects_.resize(3);
		break;
	case CONDITION_ANIMATED_OBJECT_IDLE_GREATER_THAN_VALUE:
		objects_.resize(1);
		data_.resize(1);
		init_data(0, qdConditionData::DATA_INT, 1);
		break;
	case CONDITION_ANIMATED_OBJECTS_INTERSECTIONAL_BOUNDS:
		objects_.resize(2);
		break;
	}
}

bool qdCondition::put_value(int idx, const char *str) {
	assert(idx >= 0 && idx < data_.size());
	return data_[idx].put_string(str);
}

bool qdCondition::put_value(int idx, int val, int val_index) {
	assert(idx >= 0 && idx < data_.size());
	return data_[idx].put_int(val, val_index);
}

bool qdCondition::put_value(int idx, float val, int val_index) {
	assert(idx >= 0 && idx < data_.size());
	return data_[idx].put_float(val, val_index);
}

bool qdCondition::get_value(int idx, const char *&str) const {
	assert(idx >= 0 && idx < data_.size());

	if (data_[idx].get_string()) {
		str = data_[idx].get_string();
		return true;
	}

	return false;
}

bool qdCondition::get_value(int idx, int &val, int val_index) const {
	assert(idx >= 0 && idx < data_.size());
	val = data_[idx].get_int(val_index);

	return true;
}

bool qdCondition::get_value(int idx, float &val, int val_index) const {
	assert(idx >= 0 && idx < data_.size());
	val = data_[idx].get_float(val_index);

	return true;
}

bool qdCondition::load_script(const xml::tag *p) {
	int data_idx = 0;
	for (xml::tag::subtag_iterator it = p->subtags_begin(); it != p->subtags_end(); ++it) {
		switch (it->ID()) {
		case QDSCR_CONDITION_DATA_INT:
		case QDSCR_CONDITION_DATA_FLOAT:
		case QDSCR_CONDITION_DATA_STRING:
			if (data_idx < data_.size())
				data_[data_idx++].load_script(&*it);
			break;
		case QDSCR_CONDITION_INVERSE:
			if (xml::tag_buffer(*it).get_int())
				inverse(true);
			else
				inverse(false);
			break;
		case QDSCR_CONDITION_OBJECT:
			if (const xml::tag * tp = it->search_subtag(QDSCR_ID)) {
				int object_idx = xml::tag_buffer(*tp).get_int();

				if (object_idx >= 0 && object_idx < objects_.size())
					objects_[object_idx].load_script(&*it);
			}
			break;
		}
	}
	return true;
}

bool qdCondition::save_script(Common::WriteStream &fh, int indent) const {
	for (int i = 0; i < indent; i++) {
		fh.writeString("\t");
	}

	fh.writeString(Common::String::format("<condition type=\"%d\"", type_));

	if (is_inversed()) {
		fh.writeString("condition_inverse=\"1\"");
	}

	fh.writeString(">\r\n");

	for (auto &it : data_) {
		it.save_script(fh, indent + 1);
	}

	for (int i = 0; i < objects_.size(); i++) {
		if (objects_[i].object())
			objects_[i].save_script(fh, indent + 1, i);
	}

	for (int i = 0; i < indent; i++) {
		fh.writeString("\t");
	}

	fh.writeString("</condition>\r\n");
	return true;
}

void qdCondition::quant(float dt) {
	debugC(9, kDebugQuant, "qdCondition::quant(%f)", dt);
	if (type_ == CONDITION_TIMER) {
		float period, timer;
		if (!get_value(TIMER_PERIOD, period, 0)) return;
		if (!get_value(TIMER_PERIOD, timer, 1)) return;

		timer += dt;

		put_value(TIMER_PERIOD, timer, 1);

		if (timer >= period) {
			debugC(3, kDebugQuant, "qdCondition::quant() timer >= period");
			timer -= period;
			put_value(TIMER_PERIOD, timer, 1);

			int rnd;
			if (!get_value(TIMER_RND, rnd)) return;

			int state = 1;
			if (rnd && qd_rnd(100 - rnd))
				state = 0;

			put_value(TIMER_RND, state, 1);
		} else
			put_value(TIMER_RND, 0, 1);
	}
}

bool qdCondition::load_data(Common::SeekableReadStream &fh, int save_version) {
	debugC(5, kDebugSave, "      qdCondition::load_data(): before %ld", fh.pos());
	if (type_ == CONDITION_TIMER) {
		int state;
		float timer;

		timer = fh.readFloatLE();
		state = fh.readSint32LE();

		if (!put_value(TIMER_PERIOD, timer, 1)) return false;
		if (!put_value(TIMER_RND, state, 1)) return false;
	}

	debugC(5, kDebugSave, "      qdCondition::load_data(): after %ld", fh.pos());
	return true;
}

bool qdCondition::save_data(Common::WriteStream &fh) const {
	debugC(5, kDebugSave, "      qdCondition::save_data(): before %ld", fh.pos());
	if (type_ == CONDITION_TIMER) {
		float timer;
		if (!get_value(TIMER_PERIOD, timer, 1)) {
			return false;
		}

		int state;
		if (!get_value(TIMER_RND, state, 1)) {
			return false;
		}

		fh.writeFloatLE(timer);
		fh.writeSint32LE(state);
	}

	debugC(5, kDebugSave, "      qdCondition::save_data(): after %ld", fh.pos());
	return true;
}

bool qdCondition::check() {
	bool result = false;
	if (qdGameDispatcher * dp = qdGameDispatcher::get_dispatcher()) {
		if (dp->check_condition(this))
			result = !is_inversed_;
		else
			result = is_inversed_;
	}

	if (result) {
		if (is_click_condition())
			successful_click_ = true;
		else if (is_object_click_condition())
			successful_object_click_ = true;
	}

	return result;
}

bool qdCondition::put_object(int idx, const qdNamedObject *obj) {
	assert(idx >= 0 && idx < objects_.size());
	objects_[idx].set_object(obj);
	return true;
}

const qdNamedObject *qdCondition::get_object(int idx) {
	if (idx >= 0 && idx < objects_.size()) {
		if (!objects_[idx].object())
			objects_[idx].find_object();

		return objects_[idx].object();
	}

	return NULL;
}


bool qdCondition::init() {
	if (type_ == CONDITION_TIMER) {
		if (!put_value(TIMER_PERIOD, 0.0f, 1)) return false;
		if (!put_value(TIMER_RND, 0, 1)) return false;
	}
	return true;
}
} // namespace QDEngine
