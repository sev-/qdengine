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

#ifndef QDENGINE_QDCORE_QD_FILE_OWNER
#define QDENGINE_QDCORE_QD_FILE_OWNER

namespace QDEngine {

class qdFileOwner {
public:
	qdFileOwner() : _files_size(0), _cd_info(0) { }
	virtual ~qdFileOwner() {}

	unsigned files_size() const {
		return _files_size;
	}
	virtual bool get_files_list(qdFileNameList &files_to_copy, qdFileNameList &files_to_pack) const = 0;

	void add_to_CD(int cd_number) {
		_cd_info |= 1 << cd_number;
	}
	void remove_from_CD(int cd_number) {
		_cd_info &= ~(1 << cd_number);
	}
	bool is_on_CD(int cd_number = 1) const {
		int inf = (_cd_info) ? _cd_info : 1;
		return (inf & (1 << cd_number));
	}
	void clear_cd_info() {
		_cd_info = 0;
	}
	void calc_files_size();

	unsigned CD_info() const {
		return _cd_info;
	}
	void set_CD_info(unsigned inf) {
		_cd_info = inf;
	}

private:

	unsigned _cd_info;
	unsigned _files_size;
};

} // namespace QDEngine

#endif // QDENGINE_QDCORE_QD_FILE_OWNER
