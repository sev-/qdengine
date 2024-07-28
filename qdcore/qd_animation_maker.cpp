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
#include "common/archive.h"
#include "qdengine/qd_precomp.h"
#include "qdengine/qdcore/qd_animation.h"
#include "qdengine/qdcore/qd_animation_maker.h"


namespace QDEngine {

qdAnimationMaker::qdAnimationMaker() : _default_frame_length(0.05f),
	_callback_data(0),
	_progress_callback(0) {
}

qdAnimationMaker::~qdAnimationMaker() {
}

maker_progress_fnc qdAnimationMaker::set_callback(maker_progress_fnc p, void *data) {
	maker_progress_fnc old_p = _progress_callback;
	_progress_callback = p;
	_callback_data = data;

	return old_p;
}

bool qdAnimationMaker::insert_frame(class qdAnimation *p, const char *fname, int &insert_pos, bool insert_after, bool dupe_check) {
	// IMPORTANT(pabdulin): auto_ptr usage was removed
	qdAnimationFrame *fp = new qdAnimationFrame;
	fp->set_file(fname);
	fp->set_length(_default_frame_length);

	if (!fp->load_resources())
		return false;

	if (p->check_flag(QD_ANIMATION_FLAG_CROP))
		fp->crop();

	if (p->check_flag(QD_ANIMATION_FLAG_COMPRESS))
		fp->compress();

	qdAnimationFrame *ins_p = p->get_frame(insert_pos);
	if (dupe_check && ins_p && *ins_p == *fp) {
		ins_p->set_length(ins_p->length() + fp->length());
		return true;
	}

	if (!p->add_frame(fp, ins_p, insert_after))
		return false;

	if (insert_after)
		insert_pos++;

	p->init_size();

	//fp->release();
	delete fp;
	return true;
}

bool qdAnimationMaker::insert_frames(class qdAnimation *p, const char *folder, int &insert_pos, bool insert_after) {
	bool result = false;

	warning("STUB: qdAnimationMaker::insert_frames");
#if 0
	char old_path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, old_path);
	SetCurrentDirectory(folder);
#endif
	warning("STUB:qdAnimationMaker::insert_frames. Check the implementation.");
	qdFileNameList flist;
	Common::ArchiveMemberList files;
	SearchMan.listMatchingMembers(files, "*.tga");
	for (int i = 0; i < files.size(); i++) {
		Common::String fname = files.front()->getFileName();
		files.pop_front();
		flist.push_back(fname.c_str());
	}

	flist.sort();

	if (!flist.empty()) {
		int i = 0;
		for (auto &it : flist) {
			if (insert_frame(p, it.c_str(), insert_pos, insert_after, true))
				result = true;

			if (_progress_callback) {
				int percents = i++ * 100 / flist.size();
				(*_progress_callback)(percents, _callback_data);
			}
		}
		flist.clear();
	}
#if 0
	SetCurrentDirectory(old_path);
#endif
	return result;
}
} // namespace QDEngine
