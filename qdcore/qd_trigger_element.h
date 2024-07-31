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

#ifndef QDENGINE_QDCORE_QD_TRIGGER_ELEMENT_H
#define QDENGINE_QDCORE_QD_TRIGGER_ELEMENT_H

#include "qdengine/parser/xml_fwd.h"
#include "qdengine/qdcore/qd_named_object.h"
#include "qdengine/qdcore/qd_named_object_reference.h"
#include "qdengine/qdcore/qd_trigger_profiler.h"

namespace Common {
class WriteStream;
}

namespace QDEngine {

class qdTriggerElement;
class qdTriggerLink;

typedef qdTriggerElement *qdTriggerElementPtr;
typedef qdTriggerElement const *qdTriggerElementConstPtr;

//! Связь между двумя элементами триггера.
/**
Управляет активацией элемента триггера.
*/
class qdTriggerLink {
public:
	//! Состояние связи.
	enum LinkStatus {
		//! Cвязь выключена.
		LINK_INACTIVE,
		//! Связь включена.
		/**
		    Элемент триггера проверяет условия и активируется, если они выполняются.
		*/
		LINK_ACTIVE,
		//! Связь отработана.
		/**
		    Элемент триггера активировался и завершил работу.
		*/
		LINK_DONE
	};

	qdTriggerLink(qdTriggerElementPtr p, int tp = 0);
	qdTriggerLink();
	~qdTriggerLink() { }

	//! Возвращает состояние связи.
	LinkStatus status() const {
		return _status;
	}
	//! Устанавливает состояние связи.
	void set_status(LinkStatus st) {
		_status = st;
	}

	//! Возвращает тип связи.
	int type() const {
		return type_;
	}
	//! Устанавливает тип связи.
	void set_type(int _t) {
		type_ = _t;
	}

	//! Возвращает элемент триггера, к которому идет связь.
	qdTriggerElementPtr const &element() const {
		return element_;
	}
	//! Устанавливает элемент триггера, к которому идет связь.
	void set_element(qdTriggerElementPtr const &el) {
		element_ = el;
	}

	//! Возвращает идентификатор элемента, к которому идет связь.
	int element_ID() const {
		return element_ID_;
	}
	//! Устанавливает идентификатор элемента, к которому идет связь.
	/**
	Работает только если сам элемент нулевой.
	*/
	bool set_element_ID(int id) {
		if (!element_) {
			element_ID_ = id;
			return true;
		}
		return false;
	}

	bool operator == (qdTriggerElementConstPtr e) const {
		return (element_ == e);
	}

#ifdef _QUEST_EDITOR
	const SIZE &get_owner_offset() const {
		return m_owner_offset;
	}
	void set_owner_offset(const SIZE &sz) {
		m_owner_offset = sz;
	}
	void set_owner_offset(int x, int y) {
		m_owner_offset.cx = x;
		m_owner_offset.cy = y;
	}

	const SIZE &get_child_offset() const {
		return m_child_offset;
	}
	void set_child_offset(const SIZE &sz) {
		m_child_offset = sz;
	}
	void set_child_offset(int x, int y) {
		m_child_offset.cx = x;
		m_child_offset.cy = y;
	}
#endif
	//! Активирует связь.
	void activate();
	//! Деактивирует связь.
	void deactivate();

	//! Устанавливает, надо ли автоматом перезапускать линк.
	void toggle_auto_restart(bool state) {
		auto_restart_ = state;
	}
	//! Возвращает true, если надо автоматом перезапускать линк.
	bool auto_restart() const {
		return auto_restart_;
	}

	bool load_script(const xml::tag *p);
	bool save_script(Common::WriteStream &fh, int indent = 0) const;

	Common::String toString();

private:
	//! Тип связи.
	int type_;
	//! Элемент, к которому направлена связь.
	qdTriggerElementPtr element_;
	//! Идентификатор элемента, к которому направлена связь.
	int element_ID_;

	//! Состояние связи.
	LinkStatus _status;

	//! Если true, линк автоматом активируется после выключения.
	bool auto_restart_;

#ifdef _QUEST_EDITOR
	//отступ от центра прямоугольника в родительском элементе
	SIZE m_owner_offset;
	//отступ от центра прямоугольника в дочернем элементе
	SIZE m_child_offset;
#endif // _QUEST_EDITOR
};

typedef std::vector<qdTriggerLink> qdTriggerLinkList;

//! Элемент триггера.
class qdTriggerElement
#ifdef _QUEST_EDITOR
	: public SharedCounter
#endif // _QUEST_EDITOR
{
public:
	qdTriggerElement();
	qdTriggerElement(qdNamedObject *p);
	~qdTriggerElement();

	enum {
		ROOT_ID = -1,
		INVALID_ID = -2
	};

	enum ElementStatus {
		TRIGGER_EL_INACTIVE,
		TRIGGER_EL_WAITING,
		TRIGGER_EL_WORKING,
		TRIGGER_EL_DONE
	};

#ifdef _QUEST_EDITOR
	const std::string &title() const {
		return m_strTitle;
	}
	void set_title(const std::string &strTitle) {
		m_strTitle = strTitle;
	}

	const RECT &bounding_rect() const {
		return m_rcBound;
	}
	void set_bounding_rect(const RECT &r) {
		memcpy(&m_rcBound, &r, sizeof(r));
	}

	bool in_bound(const POINT &p) {
		return static_cast<bool>(PtInRect(&m_rcBound, p));
	}

	const POINT left_top() const {
		POINT p = {m_rcBound.left, m_rcBound.top};
		return p;
	}
	void set_cell_number(int x, int y) {
		m_vCellNumber.x = x;
		m_vCellNumber.y = y;
	}

	const POINT &cell_number() const {
		return m_vCellNumber;
	}

	void select(bool bs) {
		m_bSelected = bs;
	}
	bool selected() const {
		return m_bSelected;
	}
	bool update_object_reference();
	void update_title();
#endif //_QUEST_EDITOR

	qdTriggerLink *find_child_link(qdTriggerElementConstPtr ptrChild);
	qdTriggerLink *find_child_link(int child_id);

	qdTriggerLink *find_parent_link(qdTriggerElementConstPtr ptrParent);
	qdTriggerLink *find_parent_link(int parent_id);

	//! Возвращает true, если элемент помечен как активный для отладочной проверки.
	bool is_active() const {
		return _is_active;
	}
	//! Помечает элемент как активный (или неактивный) для отладочной проверки.
	void make_active(bool v) {
		_is_active = v;
	}

	//! Инициализация для отладочной проверки.
	/**
	Включает связи, идущие к элементу, и
	помечает как невыполненные все ветки после элемента.
	*/
	bool debug_set_active();
	//! Инициализация для отладочной проверки.
	/**
	Помечает сам элемент и связи, идущие к нему, как выполненные.
	*/
	bool debug_set_done();
	//! Инициализация для отладочной проверки.
	/**
	Помечает сам элемент и связи, идущие к нему, как невыполненные.
	*/
	bool debug_set_inactive();

	bool check_external_conditions(int link_type);
	bool check_internal_conditions();

	ElementStatus status() const {
		return _status;
	}
	void set_status(ElementStatus st);
//	void set_status(ElementStatus st){ _status = st; }

	int ID() const {
		return _ID;
	}
	void set_id(int id) {
		_ID = id;
	}

	qdNamedObject *object() const {
		return _object;
	}
	bool add_object_trigger_reference();
	bool clear_object_trigger_references();

#ifdef _QUEST_EDITOR
	qdNamedObjectReference *object_reference() {
		return &object_reference_;
	}
#endif

	bool retrieve_object(const qdNamedObjectReference &ref);
	bool retrieve_link_elements(qdTriggerChain *p);

	qdTriggerLinkList &parents()  {
		return _parents;
	}
	qdTriggerLinkList &children() {
		return _children;
	}

	const qdTriggerLinkList &parents()  const {
		return _parents;
	}
	const qdTriggerLinkList &children() const {
		return _children;
	}

	bool is_parent(qdTriggerElementConstPtr p);
	bool is_child(qdTriggerElementConstPtr p);

	bool add_parent(qdTriggerElementPtr p, int link_type = 0);
	bool add_child(qdTriggerElementPtr p, int link_type = 0, bool auto_restart = false);

	bool remove_parent(qdTriggerElementPtr p);
	bool remove_child(qdTriggerElementPtr p);

	bool set_child_link_status(qdTriggerElementConstPtr child, qdTriggerLink::LinkStatus st);
	bool set_parent_link_status(qdTriggerElementConstPtr parent, qdTriggerLink::LinkStatus st);

#ifdef _QUEST_EDITOR
	bool set_parent_link_owner_offset(qdTriggerElementConstPtr el, int x, int y);
	bool set_parent_link_child_offset(qdTriggerElementConstPtr el, int x, int y);
	bool set_child_link_owner_offset(qdTriggerElementConstPtr el, int x, int y);
	bool set_child_link_child_offset(qdTriggerElementConstPtr el, int x, int y);

	void clear_parents();
	void clear_children();
#endif

	bool load_script(const xml::tag *p);
	bool save_script(Common::WriteStream &fh, int indent = 0) const;

	//! Загрузка данных из сэйва.
	bool load_data(Common::SeekableReadStream &fh, int save_version);
	//! Запись данных в сэйв.
	bool save_data(Common::WriteStream &fh) const;

	bool quant(float dt);

	void reset();
	void deactivate(const qdNamedObject *ignore_object = NULL);

#ifdef __QD_TRIGGER_PROFILER__
	void set_owner(const qdTriggerChain *p) {
		_owner = p;
	}
#endif

private:

	//! Специальные состояния - используются только в сэйве.
	enum ElementStatusSpecial {
		//! неактивен сам триггер и все связи от него и к нему
		TRIGGER_EL_INACTIVE_ALL,
		//! выполнен сам триггер и все связи от него и к нему
		TRIGGER_EL_DONE_ALL
	};

	int _ID;

	ElementStatus _status;

	//! Является ли элемент активным (для отладки, при проверке триггеров).
	bool _is_active;

	qdNamedObject *_object;

#ifdef _QUEST_EDITOR
	qdNamedObjectReference object_reference_;
#endif

	qdTriggerLinkList _parents;
	qdTriggerLinkList _children;

#ifdef __QD_TRIGGER_PROFILER__
	const qdTriggerChain *_owner;
#endif

#ifdef _QUEST_EDITOR

	//передаем элемент, на который указывает линк
	//потому что он его однозначно идентифицирует

	RECT m_rcBound;     // прямоугольник, охватывающий текст
	std::string m_strTitle; // то что отображаем на экране
	bool m_bSelected;
	POINT m_vCellNumber;    //номер ячейки по х и по у.
#endif //_QUEST_EDITOR

	bool load_links_script(const xml::tag *p, bool load_parents);

	bool activate_links(qdTriggerElementPtr child);
	bool deactivate_links(qdTriggerElementPtr child);

	bool deactivate_link(qdTriggerElementPtr child);
	bool conditions_quant(int link_type);
	void start();
};

typedef std::vector<qdTriggerElementPtr> qdTriggerElementList;

} // namespace QDEngine

#endif // QDENGINE_QDCORE_QD_TRIGGER_ELEMENT_H
