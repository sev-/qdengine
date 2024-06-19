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

#include "qdengine/xlibs/xutil/xglobal.h"


namespace QDEngine {

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */

/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

char *memoutMSG = "OUT OF MEMORY";

static unsigned int XRndValue = 83838383;

unsigned int XRnd(unsigned int m) {
	XRndValue = XRndValue * 214013L + 2531011L;
	if (!m)
		return 0;
	return ((XRndValue >> 16) & 0x7fff) % m;
}

void XRndSet(unsigned int m) {
	XRndValue = m;
}

unsigned int XRndGet() {
	return XRndValue;
}


void* xalloc(unsigned sz) {
	if (!sz) return NULL;
	void* p = GlobalAlloc(GMEM_FIXED, sz);
	if (!p) ErrH.Abort(memoutMSG, XERR_USER, sz);
	return p;
}

void* xrealloc(void* p0, unsigned sz) {
	if (!sz) return NULL;
	void* p = GlobalReAlloc(p0, sz, NULL);
	if (!p) ErrH.Abort(memoutMSG, -1, GetLastError());
	return p;
}

void xfree(void* p) {
	GlobalFree(p);
}
#if 0
static struct WIN32_FIND_DATA FFdata;
#endif
static HANDLE FFh;

char* XFindNext() {
	warning("STUB: XFindNext().");
#if 0
	if (FindNextFile(FFh, &FFdata) == TRUE) {
		if (FFdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return XFindNext();
		return FFdata.cFileName;
	} else {
		FindClose(FFh);
		return NULL;
	}
#endif
	return NULL;
}

char* XFindFirst(char* mask) {
	warning("STUB: XFindFirst()");
#if 0
	FFh = FindFirstFile(mask, &FFdata);
	if (FFh == INVALID_HANDLE_VALUE)
		return NULL;
	if (FFdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return XFindNext();
	return FFdata.cFileName;
#endif
	return NULL;
}

void xtDeleteFile(char* fname) {
	warning("STUB: xtDeleteFile().");
#if 0
	OFSTRUCT p;
	OpenFile(fname, &p, OF_DELETE);
#endif
}

} // namespace QDEngine