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

#ifndef QDENGINE_CORE_QDCORE_QD_NAMED_OBJECT_REFERENCE_H
#define QDENGINE_CORE_QDCORE_QD_NAMED_OBJECT_REFERENCE_H

#include "qdengine/core/parser/xml_fwd.h"
#include "qdengine/core/qdcore/qd_named_object.h"

namespace Common {
class SeekableWriteStream;
}

namespace QDEngine {

class qdNamedObjectReference {
public:
	qdNamedObjectReference();
	explicit qdNamedObjectReference(const qdNamedObject *p);
	qdNamedObjectReference(int levels, const int *types, const char *const *names);
	qdNamedObjectReference(const qdNamedObjectReference &ref);
	~qdNamedObjectReference();

	qdNamedObjectReference &operator = (const qdNamedObjectReference &ref);

	int num_levels()            const {
		return object_types_.size();
	}
	int object_type(int level = 0)      const {
		return object_types_[level];
	}
	const char *object_name(int level = 0)  const {
		return object_names_[level].c_str();
	}
	bool is_empty()             const {
		return object_types_.empty();
	}

	bool init(const qdNamedObject *p);

	void clear() {
		object_types_.clear();
		object_names_.clear();
	}

	void load_script(const xml::tag *p);
	bool save_script(XStream &fh, int indent = 0) const;

	//! Загрузка данных из сэйва.
	bool load_data(qdSaveStream &fh, int save_version);
	bool load_data(Common::SeekableReadStream &fh, int save_version);
	//! Запись данных в сэйв.
	bool save_data(qdSaveStream &fh) const;
	bool save_data(Common::SeekableWriteStream &fh) const;

	qdNamedObject *object() const;

	Common::String toString() const;

private:

	std::vector<int> object_types_;
	std::vector<std::string> object_names_;
	static int objects_counter_;
};

} // namespace QDEngine

#endif // QDENGINE_CORE_QDCORE_QD_NAMED_OBJECT_REFERENCE_H
