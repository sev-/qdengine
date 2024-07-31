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

#ifndef QDENGINE_QDCORE_QD_INVENTORY_H
#define QDENGINE_QDCORE_QD_INVENTORY_H

#include "qdengine/system/input/mouse_input.h"
#include "qdengine/qdcore/qd_named_object.h"
#include "qdengine/qdcore/qd_inventory_cell.h"

namespace  Common {
class WriteStream;
}

namespace QDEngine {

//! Инвентори.
class qdInventory : public qdNamedObject {
public:
	qdInventory();
	~qdInventory();

	//! Флаги.
	enum {
		//! если установлен, объект берется сначала на мышь
		INV_TAKE_TO_MOUSE       = 0x01,
		//! если установлен, то после взятия объекта инвентори не открывается
		INV_DONT_OPEN_AFTER_TAKE    = 0x02,
		//! если установлен, то после клика объектом он возвращается в инветори
		INV_PUT_OBJECT_AFTER_CLICK  = 0x04,
		//! если установлен, объект возвращается в инветори по правому клику мыши
		INV_ENABLE_RIGHT_CLICK      = 0x08,
		//! отрисовывается даже если неактивно в данный момент
		INV_VISIBLE_WHEN_INACTIVE   = 0x10
	};

	enum {
		INV_DEFAULT_SHADOW_COLOR = 0,
		INV_DEFAULT_SHADOW_ALPHA = 128
	};

	int named_object_type() const {
		return QD_NAMED_OBJECT_INVENTORY;
	}

	void add_cell_set(const qdInventoryCellSet &set) {
		cell_sets_.push_back(set);
	}
	void remove_cell_set(int idx);

#ifdef _QUEST_EDITOR
	qdInventoryCellSet *add_cell_set(const Vect2s &scr_pos, const Vect2s &sz, const qdInventoryCellType &t) {
		typedef qdInventoryCellSetVector::iterator i_t;
		i_t res = cell_sets_.insert(cell_sets_.end(), qdInventoryCellSet());
		if (res == cell_sets_.end())
			return NULL;

		res->set_size(sz);
		res->set_cell_type(t);
		res->set_screen_pos(scr_pos);

		return &*res;
	}
	const qdInventoryCellSetVector &cell_sets() const {
		return cell_sets_;
	}

	//возвращает номер сельсета, если попали иначе -1
	int hit(int x, int y);

	int selected_cellset_num() const {
		return m_iActiveCellset;
	}
	void select_cellset(int inum) const {
		assert(-1 <= inum && inum < (int)cell_sets_.size());
		m_iActiveCellset = inum;
	}
	const qdInventoryCellSet *selected_cellset() const {
		if (m_iActiveCellset == -1)
			return NULL;
		return &get_cell_set(m_iActiveCellset);
	}
#endif // _QUEST_EDITOR

	const qdInventoryCellSet &get_cell_set(int id = 0) const {
		return cell_sets_[id];
	}

	void set_cells_pos(int set_id, const Vect2s &pos) {
		if (set_id >= 0 && set_id < cell_sets_.size())
			cell_sets_[set_id].set_screen_pos(pos);
	}
	void set_cells_type(int set_id, const qdInventoryCellType &tp) {
		if (set_id >= 0 && set_id < cell_sets_.size())
			cell_sets_[set_id].set_cell_type(tp);
	}
	void set_cells_size(int set_id, const Vect2s &sz) {
		if (set_id >= 0 && set_id < cell_sets_.size())
			cell_sets_[set_id].set_size(sz);
	}

	int cell_index(const qdGameObjectAnimated *obj) const;
	Vect2s cell_position(int cell_idx) const;

	void redraw(int offs_x = 0, int offs_y = 0, bool inactive_mode = false) const;
	void toggle_redraw(bool state) {
		_need_redraw = state;
	}
	void pre_redraw() const;
	void post_redraw();

	bool put_object(qdGameObjectAnimated *p);
	bool put_object(qdGameObjectAnimated *p, const Vect2s &pos);
	bool remove_object(qdGameObjectAnimated *p);
	qdGameObjectAnimated *get_object(const Vect2s &pos) const;
	bool is_object_in_list(const qdGameObjectAnimated *p) const;

	unsigned shadow_color() const {
		return _shadow_color;
	}
	void set_shadow_color(unsigned color) {
		_shadow_color = color;
	}

	int shadow_alpha() const {
		return _shadow_alpha;
	}
	void set_shadow_alpha(int alpha) {
		_shadow_alpha = alpha;
	}

	bool load_script(const xml::tag *p);
	bool save_script(Common::WriteStream &fh, int indent = 0) const;

	bool load_resources();
	bool free_resources();

	//! Загрузка данных из сэйва.
	bool load_data(Common::SeekableReadStream &fh, int save_version);
	//! Запись данных в сэйв.
	bool save_data(Common::WriteStream &fh) const;

	bool init(const qdInventoryCellTypeVector &tp);

	bool mouse_handler(int x, int y, mouseDispatcher::mouseEvent ev);

	void objects_quant(float dt);

	Vect2s additional_cells() const {
		return _additional_cells;
	}
	void set_additional_cells(Vect2s val) {
		_additional_cells = val;
	}

	//! Скроллинг
	void scroll_left();
	void scroll_right();
	void scroll_up();
	void scroll_down();

	void debug_log() const;

private:

	qdInventoryCellSetVector cell_sets_;

	bool _need_redraw;

	unsigned _shadow_color;
	int _shadow_alpha;

	//! Дополнительные ячейки для всех наборов инвентори
	Vect2s _additional_cells;

#ifdef _QUEST_EDITOR
	mutable int m_iActiveCellset;
#endif
};

} // namespace QDEngine

#endif // QDENGINE_QDCORE_QD_INVENTORY_H
