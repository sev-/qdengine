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
#include "common/stream.h"

#include "qdengine/qd_precomp.h"
#include "qdengine/parser/qdscr_parser.h"
#include "qdengine/parser/xml_tag_buffer.h"
#include "qdengine/qdcore/qd_game_dispatcher.h"
#include "qdengine/qdcore/qd_inventory_cell.h"
#include "qdengine/qdcore/qd_interface_element.h"
#include "qdengine/qdcore/qd_named_object_reference.h"
#include "qdengine/qdcore/qd_game_object_animated.h"

namespace QDEngine {

// class qdInventoryCellType

bool qdInventoryCellType::load_resources() const {
	return sprite_.load();
}

void qdInventoryCellType::free_resources() const {
	sprite_.free();
}

bool qdInventoryCellType::load_script(const xml::tag *p) {
	for (xml::tag::subtag_iterator it = p->subtags_begin(); it != p->subtags_end(); ++it) {
		switch (it->ID()) {
		case QDSCR_TYPE:
			set_type(xml::tag_buffer(*it).get_int());
			break;
		case QDSCR_FILE:
			sprite_.set_file(Common::Path(it->data(), '\\').toString().c_str());
			break;
		}
	}

	return true;
}

bool qdInventoryCellType::save_script(Common::WriteStream &fh, int indent) const {
	for (int i = 0; i < indent; i++) {
		fh.writeString("\t");
	}

	fh.writeString(Common::String::format("<inventory_cell_type type=\"%d\"", type_));
	if (strlen(sprite_.file())) {
		fh.writeString(Common::String::format(" file=\"%s\"", qdscr_XML_string(sprite_.file())));
	}
	fh.writeString("/>\r\n");

	return true;
}

// class qdInventoryCell

Vect2i qdInventoryCell::screen_offset_ = Vect2i(0, 0);
unsigned qdInventoryCell::shadow_color_ = 0;
int qdInventoryCell::shadow_alpha_ = -1;

qdInventoryCell::qdInventoryCell() : type_(0),
#ifndef _QUEST_EDITOR
	sprite_(NULL),
#endif
	object_(NULL) {
}

qdInventoryCell::qdInventoryCell(const qdInventoryCellType &tp) : type_(tp.type()),
#ifndef _QUEST_EDITOR
	sprite_(tp.sprite()),
#endif // _QUEST_EDITOR
	object_(NULL) {
}

qdInventoryCell::qdInventoryCell(const qdInventoryCell &cl) : type_(cl.type_),
#ifndef _QUEST_EDITOR
	sprite_(cl.sprite_),
#endif // _QUEST_EDITOR
	object_(cl.object_) {
}

qdInventoryCell &qdInventoryCell::operator = (const qdInventoryCell &cl) {
	if (this == &cl) return *this;

	type_ = cl.type_;
#ifndef _QUEST_EDITOR
	sprite_ = cl.sprite_;
#endif // _QUEST_EDITOR
	object_ = cl.object_;

	return *this;
}

#ifdef _QUEST_EDITOR
const qdSprite *qdInventoryCell::sprite() const {
	qdGameDispatcher *p = qd_get_game_dispatcher();
	if (p) {
		const qdInventoryCellType *ct = p->get_inventory_cell_type(type_);
		if (ct) return ct->sprite();
	}

	return NULL;
}
#endif

void qdInventoryCell::redraw(int x, int y, bool inactive_mode) const {
	if (sprite())
		sprite()->redraw(x, y, 0);

	if (object_) {
		object_->set_pos(Vect3f(x, y, 0));
		object_->set_flag(QD_OBJ_SCREEN_COORDS_FLAG);
		object_->update_screen_pos();
		object_->redraw();

		if (inactive_mode)
			object_->draw_shadow(0, 0, shadow_color_, shadow_alpha_);
	}
}

void qdInventoryCell::set_object(qdGameObjectAnimated *obj) {
	object_ = obj;
	if (object_) object_->set_flag(QD_OBJ_SCREEN_COORDS_FLAG);
}

bool qdInventoryCell::load_resources() {
	if (object_)
		return object_->load_resources();
	return true;
}

bool qdInventoryCell::free_resources() {
	if (object_) object_->free_resources();
	return true;
}

bool qdInventoryCell::load_data(Common::SeekableReadStream &fh, int saveVersion) {
	debugC(5, kDebugSave, "      qdInventoryCell::load_data before: %ld", fh.pos());
	char flag = fh.readByte();

	if (flag) {
		qdNamedObjectReference ref;
		if (!ref.load_data(fh, saveVersion)) {
			return false;
		}

		if (qdGameDispatcher * p = qdGameDispatcher::get_dispatcher()) {
			object_ = static_cast<qdGameObjectAnimated *>(p->get_named_object(&ref));
		}
	} else
		object_ = NULL;

	debugC(5, kDebugSave, "      qdInventoryCell::load_data after: %ld", fh.pos());
	return true;
}

bool qdInventoryCell::save_data(Common::WriteStream &fh) const {
	debugC(5, kDebugSave, "      qdInventoryCell::save_data before: %ld", fh.pos());
	if (object_) {
		fh.writeByte(1);

		qdNamedObjectReference ref(object_);
		if (!ref.save_data(fh)) {
			return false;
		}
	} else
		fh.writeByte(0);

	debugC(5, kDebugSave, "      qdInventoryCell::save_data after: %ld", fh.pos());
	return true;
}

// class qdInventoryCellSet

qdInventoryCellSet::qdInventoryCellSet() : _size(0, 0),
	_screen_pos(0, 0),
	_additional_cells(0, 0),
	_cells_shift(0, 0) {
}

qdInventoryCellSet::qdInventoryCellSet(int x, int y, int sx, int sy, short addit_sx, short addit_sy, const qdInventoryCellType &tp) : _size(sx, sy),
	_screen_pos(x, y),
	_additional_cells(addit_sx, addit_sy),
	_cells_shift(0, 0),
	_cells((sx + addit_sx) * (sy + addit_sy), tp) {
}

qdInventoryCellSet::qdInventoryCellSet(const qdInventoryCellSet &set) : _size(set._size),
	_cells(set._cells),
	_screen_pos(set._screen_pos),
	_additional_cells(set._additional_cells),
	_cells_shift(set._cells_shift) {
}

qdInventoryCellSet::~qdInventoryCellSet() {
	_cells.clear();
}

qdInventoryCellSet &qdInventoryCellSet::operator = (const qdInventoryCellSet &set) {
	if (this == &set) return *this;

	_size = set._size;
	_cells = set._cells;
	_screen_pos = set._screen_pos;
	_additional_cells = set._additional_cells;
	_cells_shift = set._cells_shift;

	return *this;
}

void qdInventoryCellSet::redraw(int offs_x, int offs_y, bool inactive_mode) const {
	Vect2s pos = screen_pos();

	int idx;
	for (int i = _cells_shift.y; i < _size.y + _cells_shift.y; i++) {
		idx = i * (_size.x + _additional_cells.x) + _cells_shift.x;
		for (int j = _cells_shift.x; j < _size.x + _cells_shift.x; j++) {
			assert(idx >= 0 && idx < _cells.size());
			_cells[idx].redraw(offs_x + pos.x, offs_y + pos.y, inactive_mode);
			pos.x += _cells[idx].size_x();
			idx++;
		}
		pos.x = screen_pos().x;
		pos.y += _cells.front().size_y();
	}
}

bool qdInventoryCellSet::put_object(qdGameObjectAnimated *p) {
	for (auto &it : _cells) {
		if (it.is_empty() && it.type() == p->inventory_type()) {
			it.set_object(p);
			return true;
		}
	}

	return false;
}

bool qdInventoryCellSet::hit(const Vect2s &pos) const {
	if (_cells.empty() || !_cells.front().size_x() || !_cells.front().size_y())
		return false;

	Vect2s v = pos - screen_pos() + _cells.front().size() / 2;

	if (v.x >= 0 && v.x < _size.x * _cells.front().size_x() && v.y >= 0 && v.y < _size.y * _cells.front().size_y())
		return true;

	return false;
}

grScreenRegion qdInventoryCellSet::screen_region() const {
	if (_cells.empty() || !_cells.front().size_x() || !_cells.front().size_y())
		return grScreenRegion::EMPTY;

	int sx = _size.x * _cells.front().size_x();
	int sy = _size.y * _cells.front().size_y();

	int x = screen_pos().x - _cells.front().size_x() / 2 + sx / 2;
	int y = screen_pos().y - _cells.front().size_y() / 2 + sy / 2;

	return grScreenRegion(x, y, sx, sy);
}

bool qdInventoryCellSet::put_object(qdGameObjectAnimated *p, const Vect2s &pos) {
	if (!hit(pos)) return false;

	Vect2s v = pos - screen_pos() + _cells.front().size() / 2;
	int idx = v.x / _cells.front().size_x() + (v.y / _cells.front().size_y()) * _size.x;
	// Двигаем индекс на текущее смещение ячеек
	idx += _cells_shift.y * (_size.x + _additional_cells.x) + _cells_shift.x;

	if (idx >= 0 && idx < _cells.size() && _cells[idx].is_empty() && _cells[idx].type() == p->inventory_type()) {
		_cells[idx].set_object(p);
		return true;
	}

	return false;
}

qdGameObjectAnimated *qdInventoryCellSet::get_object(const Vect2s &pos) const {
	if (!hit(pos)) return NULL;

	Vect2s v = pos - screen_pos() + _cells.front().size() / 2;

	int idx = v.x / _cells.front().size_x() + (v.y / _cells.front().size_y()) * _size.x;
	// Двигаем индекс на текущее смещение ячеек
	idx += _cells_shift.y * (_size.x + _additional_cells.x) + _cells_shift.x;

	if (idx >= 0 && idx < _cells.size())
		return _cells[idx].object();

	return NULL;
}

bool qdInventoryCellSet::remove_object(qdGameObjectAnimated *p) {
	for (auto &it : _cells) {
		if (it.object() == p) {
			it.set_object(NULL);
			return true;
		}
	}

	return false;
}

bool qdInventoryCellSet::is_object_in_list(const qdGameObjectAnimated *p) const {
	for (auto &it : _cells) {
		if (it.object() == p)
			return true;
	}

	return false;
}

bool qdInventoryCellSet::load_script(const xml::tag *p) {
	Vect2s v;
	for (xml::tag::subtag_iterator it = p->subtags_begin(); it != p->subtags_end(); ++it) {
		switch (it->ID()) {
		case QDSCR_INVENTORY_CELL_SET_POS:
			xml::tag_buffer(*it) > v.x > v.y;
			set_screen_pos(v);
			break;
		case QDSCR_INVENTORY_CELL_SET_SIZE:
			xml::tag_buffer(*it) > v.x > v.y;
			set_size(v);
			break;
		case QDSCR_INVENTORY_CELL_SET_ADDITIONAL_CELLS:
			xml::tag_buffer(*it) > _additional_cells.x > _additional_cells.y;
			break;
		}
	}

	if (const xml::tag * tp = p->search_subtag(QDSCR_TYPE))
		set_cell_type(qdInventoryCellType(xml::tag_buffer(*tp).get_int()));

	return true;
}

bool qdInventoryCellSet::save_script(Common::WriteStream &fh, int indent) const {
	int tp = 0;
	if (!_cells.empty()) {
		tp = _cells.front().type();
	}

	for (int i = 0; i < indent; i++) {
		fh.writeString("\t");
	}
	fh.writeString(Common::String::format("<inventory_cell_set type=\"%d\">\r\n", tp));

	for (int i = 0; i <= indent; i++) {
		fh.writeString("\t");
	}
	fh.writeString(Common::String::format("<inventory_cell_set_size>%d %d</inventory_cell_set_size>\r\n", _size.x, _size.y));

	for (int i = 0; i <= indent; i++) {
		fh.writeString("\t");
	}
	fh.writeString(Common::String::format("<inventory_cell_set_pos>%d %d</inventory_cell_set_pos>\r\n", _screen_pos.x, _screen_pos.y));

	for (int i = 0; i <= indent; i++) {
		fh.writeString("\t");
	}
	fh.writeString(Common::String::format("<inventory_cell_set_additional_cells>%d %d</inventory_cell_set_additional_cells>\r\n", _additional_cells.x, _additional_cells.y));

	for (int i = 0; i < indent; i++) {
		fh.writeString("\t");
	}
	fh.writeString("</inventory_cell_set>\r\n");

	return true;
}

bool qdInventoryCellSet::init(const qdInventoryCellTypeVector &tp) {
	if (_cells.empty()) return false;

	for (auto &it : tp) {
		if (it.type() == _cells.front().type()) {
			set_cell_type(it);
			return true;
		}
	}

	return false;
}

#ifdef _QUEST_EDITOR
const Vect2i qdInventoryCellSet::screen_size() const {
	if (_cells.empty()) return Vect2i(0, 0);
	const qdInventoryCell &cell = _cells.front();
	return Vect2i(_size.x * cell.size_x(), _size.y * cell.size_y());
}

const Vect2i qdInventoryCellSet::cell_size() const {
	if (_cells.empty()) return Vect2i(0, 0);
	return _cells.front().size();
}
#endif // _QUEST_EDITOR

bool qdInventoryCellSet::load_resources() {
	for (auto &it : _cells)
		it.load_resources();

	return true;
}

bool qdInventoryCellSet::free_resources() {
	for (auto &it : _cells) {
		it.free_resources();
	}

	return true;
}

bool qdInventoryCellSet::load_data(Common::SeekableReadStream &fh, int save_version) {
	debugC(4, kDebugSave, "    qdInventoryCellSet::load_data before: %ld", fh.pos());
	if (save_version >= 102) {
		_additional_cells.x = fh.readSint32LE();
		_additional_cells.y = fh.readSint32LE();
	}
	for (auto &it : _cells) {
		if (!it.load_data(fh, save_version))
			return false;
	}

	debugC(4, kDebugSave, "    qdInventoryCellSet::load_data after: %ld", fh.pos());
	return true;
}

bool qdInventoryCellSet::save_data(Common::WriteStream &fh) const {
	debugC(4, kDebugSave, "    qdInventoryCellSet::save_data before: %ld", fh.pos());
	fh.writeSint32LE(_additional_cells.x);
	fh.writeSint32LE(_additional_cells.y);

	for (auto &it : _cells) {
		if (!it.save_data(fh))
			return false;
	}

	debugC(4, kDebugSave, "    qdInventoryCellSet::save_data after: %ld", fh.pos());
	return true;
}

int qdInventoryCellSet::cell_index(const qdGameObjectAnimated *obj) const {
	for (auto it = _cells.begin(); it != _cells.end(); it++) {
		if (it->object() == obj) {
			return (it - _cells.begin());
		}
	}

	return -1;
}

Vect2s qdInventoryCellSet::cell_position(int cell_idx) const {
	if (cell_idx >= 0 && cell_idx < _cells.size()) {
		int x = (cell_idx % _size.x) * _cells.front().size_x() + screen_pos().x;
		int y = (cell_idx / _size.x) * _cells.front().size_y() + screen_pos().y;
		// Делаем поправку на смещение ячеек
		x -= _cells.front().size_x() * _cells_shift.x;
		y -= _cells.front().size_y() * _cells_shift.y;
		return Vect2s(x, y);
	}

	return Vect2s(0, 0);
}

void qdInventoryCellSet::set_mouse_hover_object(qdGameObjectAnimated *obj) {
	for (auto &it : _cells) {
		if (it.object() && it.object() != obj && it.object()->get_cur_state()) {
			if (it.object()->get_cur_state()->check_flag(qdGameObjectState::QD_OBJ_STATE_FLAG_MOUSE_HOVER_STATE)) {
				if (qdGameObjectState *sp = it.object()->get_inventory_state())
					it.object()->set_state(sp);
			}
		}
	}

	if (obj) {
		if (qdGameObjectState * p = obj->get_mouse_hover_state()) {
			if (obj->get_cur_state() != p)
				p->set_prev_state(obj->get_cur_state());
			obj->set_state(p);
		}
	}
}

void qdInventoryCellSet::objects_quant(float dt) {
	for (auto &it : _cells) {
		if (!it.is_empty()) {
			it.object()->quant(dt);
		}
	}
}

void qdInventoryCellSet::pre_redraw() const {
	qdGameDispatcher *dp = qdGameDispatcher::get_dispatcher();
	if (!dp) return;

	int idx;
	for (int i = _cells_shift.y; i < size().y + _cells_shift.y; i++) {
		idx = i * (_size.x + _additional_cells.x) + _cells_shift.x;
		for (int j = _cells_shift.x; j < size().x + _cells_shift.x; j++) {
			assert(idx >= 0 && idx < _cells.size());
			if (!_cells[idx].is_empty() && _cells[idx].object()->need_redraw()) {
				dp->add_redraw_region(_cells[idx].object()->last_screen_region());
				dp->add_redraw_region(_cells[idx].object()->screen_region());
			}
			idx++;
		}
	}

	grScreenRegion reg = screen_region();
	if (reg != _last_screen_region) {
		dp->add_redraw_region(_last_screen_region);
		dp->add_redraw_region(reg);
	}
}

void qdInventoryCellSet::post_redraw() {
	int idx;
	for (int i = _cells_shift.y; i < size().y + _cells_shift.y; i++) {
		idx = i * (_size.x + _additional_cells.x) + _cells_shift.x;
		for (int j = _cells_shift.x; j < size().x + _cells_shift.x; j++) {
			assert(idx >= 0 && idx < _cells.size());
			if (!_cells[idx].is_empty())
				_cells[idx].object()->post_redraw();
			idx++;
		}
	}

	_last_screen_region = screen_region();
}

bool qdInventoryCellSet::has_rect_objects(int left, int top, int right, int bottom) const {
	int idx;
	for (int i = top; i <= bottom; i++)
		for (int j = left; j <= right; j++) {
			idx = i * (_size.x + _additional_cells.x) + j;
			assert(idx >= 0 && idx < _cells.size());
			// Нашли объект вне видимой области - значит скроллинг нужен
			if (!_cells[idx].is_empty())
				return true;
		}
	return false;
}

void qdInventoryCellSet::scroll_left() {
	if (!has_rect_objects(0, 0, _cells_shift.x - 1, _size.y + _additional_cells.y - 1))
		return;
	_cells_shift.x--;
	if (_cells_shift.x < 0) _cells_shift.x = _additional_cells.x;
}

void qdInventoryCellSet::scroll_right() {
	if (!has_rect_objects(_cells_shift.x + _size.x, 0, _size.x + _additional_cells.x - 1, _size.y + _additional_cells.y - 1))
		return;
	_cells_shift.x++;
	if (_cells_shift.x > _additional_cells.x) _cells_shift.x = 0;
}

void qdInventoryCellSet::scroll_up() {
	if (!has_rect_objects(0, 0, _size.x + _additional_cells.x - 1, _cells_shift.y - 1))
		return;
	_cells_shift.y--;
	if (_cells_shift.y < 0) _cells_shift.y = _additional_cells.y;
}

void qdInventoryCellSet::scroll_down() {
	if (!has_rect_objects(0, _cells_shift.y + _size.y, _size.x + _additional_cells.x - 1, _size.y + _additional_cells.y - 1))
		return;
	_cells_shift.y++;
	if (_cells_shift.y > _additional_cells.y) _cells_shift.y = 0;
}

void qdInventoryCellSet::debug_log() const {
#ifdef _DEBUG
	for (int i = _cells_shift.y; i < size().y + _cells_shift.y; i++) {
		int idx = i * (_size.x + _additional_cells.x) + _cells_shift.x;
		for (int j = _cells_shift.x; j < size().x + _cells_shift.x; j++) {
			if (!_cells[idx].is_empty()) {
				debugC(3, kDebugLog, "Inventory cell: %d %d %s", i, j, transCyrillic(_cells[idx].object()->name()));
			}
		}
	}
#endif
}
} // namespace QDEngine
