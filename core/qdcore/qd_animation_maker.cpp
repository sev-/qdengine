/* ---------------------------- INCLUDE SECTION ----------------------------- */
#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "common/archive.h"
#include "qdengine/core/qd_precomp.h"
#include "qdengine/core/qdcore/qd_animation.h"
#include "qdengine/core/qdcore/qd_animation_maker.h"


namespace QDEngine {


/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

qdAnimationMaker::qdAnimationMaker() : default_frame_length_(0.05f),
	callback_data_(0),
	progress_callback_(0) {
}

qdAnimationMaker::~qdAnimationMaker() {
}

maker_progress_fnc qdAnimationMaker::set_callback(maker_progress_fnc p, void *data) {
	maker_progress_fnc old_p = progress_callback_;
	progress_callback_ = p;
	callback_data_ = data;

	return old_p;
}

bool qdAnimationMaker::insert_frame(class qdAnimation *p, const char *fname, int &insert_pos, bool insert_after, bool dupe_check) {
	// IMPORTANT(pabdulin): auto_ptr usage was removed
	qdAnimationFrame *fp = new qdAnimationFrame;
	fp -> set_file(fname);
	fp -> set_length(default_frame_length_);

	if (!fp -> load_resources())
		return false;

	if (p -> check_flag(QD_ANIMATION_FLAG_CROP))
		fp -> crop();

	if (p -> check_flag(QD_ANIMATION_FLAG_COMPRESS))
		fp -> compress();

	qdAnimationFrame *ins_p = p -> get_frame(insert_pos);
	if (dupe_check && ins_p && *ins_p == *fp) {
		ins_p -> set_length(ins_p -> length() + fp -> length());
		return true;
	}

	if (!p -> add_frame(fp, ins_p, insert_after))
		return false;

	if (insert_after)
		insert_pos++;

	p -> init_size();

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
		qdFileNameList::iterator it;
		FOR_EACH(flist, it) {
			if (insert_frame(p, it -> c_str(), insert_pos, insert_after, true))
				result = true;

			if (progress_callback_) {
				int percents = i++ * 100 / flist.size();
				(*progress_callback_)(percents, callback_data_);
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
