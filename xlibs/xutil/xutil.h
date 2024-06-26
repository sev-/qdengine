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

/*
******************************************************************************
**                                      **
**           XTOOL  Windows 32 API version 3.0nt            **
**             for Windows-NT, Windows 95               **
**                                      **
**    (C)1993-97 by K-D Lab::KranK, K-D Lab::Steeler, Dr.Tronick        **
**                                      **
******************************************************************************
*/

#ifndef __XUTIL_H
#define __XUTIL_H

#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include <memory.h>
#include <cstdlib>
#include "common/textconsole.h"




///////////////////////////////////
//		XBuffer
///////////////////////////////////
#define CONV_BUFFER_LEN 63

#define XB_DEFSIZE  256

#define XB_BEG      0
#define XB_CUR      1
#define XB_END      2

#define XB_FORWARD  0
#define XB_BACKWARD 1
#define XB_GLOBAL   2

#define XB_CASEON   0
#define XB_CASEOFF  1

#define XB_DEFRADIX 10
#define XB_DEFDIGITS    8

namespace QDEngine {

class XBuffer {
public:
	XBuffer();
	XBuffer(unsigned int sz, bool automatic_realloc_ = false);
	XBuffer(void *p, int sz);
	~XBuffer() {
		free();
	}

	void SetRadix(int r) {
		radix_ = r;
	}
	void SetDigits(int d) {
		digits_ = d;
	}

	void alloc(unsigned int sz);
	void free();
	bool makeFree() const {
		return makeFree_;
	}

	void fill(char fc = '\0');
	void set(int off, int mode = XB_BEG);
	void init();
	bool search(char *what, int mode = XB_FORWARD, int cs = XB_CASEON);
	bool end() const {
		return offset_ > size_;
	}

	unsigned int tell() const {
		return offset_;
	}
	unsigned int size() const {
		return size_;
	}
	char *buffer() const {
		return buffer_;
	}

	unsigned int read(void *s, unsigned int len);
	unsigned int write(const void *s, unsigned int len, bool bin_flag = true);

	XBuffer &operator< (const char *v);
	XBuffer &operator< (bool v) {
		return write(v);
	}
	XBuffer &operator< (char v) {
		return write(v);
	}
	XBuffer &operator< (unsigned char v) {
		return write(v);
	}
	XBuffer &operator< (short v) {
		return write(v);
	}
	XBuffer &operator< (unsigned short v) {
		return write(v);
	}
	XBuffer &operator< (int v) {
		return write(v);
	}
	XBuffer &operator< (unsigned int v) {
		return write(v);
	}
	XBuffer &operator< (long v) {
		return write(v);
	}
	XBuffer &operator< (unsigned long v) {
		return write(v);
	}
	XBuffer &operator< (float v) {
		return write(v);
	}
	XBuffer &operator< (double v) {
		return write(v);
	}
	XBuffer &operator< (long double v) {
		return write(v);
	}

	XBuffer &operator> (char *v);//��������� ���� �������� �������� ���������, �� ����� �� ������������, ������ ��� ������� ����� �� ������� ����� ��������.
	XBuffer &operator> (bool &v) {
		return read(v);
	}
	XBuffer &operator> (char &v) {
		return read(v);
	}
	XBuffer &operator> (unsigned char &v) {
		return read(v);
	}
	XBuffer &operator> (short &v) {
		return read(v);
	}
	XBuffer &operator> (unsigned short &v) {
		return read(v);
	}
	XBuffer &operator> (int &v) {
		return read(v);
	}
	XBuffer &operator> (unsigned int &v) {
		return read(v);
	}
	XBuffer &operator> (long &v) {
		return read(v);
	}
	XBuffer &operator> (unsigned long &v) {
		return read(v);
	}
	XBuffer &operator> (float &v) {
		return read(v);
	}
	XBuffer &operator> (double &v) {
		return read(v);
	}
	XBuffer &operator> (long double &v) {
		return read(v);
	}

	XBuffer &operator<= (char);
	XBuffer &operator<= (unsigned char);
	XBuffer &operator<= (short);
	XBuffer &operator<= (unsigned short);
	XBuffer &operator<= (int);
	XBuffer &operator<= (unsigned int);
	XBuffer &operator<= (long);
	XBuffer &operator<= (unsigned long);
	XBuffer &operator<= (float);
	XBuffer &operator<= (double);
	XBuffer &operator<= (long double);

	XBuffer &operator>= (char &);
	XBuffer &operator>= (unsigned char &);
	XBuffer &operator>= (short &);
	XBuffer &operator>= (unsigned short &);
	XBuffer &operator>= (int &);
	XBuffer &operator>= (unsigned int &);
	XBuffer &operator>= (long &);
	XBuffer &operator>= (unsigned long &);
	XBuffer &operator>= (float &);
	XBuffer &operator>= (double &);
	XBuffer &operator>= (long double &);

	const char *c_str() const {
		return buffer_;
	}
	operator const char *() const {
		return buffer_;
	}
	const char *operator()(int offs) {
		return buffer_ + offs;
	}
	XBuffer &operator++() {
		offset_++;
		return *this;
	}
	XBuffer &operator--() {
		offset_--;
		return *this;
	}
	XBuffer &operator+= (int v) {
		offset_ += v;
		return *this;
	}
	XBuffer &operator-= (int v) {
		offset_ -= v;
		return *this;
	}

	const char &operator[](int ind) const {
		return buffer_[ind];
	}
	char &operator[](int ind) {
		return buffer_[ind];
	}
	const char &operator()() const {
		return buffer_[offset_];
	}
	char &operator()() {
		return buffer_[offset_];
	}

	template<class T> XBuffer &write(const T &v) {
		while (offset_ + sizeof(T) > size_) handleOutOfSize();
		memcpy(&buffer_[offset_], &v, sizeof(T));
		offset_ += sizeof(T);
		return *this;
	}
	template<class T> XBuffer &read(T &v) {
		memcpy(&v, &buffer_[offset_], sizeof(T));
		offset_ += sizeof(T);
		return *this;
	}

private:
	char *buffer_;
	unsigned int size_;
	unsigned int offset_;
	int radix_;
	int digits_;
	bool makeFree_;
	bool automaticRealloc_;
	char ConvertBuffer_[CONV_BUFFER_LEN + 1];

	void handleOutOfSize();
};


///////////////////////////////////
//		XStream
///////////////////////////////////

#define XS_IN       0x0001
#define XS_OUT      0x0002
#define XS_NOREPLACE    0x0004
#define XS_APPEND   0x0008
#define XS_NOBUFFERING  0x0010
#define XS_NOSHARING    0x0020

#define XS_SHAREREAD    0x0040
#define XS_SHAREWRITE   0x0080

#define XS_BEG      0
#define XS_CUR      1
#define XS_END      2

#define XS_DEFRADIX 10
#define XS_DEFDIGITS    8

class XStream {
	typedef void *XSHANDLE;

public:
	XStream(bool handleErrors = true);
	XStream(const char *name, unsigned flags, bool handleErrors = true);
	~XStream();

	bool    open(const char *name, unsigned f = XS_IN);
	bool    open(XStream *owner, long start, long ext_sz = -1);
	void    close();
	unsigned long read(void *buf, unsigned long len);
	unsigned long write(const void *buf, unsigned long len);
	long    seek(long offset, int dir);
	long    tell() const {
		return pos;
	}
	char   *getline(char *buf, unsigned len);
	bool    eof() {
		return eofFlag || pos >= size();
	}
	long    size() const;
	XSHANDLE    gethandler() {
		return handler;
	}
	void    gettime(unsigned &date, unsigned &time);
	void    flush();
	const char *GetFileName() const {
		return fname;
	}
	void    SetRadix(int r) {
		radix = r;
	}
	void    SetDigits(int d) {
		digits = d;
	}

	XStream &operator< (const char *);
	XStream &operator< (char);
	XStream &operator< (unsigned char);
	XStream &operator< (short);
	XStream &operator< (unsigned short);
	XStream &operator< (int);
	XStream &operator< (unsigned int);
	XStream &operator< (long);
	XStream &operator< (unsigned long);
	XStream &operator< (float);
	XStream &operator< (double);
	XStream &operator< (long double);

	XStream &operator> (char &);
	XStream &operator> (unsigned char &);
	XStream &operator> (short &);
	XStream &operator> (unsigned short &);
	XStream &operator> (int &);
	XStream &operator> (unsigned int &);
	XStream &operator> (long &);
	XStream &operator> (unsigned long &);
	XStream &operator> (float &);
	XStream &operator> (double &);
	XStream &operator> (long double &);

	XStream &operator<= (char);
	XStream &operator<= (unsigned char);
	XStream &operator<= (short);
	XStream &operator<= (unsigned short);
	XStream &operator<= (int);
	XStream &operator<= (unsigned int);
	XStream &operator<= (long);
	XStream &operator<= (unsigned long);
	XStream &operator<= (float);
	XStream &operator<= (double);
	XStream &operator<= (long double);

	XStream &operator>= (char &);
	XStream &operator>= (unsigned char &);
	XStream &operator>= (short &);
	XStream &operator>= (unsigned short &);
	XStream &operator>= (int &);
	XStream &operator>= (unsigned int &);
	XStream &operator>= (long &);
	XStream &operator>= (unsigned long &);
	XStream &operator>= (float &);
	XStream &operator>= (double &);
	XStream &operator>= (long double &);

	bool handleErrors() const {
		return handleErrors_;
	}
	void setHandleErrors(bool handleErrors) {
		handleErrors_ = handleErrors;
	}

	bool isOpen() const {
		return handler != XSHANDLE(-1);
	}
	bool ioError() const {
		return ioError_;
	}

	template<class T> XStream &write(const T &v) {
		write(&v, sizeof(T));
		return *this;
	}
	template<class T> XStream &read(T &v) {
		read(&v, sizeof(T));
		return *this;
	}

private:
	XSHANDLE handler;
	long    pos;
	bool eofFlag;
	bool handleErrors_;
	bool ioError_;
	const char *fname;
	int radix, digits;
	long    extSize;
	long    extPos;
	char ConvertBuffer_[CONV_BUFFER_LEN + 1];
};


///////////////////////////////////
//		ErrH
///////////////////////////////////

#ifndef NULL
#define NULL    0L
#endif

#define X_WINNT         0x0001
#define X_WIN32S        0x0002
#define X_WIN95         0x0003

#define XERR_ALL        0xFFFF
#define XERR_NONE       0x0000

#define XERR_USER       0x0001
#define XERR_CTRLBRK        0x0004
#define XERR_MATH       0x0008
#define XERR_FLOAT      0x0010
#define XERR_CRITICAL       0x0020
#define XERR_SIGNAL     0x0040

#define XERR_ACCESSVIOLATION    0x0080
#define XERR_DEBUG      0x0100
#define XERR_UNKNOWN        0x0200

#define xassert(exp) assert(exp)
#define xassertStr(exp, str)

///////////////////////////////////
//		Utils
///////////////////////////////////

void *xalloc(unsigned sz);
void *xrealloc(void *p, unsigned sz);
void  xfree(void *p);

#define XALLOC(a)   xalloc(a)
#define XREALLOC(a,b)   xrealloc(a,b)
#define XFREE(a)    xfree(a)

unsigned int XRnd(unsigned int m);
void XRndSet(unsigned int m);
unsigned int XRndGet();

char *XFindNext();
char *XFindFirst(char *mask);

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

#ifndef __ROUND__
#define __ROUND__

template <class T>
inline T sqr(const T &x) {
	return x*x;
}

template <class T>
inline int SIGN(const T &x) {
	return x ? (x > 0 ? 1 : -1) : 0;
}

#endif // __ROUND__

inline int BitSR(int x) {
	int return_var = 0;

	warning("STUB: BitSR()");

#if 0
	_asm {
		mov eax, x
		cdq
		xor eax, edx
		sub     eax, edx
		bsr     eax, eax
		mov[return_var], eax
	}
#endif
	return return_var;
}


void xtDeleteFile(char *fname);

///////////////////////////////////

const char *check_command_line(const char *switch_str); // 0 ��� ������ ����� ����� ��� �������

template<class T> // ��� ��������� ����������
bool check_command_line_parameter(const char *switch_str, T &parameter) {
	const char *s = check_command_line(switch_str);
	if (s) {
		parameter = atoi(s);
		return true;
	} else return false;
}

// ����� Unicode ������ � �������
//void dcprintfW(wchar_t *format, ...);
// ����� Unicode ������ � Debug Output
//void dprintfW(wchar_t *format, ...);

///////////////////////////////////
//		__XCPUID_H
///////////////////////////////////

#define CPU_INTEL       0x00000001

// Intel features IDs

#define INTEL_MMX       0x00800000

// Intel Family IDs
#define CPU_INTEL_386       3
#define CPU_INTEL_486       4
#define CPU_INTEL_PENTIUM   5
#define CPU_INTEL_PENTIUM_PRO   6

//Intel Pentium model's
#define PENTIUM_P5A     0
#define PENTIUM_P5      1
#define PENTIUM_P54C        2
#define PENTIUM_P54T        3
#define PENTIUM_MMX     4
#define PENTIUM_DX4     5
#define PENTIUM_P5OVER      6
#define PENTIUM_P54CA       7

// Intel Pentium Pro model's
#define PENTIUM_PROA        0
#define PENTIUM_PRO     1
#define PENTIUM_PRO2        3
#define PENTIUM_PRO55CT     4
#define PENTIUM_PROF        5

extern unsigned int xt_processor_id;
extern unsigned int xt_processor_type;
extern unsigned int xt_processor_family;
extern unsigned int xt_processor_model;
extern unsigned int xt_processor_revision;
extern unsigned int xt_processor_features;

unsigned int xt_get_cpuid();
char *xt_getMMXstatus();

extern int xt_mmxUse;


///////////////////////////////////
//		__XZIP_H__
///////////////////////////////////

typedef unsigned long ulong;

ulong ZIP_compress(char *trg, ulong trgsize, char *src, ulong srcsize);
ulong ZIP_GetExpandedSize(char *p);
void ZIP_expand(char *trg, ulong trgsize, char *src, ulong srcsize);


///////////////////////////////////
//		Automatic linking
///////////////////////////////////
#ifndef _XUTIL_NO_AUTOMATIC_LIB

#define _LIB_NAME "XUtil"

#endif // _XUTIL_NO_AUTOMATIC_LIB

} // namespace QDEngine

#endif //
