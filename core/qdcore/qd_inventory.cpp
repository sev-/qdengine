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
#include <algorithm>
#include "common/stream.h"
#include "qdengine/core/qd_precomp.h"
#include "qdengine/qdengine.h"

#ifdef _QUEST_EDITOR
#include <functional>
#endif // _QUEST_EDITOR

#include "qdengine/core/parser/xml_tag_buffer.h"

#include "qdengine/core/parser/qdscr_parser.h"
#include "qdengine/core/qdcore/qd_inventory.h"
#include "qdengine/core/qdcore/qd_game_scene.h"
#include "qdengine/core/qdcore/qd_game_dispatcher.h"
#include "qdengine/core/qdcore/qd_game_object_mouse.h"


namespace QDEngine {


/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

qdInventory::qdInventory() : need_redraw_(false),
	shadow_color_(INV_DEFAULT_SHADOW_COLOR),
	shadow_alpha_(INV_DEFAULT_SHADOW_ALPHA),
	additional_cells_(0, 0) {
#ifdef _QUEST_EDITOR
	m_iActiveCellset = -1;
#endif // _QUEST_EDITOR
}

qdInventory::~qdInventory() {
	cell_sets_.clear();
}

void qdInventory::redraw(int offs_x, int offs_y, bool inactive_mode) const {
	qdInventoryCell::set_shadow(shadow_color_, shadow_alpha_);

	for (qdInventoryCellSetVector::const_iterator it = cell_sets_.begin(); it != cell_sets_.end(); ++it)
		it -> redraw(offs_x, offs_y, inactive_mode);
}

void qdInventory::pre_redraw() const {
	for (qdInventoryCellSetVector::const_iterator it = cell_sets_.begin(); it != cell_sets_.end(); ++it)
		it -> pre_redraw();

	if (need_redraw_) {
		if (qdGameDispatcher * dp = qdGameDispatcher::get_dispatcher()) {
			for (qdInventoryCellSetVector::const_iterator it = cell_sets_.begin(); it != cell_sets_.end(); ++it) {
				dp -> add_redraw_region(it -> screen_region());
				dp -> add_redraw_region(it -> last_screen_region());
			}
		}
	}
}

void qdInventory::post_redraw() {
	toggle_redraw(false);

	for (qdInventoryCellSetVector::iterator it = cell_sets_.begin(); it != cell_sets_.end(); ++it)
		it -> post_redraw();
}

bool qdInventory::put_object(qdGameObjectAnimated *p) {
	if (p -> inventory_cell_index() != -1) {
		if (put_object(p, cell_position(p -> inventory_cell_index())))
			return true;
	}

	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it) {
		if (it -> put_object(p)) {
			p -> set_inventory_cell_index(cell_index(p));
			p -> set_flag(QD_OBJ_IS_IN_INVENTORY_FLAG);
			need_redraw_ = true;
			return true;
		}
	}

	return false;
}

bool qdInventory::put_object(qdGameObjectAnimated *p, const Vect2s &pos) {
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it) {
		if (it -> put_object(p, pos)) {
			p -> set_inventory_cell_index(cell_index(p));
			p -> set_flag(QD_OBJ_IS_IN_INVENTORY_FLAG);
			need_redraw_ = true;
			return true;
		}
	}

	return false;
}

qdGameObjectAnimated *qdInventory::get_object(const Vect2s &pos) const {
	qdInventoryCellSetVector::const_iterator it;
	FOR_EACH(cell_sets_, it) {
		if (it -> hit(pos)) {
			qdGameObjectAnimated *p = it -> get_object(pos);
			if (p) return p;
		}
	}

	return NULL;
}

bool qdInventory::remove_object(qdGameObjectAnimated *p) {
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it) {
		if (it -> remove_object(p)) {
			p -> drop_flag(QD_OBJ_IS_IN_INVENTORY_FLAG);
			need_redraw_ = true;
			return true;
		}
	}

	return false;
}

bool qdInventory::is_object_in_list(const qdGameObjectAnimated *p) const {
	qdInventoryCellSetVector::const_iterator it;
	FOR_EACH(cell_sets_, it) {
		if (it -> is_object_in_list(p))
			return true;
	}

	return false;
}

bool qdInventory::load_script(const xml::tag *p) {
	qdInventoryCellSet set;

	for (xml::tag::subtag_iterator it = p -> subtags_begin(); it != p -> subtags_end(); ++it) {
		switch (it -> ID()) {
		case QDSCR_NAME:
			set_name(it -> data());
			break;
		case QDSCR_FLAG:
			set_flag(xml::tag_buffer(*it).get_int());
			break;
		case QDSCR_INVENTORY_CELL_SET:
			set.load_script(&*it);
			add_cell_set(set);
			break;
		case QDSCR_GRID_ZONE_SHADOW_COLOR:
			xml::tag_buffer(*it) > shadow_color_;
			break;
		case QDSCR_GRID_ZONE_SHADOW_ALPHA:
			xml::tag_buffer(*it) > shadow_alpha_;
			break;
		case QDSCR_INVENTORY_CELL_SET_ADDITIONAL_CELLS:
			xml::tag_buffer(*it) > additional_cells_.x > additional_cells_.y;
			break;
		}
	}

	return true;
}
bool qdInventory::save_script(Common::SeekableWriteStream &fh, int indent) const {
	warning("qdInventory::save_script(Common::SeekableStream &fh)");
	return true;
}

bool qdInventory::save_script(class XStream &fh, int indent) const {
	for (int i = 0; i < indent; i ++) fh < "\t";

	fh < "<inventory name=";

	if (name()) fh < "\"" < qdscr_XML_string(name()) < "\"";
	else fh < "\" \"";

	if (flags()) fh < " flags=\"" <= flags() < "\"";

	if (shadow_color_ != INV_DEFAULT_SHADOW_COLOR)
		fh < " shadow_color=\"" <= shadow_color_ < "\"";

	if (shadow_alpha_ != INV_DEFAULT_SHADOW_ALPHA)
		fh < " shadow_alpha=\"" <= shadow_alpha_ < "\"";

	fh < ">\r\n";

	for (int i = 0; i <= indent; i ++) fh < "\t";
	fh < "<inventory_cell_set_additional_cells>" <= additional_cells_.x < " " <= additional_cells_.y < "</inventory_cell_set_additional_cells>\r\n";

	qdInventoryCellSetVector::const_iterator it;
	FOR_EACH(cell_sets_, it)
	it -> save_script(fh, indent + 1);

	for (int i = 0; i < indent; i ++) fh < "\t";
	fh < "</inventory>\r\n";

	return true;
}

bool qdInventory::init(const qdInventoryCellTypeVector &tp) {
	bool result = true;
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it) {
		if (!it -> init(tp))
			result = false;
		it -> set_additional_cells(additional_cells_);
	}
	return result;
}

bool qdInventory::mouse_handler(int x, int y, mouseDispatcher::mouseEvent ev) {
	if (ev == mouseDispatcher::EV_LEFT_DOWN) {
		if (qdGameDispatcher * dp = qdGameDispatcher::get_dispatcher()) {
			bool ret_val = false;
			bool obj_flag = false;
			qdGameObjectAnimated *obj = get_object(Vect2s(x, y));

			qdGameObjectMouse *mp = dp -> mouse_object();
			if (mp -> object()) {
				if (!obj) {
					if (put_object(mp -> object(), mp -> screen_pos())) {
						mp -> take_object(NULL);
						ret_val = true;
						obj_flag = true;
					}
				}
			} else {
				if (obj) {
					remove_object(obj);
					mp -> take_object(obj);
					ret_val = true;
					obj_flag = true;
				}
			}
			if (obj) {
				if (qdGameScene * sp = dp -> get_active_scene()) {
					sp -> set_mouse_click_object(obj);
					ret_val = true;
				}
			}

			if (!obj_flag && check_flag(INV_PUT_OBJECT_AFTER_CLICK) && mp -> object()) {
				if (put_object(mp -> object()))
					mp -> take_object(NULL);
			}

			return ret_val;
		}
	} else if (ev == mouseDispatcher::EV_RIGHT_DOWN) {
		if (check_flag(INV_ENABLE_RIGHT_CLICK)) {
			if (qdGameDispatcher * dp = qd_get_game_dispatcher()) {
				qdGameObjectMouse *mp = dp -> mouse_object();
				if (mp -> object()) {
					if (put_object(mp -> object())) {
						mp -> take_object(NULL);
						return true;
					}
				}
			}
		}
		if (qdGameDispatcher * dp = qdGameDispatcher::get_dispatcher()) {
			if (qdGameObjectAnimated * obj = get_object(Vect2s(x, y))) {
				if (qdGameScene * sp = dp -> get_active_scene()) {
					sp -> set_mouse_right_click_object(obj);
					return true;
				}
			}
		}
	} else if (ev == mouseDispatcher::EV_MOUSE_MOVE) {
		qdGameObjectAnimated *obj = get_object(Vect2s(x, y));

		if (obj) {
			if (qdGameDispatcher * dp = qdGameDispatcher::get_dispatcher()) {
				if (qdGameScene * sp = dp -> get_active_scene()) {
					sp -> set_mouse_hover_object(obj);
				}
			}
		}

		qdInventoryCellSetVector::iterator it;
		FOR_EACH(cell_sets_, it)
		it -> set_mouse_hover_object(obj);
	}

	return false;
}

#ifdef _QUEST_EDITOR
int qdInventory::hit(int x, int y) {
	Vect2s pt(x, y);
	typedef qdInventoryCellSetVector::iterator i_t;
	i_t i = cell_sets_.begin(), e = cell_sets_.end();
	for (; i != e; ++i) {
		if (i->hit(pt))
			return std::distance(cell_sets_.begin(), i);
	}
	return -1;
}
#endif // _QUEST_EDITOR

void qdInventory::remove_cell_set(int idx) {
	assert(-1 < idx && idx < static_cast<int>(cell_sets_.size()));

	cell_sets_.erase(cell_sets_.begin() + idx);
#ifdef _QUEST_EDITOR
	if (idx == m_iActiveCellset) m_iActiveCellset = -1;
#endif // _QUEST_EDITOR
}

bool qdInventory::load_resources() {
	debugC(4, kDebugLoad, "qdInventory::load_resources(), %d cells", cell_sets_.size());

	for (auto &it : cell_sets_)
		it.load_resources();

	return true;
}

bool qdInventory::free_resources() {
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it)
	it -> free_resources();

	return true;
}

bool qdInventory::load_data(qdSaveStream &fh, int save_version) {
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it) {
		if (!it -> load_data(fh, save_version))
			return false;
	}

	debug_log();

	return true;
}

bool qdInventory::save_data(Common::SeekableWriteStream &fh) const {
	qdInventoryCellSetVector::const_iterator it;
	FOR_EACH(cell_sets_, it) {
		if (!it -> save_data(fh)) {
			return false;
		}
	}

	return true;
}

bool qdInventory::save_data(qdSaveStream &fh) const {
	qdInventoryCellSetVector::const_iterator it;
	FOR_EACH(cell_sets_, it) {
		if (!it -> save_data(fh))
			return false;
	}

	return true;
}

int qdInventory::cell_index(const qdGameObjectAnimated *obj) const {
	int index = 0;
	qdInventoryCellSetVector::const_iterator it;
	FOR_EACH(cell_sets_, it) {
		int idx = it -> cell_index(obj);
		if (idx != -1)
			return index + idx;
		else
			index += it -> num_cells();
	}

	return -1;
}

Vect2s qdInventory::cell_position(int cell_idx) const {
	qdInventoryCellSetVector::const_iterator it;
	FOR_EACH(cell_sets_, it) {
		if (cell_idx < it -> num_cells())
			return it -> cell_position(cell_idx);

		cell_idx -= it -> num_cells();
	}

	return Vect2s(0, 0);
}

void qdInventory::objects_quant(float dt) {
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it) {
		it -> objects_quant(dt);
	}
}

void qdInventory::scroll_left() {
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it)
	it->scroll_left();
	toggle_redraw(true);
}

void qdInventory::scroll_right() {
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it)
	it->scroll_right();
	toggle_redraw(true);
}

void qdInventory::scroll_up() {
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it)
	it->scroll_up();
	toggle_redraw(true);
}

void qdInventory::scroll_down() {
	qdInventoryCellSetVector::iterator it;
	FOR_EACH(cell_sets_, it)
	it->scroll_down();
	toggle_redraw(true);
}

void qdInventory::debug_log() const {
#ifdef _DEBUG
	debugCN(3, kDebugLog, "Inventory ");
	if (name())
		debugCN(3, kDebugLog, "%s", transCyrillic(name()));

	debugC(3, kDebugLog, "--------------");

	qdInventoryCellSetVector::const_iterator it;
	FOR_EACH(cell_sets_, it)
	it->debug_log();

	debugC(3, kDebugLog, "--------------");
#endif
}
} // namespace QDEngine
