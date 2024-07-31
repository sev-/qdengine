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

#define _NO_ZIP_
#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "common/debug.h"
#include "common/file.h"

#include "qdengine/qdengine.h"
#include "qdengine/qd_precomp.h"
#include "qdengine/system/graphics/gr_dispatcher.h"
#include "qdengine/system/graphics/gr_tile_animation.h"


namespace QDEngine {

CompressionProgressHandler grTileAnimation::_progressHandler;
void *grTileAnimation::_progressHandlerContext;

grTileAnimation::grTileAnimation() {
	clear();
}

void grTileAnimation::clear() {
	_hasAlpha = false;

	_compression = TILE_UNCOMPRESSED;

	_frameCount = 0;
	_frameSize = Vect2i(0, 0);
	_frameTileSize = Vect2i(0, 0);

	_frameIndex.clear();
	FrameIndex(_frameIndex).swap(_frameIndex);

	_tileOffsets.clear();
	TileOffsets(_tileOffsets).swap(_tileOffsets);

	_tileData.clear();
	TileData(_tileData).swap(_tileData);
}

void grTileAnimation::init(int frame_count, const Vect2i &frame_size, bool alpha_flag) {
	clear();

	_hasAlpha = alpha_flag;

	_frameSize = frame_size;

	_frameTileSize.x = (frame_size.x + GR_TILE_SPRITE_SIZE_X / 2) / GR_TILE_SPRITE_SIZE_X;
	_frameTileSize.y = (frame_size.y + GR_TILE_SPRITE_SIZE_Y / 2) / GR_TILE_SPRITE_SIZE_Y;

	_frameIndex.reserve(frame_count * _frameTileSize.x * _frameTileSize.y);

	_tileOffsets.reserve(frame_count * _frameTileSize.x * _frameTileSize.y + 1);
	_tileOffsets.push_back(0);

	_tileData.reserve(frame_count * _frameTileSize.x * _frameTileSize.y * GR_TILE_SPRITE_SIZE);

	_frameCount = frame_count;
}

void grTileAnimation::compact() {
	TileOffsets(_tileOffsets).swap(_tileOffsets);
	TileData(_tileData).swap(_tileData);
	debugC(3, kDebugLog, "Tile animation: %lu Kbytes", (_frameIndex.size() + _tileData.size() + _tileOffsets.size()) * 4 / 1024);
}

bool grTileAnimation::compress(grTileCompressionMethod method) {
	if (_compression != TILE_UNCOMPRESSED)
		return false;

	_compression = method;

	TileData tile_data;
	tile_data.reserve(_tileData.size());

	TileOffsets tile_offsets;
	tile_offsets.reserve(_tileOffsets.size());
	tile_offsets.push_back(0);

	TileData tile_vector = TileData(GR_TILE_SPRITE_SIZE * 4, 0);

	int count = tileCount();
	for (int i = 0; i < count; i++) {
		if (_progressHandler) {
			int percent_done = 100 * (i + 1) / count;
			(*_progressHandler)(percent_done, _progressHandlerContext);
		}

		unsigned *data = &*_tileData.begin() + i * GR_TILE_SPRITE_SIZE;

		unsigned offs = tile_offsets.back();
		unsigned sz = grTileSprite::compress(data, &*tile_vector.begin(), method);
		tile_data.insert(tile_data.end(), tile_vector.begin(), tile_vector.begin() + sz);
		tile_offsets.push_back(offs + sz);
	}

	_tileData.swap(tile_data);
	_tileOffsets.swap(tile_offsets);

	return true;
}


grTileSprite grTileAnimation::getTile(int tile_index) const {
	debugC(3, kDebugTemp, "The tile index is given by %d", tile_index);
	static unsigned tile_buf[GR_TILE_SPRITE_SIZE];

	switch (_compression) {
	case TILE_UNCOMPRESSED:
		return grTileSprite(&*_tileData.begin() + _tileOffsets[tile_index]);
	default:
		if (!grTileSprite::uncompress(&*_tileData.begin() + _tileOffsets[tile_index], GR_TILE_SPRITE_SIZE, tile_buf, _compression)) {
			assert(0 && "Unknown compression algorithm");
		}
		return grTileSprite(tile_buf);
	}
}

void grTileAnimation::addFrame(const unsigned *frame_data) {
	TileData tile_vector = TileData(GR_TILE_SPRITE_SIZE, 0);
	TileData tile_vector2 = TileData(GR_TILE_SPRITE_SIZE * 4, 0);

	if (_progressHandler) {
		int percent_done = 100 * (_frameIndex.size() / (_frameTileSize.x * _frameTileSize.y) + 1) / (_frameCount ? _frameCount : 1);
		(*_progressHandler)(percent_done, _progressHandlerContext);
	}

	for (int i = 0; i < _frameTileSize.y; i++) {
		for (int j = 0; j < _frameTileSize.x; j++) {
			std::fill(tile_vector.begin(), tile_vector.end(), 0);

			const unsigned *data_ptr = frame_data + j * GR_TILE_SPRITE_SIZE_X
			                           + i * GR_TILE_SPRITE_SIZE_Y * _frameSize.x;

			unsigned *tile_ptr = &tile_vector[0];
			for (int y = 0; y < GR_TILE_SPRITE_SIZE_Y; y++) {
				if (y + i * GR_TILE_SPRITE_SIZE_Y >= _frameSize.y) break;

				for (int x = 0; x < GR_TILE_SPRITE_SIZE_X; x++) {
					if (x + j * GR_TILE_SPRITE_SIZE_X >= _frameSize.x) break;
					tile_ptr[x] = data_ptr[x];
				}

				data_ptr += _frameSize.x;
				tile_ptr += GR_TILE_SPRITE_SIZE_X;
			}

			int tile_id = -1;
			int tile_count = tileCount();
			for (int tile_idx = 0; tile_idx < tile_count; tile_idx++) {
				grTileSprite tile = getTile(tile_idx);
				if (tile == grTileSprite(&tile_vector[0])) {
					tile_id = tile_idx;
					break;
				}
			}

			if (tile_id == -1) {
				unsigned sz = GR_TILE_SPRITE_SIZE;
				unsigned offs = _tileOffsets.back();

				_tileData.insert(_tileData.end(), tile_vector.begin(), tile_vector.end());
				_tileOffsets.push_back(offs + sz);
				_frameIndex.push_back(tile_count);
			} else
				_frameIndex.push_back(tile_id);
		}
	}
}

bool grTileAnimation::load(Common::SeekableReadStream *fh) {

	debugC(7, kDebugLoad, "grTileAnimation::load(): pos start: %lu", fh->pos());

	_frameCount = fh->readSint32LE();
	_frameSize.x = fh->readSint32LE();
	_frameSize.y = fh->readSint32LE();
	_frameTileSize.x = fh->readSint32LE();
	_frameTileSize.y = fh->readSint32LE();
	uint32 size = fh->readUint32LE();

	debugC(7, kDebugLoad, "grTileAnimation::load(): frameCount: %d  frame: %d x %d tile: %d x %d compsize: %d", _frameCount, _frameSize.x, _frameSize.y,
		_frameTileSize.x, _frameTileSize.y, size);

	_compression = grTileCompressionMethod(size);

	size = fh->readUint32LE();
	_frameIndex.resize(size);
	debugC(7, kDebugLoad, "grTileAnimation::load(): pos: %ld _frameIndex size: %u", fh->pos() - 4, size);
	for (int i = 0; i < size; i++) {
		_frameIndex[i] = fh->readUint32LE();
		debugCN(8, kDebugLoad, " %d ", _frameIndex[i]);
	}
	debugCN(8, kDebugLoad, "\n");

	size = fh->readUint32LE();
	_tileOffsets.resize(size);
	for (int i = 0; i < size; i++) {
		_tileOffsets[i] = fh->readUint32LE();
	}

	size = fh->readUint32LE();
	_tileData.resize(size);
	for (int i = 0; i < size; i++) {
		_tileData[i] = fh->readUint32LE();
	}

	return true;
}

void grTileAnimation::drawFrame(const Vect2i &position, int32 frame_index, int32 mode) const {
	Vect2i pos0 = position - _frameSize / 2;

	int32 dx = GR_TILE_SPRITE_SIZE_X;
	int32 dy = GR_TILE_SPRITE_SIZE_Y;

	if (mode & GR_FLIP_HORIZONTAL) {
		pos0.x += _frameSize.x - GR_TILE_SPRITE_SIZE_X;
		dx = -dx;
	}
	if (mode & GR_FLIP_VERTICAL) {
		pos0.y += _frameSize.y - GR_TILE_SPRITE_SIZE_Y;
		dy = -dy;
	}

//	grDispatcher::instance()->Rectangle(position.x - _frameSize.x/2, position.y - _frameSize.y/2, _frameSize.x, _frameSize.y, 0xFFFFF, 0, GR_OUTLINED);

	const uint32 *index_ptr = &_frameIndex[0] + _frameTileSize.x * _frameTileSize.y * frame_index;

	Vect2i pos = pos0;
	for (int32 i = 0; i < _frameTileSize.y; i++) {
		pos.x = pos0.x;

		for (int32 j = 0; j < _frameTileSize.x; j++) {
			grDispatcher::instance()->PutTileSpr(pos.x, pos.y, getTile(*index_ptr++), _hasAlpha, mode);
			pos.x += dx;
		}

		pos.y += dy;
	}
}

void grTileAnimation::drawFrame(const Vect2i &position, int frame_index, float angle, int mode) const {
	unsigned char *buf = (unsigned char *)grDispatcher::instance()->temp_buffer(_frameSize.x * _frameSize.y * 4);

	const unsigned *index_ptr = &_frameIndex[0] + _frameTileSize.x * _frameTileSize.y * frame_index;

	for (int i = 0; i < _frameTileSize.y; i++) {
		for (int j = 0; j < _frameTileSize.x; j++) {
			unsigned char *buf_ptr = buf + (i * _frameSize.x + j) * 4;
			const unsigned char *data_ptr = (const unsigned char *)getTile(*index_ptr++).data();
			int dx = MIN(_frameSize.x - j * GR_TILE_SPRITE_SIZE_X, GR_TILE_SPRITE_SIZE_X) * 4;
			for (int k = 0; k < GR_TILE_SPRITE_SIZE_Y; k++) {
				memcpy(buf_ptr, data_ptr, dx);
				data_ptr += GR_TILE_SPRITE_SIZE_X * 4;
				buf_ptr += _frameSize.x * 4;
			}
		}
	}

	grDispatcher::instance()->PutSpr_rot(position, _frameSize, buf, _hasAlpha, mode, angle);
}

} // namespace QDEngine
