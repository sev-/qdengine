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
#include "common/file.h"
#include "graphics/managed_surface.h"

#include "video/mpegps_decoder.h"

#include "qdengine/qdengine.h"
#include "qdengine/qdcore/util/WinVideo.h"

#include "qdengine/system/graphics/gr_dispatcher.h"


namespace QDEngine {

bool winVideo::_is_initialized = false;

#ifdef __WINVIDEO_LOG__
void *winVideo::log_file_handle_ = NULL;
#endif

winVideo::winVideo() : _graph_builder(NULL),
	_media_control(NULL),
	_video_window(NULL),
	_media_event(NULL),
	_basic_video(NULL),
	_basic_audio(NULL),
	_hwnd(NULL) {
	_decoder = new Video::MPEGPSDecoder();
}

winVideo::~winVideo() {
	close_file();
	delete _decoder;
}

bool winVideo::init() {
	return true;
}

bool winVideo::done() {
	if (!_is_initialized) return false;

	_is_initialized = false;

	return true;
}

bool winVideo::set_volume(int volume_db) {
	warning("STUB: winVideo::set_volume");
#if 0
	if (_basic_audio) {
		_basic_audio->put_Volume(volume_db);
		return true;
	}
#endif
	return false;
}

void winVideo::set_window(int x, int y, int xsize, int ysize) {
	_x = x;
	_y = y;

	_vidWidth = xsize;
	_vidHeight = ysize;

	delete _tempSurf;
	_tempSurf = nullptr;

	if (_vidWidth != _decoder->getWidth() || _vidHeight != _decoder->getHeight())
		_tempSurf = new Graphics::ManagedSurface(xsize, ysize, g_engine->_pixelformat);
}

bool winVideo::open_file(const char *fname) {
	Common::File *videoFile = new Common::File();

	if (!videoFile->open(fname)) {
		warning("WinVideo::open: Failed to open file %s", fname);
		delete videoFile;
		return false;
	}

	_videostream = videoFile;

	if (!_decoder->loadStream(_videostream)) {
		warning("WinVideo::play: Failed to Load Stream");
		delete videoFile;
		return false;
	}

	return true;
}

bool winVideo::play() {
	if (!_videostream) {
		warning("WinVideo::play: No video stream loaded");
		return false;
	}

	_decoder->start();

	return true;
}

bool winVideo::quant() {
	debugC(9, kDebugGraphics, "WinVideo::play: Video Playback loop");

	// Video Playback loop
	if (_decoder->needsUpdate()) {
		const Graphics::Surface *frame = _decoder->decodeNextFrame();
		int frameWidth = _decoder->getWidth();
		int frameHeight = _decoder->getHeight();

		const Graphics::Surface *surf = frame;

		if (frame) {
			if (_tempSurf) {
				const Common::Rect srcRect(0, 0, frameWidth, frameHeight);
				const Common::Rect destRect(0, 0, _vidWidth, _vidHeight);

				_tempSurf->blitFrom(*frame, srcRect, destRect);
				surf = _tempSurf->surfacePtr();
			}

			g_system->copyRectToScreen(surf->getPixels(), surf->pitch, _x, _y, _vidWidth, _vidHeight);
		}

		g_system->updateScreen();
	}

	return true;
}

bool winVideo::stop() {
	delete _tempSurf;
	_tempSurf = NULL;

	return true;
}

winVideo::PlaybackStatus winVideo::playback_status() {
	if (_graph_builder && _video_window && _media_control && _hwnd) {
		warning("STUB: winVideo::playback_status");
#if 0
		OAFilterState pfs;

		if (_media_control->GetState(INFINITE, &pfs) != S_OK)
			return VID_STOPPED;
		if (pfs == State_Running) return VID_RUNNING;
		if (pfs == State_Stopped) return VID_STOPPED;
		if (pfs == State_Paused) return VID_PAUSED;
#endif
	}

	return VID_STOPPED;
}

bool winVideo::wait_end() {
	warning("STUB: winVideo::wait_end()");

	if (_media_event) {
#if 0
		long evCode;
		_media_event->WaitForCompletion(INFINITE, &evCode);
#endif
		return true;
	}
	return false;
}

bool winVideo::is_playback_finished() {
	return _decoder->endOfVideo();
}

bool winVideo::toggle_fullscreen(bool fullscr) {
	warning("STUB: winVideo::toggle_fullscreen()");

	if (_video_window) {
#if 0
		_video_window->put_FullScreenMode(fullscr);
#endif
		return true;
	}

	return false;
}

bool winVideo::toggle_cursor(bool visible) {
	warning("STUB: winVideo::toggle_cursor()");

	if (!_video_window)
		return false;
#if 0
	_video_window->HideCursor(visible);
#endif
	return true;
}

bool winVideo::get_movie_size(int &sx, int &sy) {
	if (!_decoder)
		return false;

	sx = _decoder->getWidth();
	sy = _decoder->getHeight();

	return true;
}

bool winVideo::set_window_size(int sx, int sy) {
	warning("STUB: winVideo::set_window_size()");

	if (!_video_window) return false;
#if 0
	_video_window->put_Width(sx);
	_video_window->put_Height(sy);
#endif
	return true;
}

void winVideo::close_file() {
	warning("STUB: winVideo::close_file()");
	return;
}

} // namespace QDEngine
