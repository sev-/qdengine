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

#ifndef QDENGINE_CORE_QDCORE_QD_NAMED_OBJECT_BASE_H
#define QDENGINE_CORE_QDCORE_QD_NAMED_OBJECT_BASE_H

#include <string>

namespace QDEngine {

//! Базовый поименованный объект.
class qdNamedObjectBase {
public:

	qdNamedObjectBase();
	qdNamedObjectBase(const qdNamedObjectBase &obj);
	virtual ~qdNamedObjectBase() = 0;

	qdNamedObjectBase &operator = (const qdNamedObjectBase &obj);

	//! Возвращает имя объекта.
	const char *name() const {
		if (!name_.empty()) return name_.c_str();
		return NULL;
	}
	//! Устанавливает имя объекта.
	void set_name(const char *p) {
		if (p) name_ = p;
		else name_.clear();
	}

private:

	//! Имя объекта.
	std::string name_;
};

} // namespace QDEngine

#endif // QDENGINE_CORE_QDCORE_QD_NAMED_OBJECT_BASE_H
