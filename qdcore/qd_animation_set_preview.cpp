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
#include "common/system.h"

#include "qdengine/qd_precomp.h"
#include "qdengine/qdcore/qd_animation_set.h"
#include "qdengine/qdcore/qd_animation_set_preview.h"
#include "qdengine/qdcore/qd_camera.h"


namespace QDEngine {

qdAnimationSetPreview::qdAnimationSetPreview(qdAnimationSet *p) :
	graph_d_(0),
	_animation_set(p),
	preview_mode_(VIEW_WALK_ANIMATIONS) {
	_animation = new qdAnimation;

	camera_ = new qdCamera;
	camera_->set_focus(2000.0f);
	camera_angle_ = 45;

	_start_time = 0;
	back_color_ = 0x000000FF;
	grid_color_ = 0x00FF00FF;

	set_direction(0);

	cell_size_ = 100;

	_personage_height = float(_animation->size_y());

	p->load_animations(NULL);
}

qdAnimationSetPreview::~qdAnimationSetPreview() {
	_animation_set->free_animations(NULL);

	delete _animation;
	delete camera_;
}

void qdAnimationSetPreview::set_graph_dispatcher(grDispatcher *p) {
	graph_d_ = p;
	set_screen(Vect2s(0, 0), Vect2s(p->Get_SizeX(), p->Get_SizeY()));
}

void qdAnimationSetPreview::start() {
	_start_time = g_system->getMillis();

	_animation->start();
	cell_offset_ = 0.0f;
}

void qdAnimationSetPreview::quant() {
	int time = g_system->getMillis();
	float tm = float(time - _start_time) / 1000.0f;
	_start_time = time;

	if (tm >= 0.3f) return;

	quant(tm);
}

void qdAnimationSetPreview::quant(float tm) {
	_animation->quant(tm);

	cell_offset_ -= _personage_speed * tm;
	while (cell_offset_ <= -float(cell_size_)) cell_offset_ += float(cell_size_);
}

void qdAnimationSetPreview::redraw() {
	grDispatcher *gp = grDispatcher::set_instance(graph_d_);

	grDispatcher::instance()->Fill(back_color_);

	redraw_grid();

	Vect2s v = camera_->global2scr(Vect3f(0.0f, 0.0f, _personage_height / 2.0f));
	float scale = camera_->get_scale(Vect3f(0.0f, 0.0f, _personage_height / 2.0f));
	_animation->redraw(v.x, v.y, scale);

	Vect2s v0 = camera_->global2scr(Vect3f(0.0f, 0.0f, _personage_height));
	Vect2s v1 = camera_->global2scr(Vect3f(0.0f, 0.0f, 0.0f));

	const int rect_sz = 4;
	grDispatcher::instance()->Rectangle(v.x - rect_sz / 2, v.y - rect_sz / 2, rect_sz, rect_sz, grid_color_, grid_color_, GR_FILLED);

	const int line_sz = 10;
	grDispatcher::instance()->Line(v0.x - line_sz, v0.y, v0.x + line_sz, v0.y, grid_color_);
	grDispatcher::instance()->Line(v1.x - line_sz, v1.y, v1.x + line_sz, v1.y, grid_color_);
	grDispatcher::instance()->Line(v0.x, v0.y, v1.x, v1.y, grid_color_);
	grDispatcher::instance()->Rectangle(screen_offset_.x, screen_offset_.y, screen_size_.x, screen_size_.y, grid_color_, 0, GR_OUTLINED);
	grDispatcher::instance()->Flush();

	grDispatcher::set_instance(gp);
}

bool qdAnimationSetPreview::set_direction(int dir) {
	bool result = false;

	_animation->clear();
	_personage_speed = 0.0f;
	_direction = dir;
	cell_offset_ = 0.0f;

	float angle = _animation_set->get_index_angle(_direction) * 180.0f / M_PI;
	camera_->rotate_and_scale(camera_angle_, 0, angle, 1.0f, 1.0f, 1.0f);

	qdAnimationInfo *p = NULL;
	if (preview_mode_ == VIEW_WALK_ANIMATIONS)
		p = _animation_set->get_animation_info(dir);
	else if (preview_mode_ == VIEW_STATIC_ANIMATIONS)
		p = _animation_set->get_static_animation_info(dir);
	else if (preview_mode_ == VIEW_START_ANIMATIONS)
		p = _animation_set->get_start_animation_info(dir);
	else if (preview_mode_ == VIEW_STOP_ANIMATIONS)
		p = _animation_set->get_stop_animation_info(dir);

	if (p) {
		qdAnimation *ap = p->animation();
		if (ap) {
			ap->create_reference(_animation, p);
			result = true;
		}
		_personage_speed = p->speed();
	}

	_animation->start();

	return result;
}

void qdAnimationSetPreview::set_cell_size(int sz) {
	cell_size_ = sz;

	_animation->start();
	cell_offset_ = 0.0f;
}

void qdAnimationSetPreview::set_screen(Vect2s offs, Vect2s size) {
	if (!graph_d_) return;

	screen_offset_ = offs;
	screen_size_ = size;

	camera_->set_scr_size(size.x, size.y);
	camera_->set_scr_center(offs.x + size.x / 2, offs.y + size.y * 3 / 4);

	graph_d_->SetClip(offs.x, offs.y, offs.x + size.x, offs.y + size.y);
}

void qdAnimationSetPreview::redraw_grid() {
	float size = 0;
	Vect2f p = camera_->scr2plane(screen_offset_);
	if (fabs(p.x) > size) size = fabs(p.x);
	if (fabs(p.y) > size) size = fabs(p.y);
	p = camera_->scr2plane(screen_offset_ + screen_size_);
	if (fabs(p.x) > size) size = fabs(p.x);
	if (fabs(p.y) > size) size = fabs(p.y);
	p = camera_->scr2plane(Vect2s(screen_offset_.x + screen_size_.x, screen_offset_.y));
	if (fabs(p.x) > size) size = fabs(p.x);
	if (fabs(p.y) > size) size = fabs(p.y);
	p = camera_->scr2plane(Vect2s(screen_offset_.x, screen_offset_.y + screen_size_.y));
	if (fabs(p.x) > size) size = fabs(p.x);
	if (fabs(p.y) > size) size = fabs(p.y);

	int sz = round(size) + cell_size_;
	sz -= sz % cell_size_;
	for (int i = -sz; i <= sz; i += cell_size_) {
		int dx = round(cell_offset_);

		Vect3f v00 = camera_->global2camera_coord(Vect3f(i + dx, -sz, 0));
		Vect3f v10 = camera_->global2camera_coord(Vect3f(i + dx, sz, 0));
		if (camera_->line_cutting(v00, v10)) {
			Vect2s v0 = camera_->camera_coord2scr(v00);
			Vect2s v1 = camera_->camera_coord2scr(v10);
			grDispatcher::instance()->Line(v0.x, v0.y, v1.x, v1.y, grid_color_, 0);
		}

		v00 = camera_->global2camera_coord(Vect3f(-sz + dx, i, 0));
		v10 = camera_->global2camera_coord(Vect3f(sz + dx, i, 0));
		if (camera_->line_cutting(v00, v10)) {
			Vect2s v0 = camera_->camera_coord2scr(v00);
			Vect2s v1 = camera_->camera_coord2scr(v10);
			grDispatcher::instance()->Line(v0.x, v0.y, v1.x, v1.y, grid_color_, 0);
		}
		/*
		        v0 = camera_->global2scr(Vect3f(-sz + dx,i,0));
		        v1 = camera_->global2scr(Vect3f(sz + dx,i,0));
		        grDispatcher::instance()->Line(v0.x,v0.y,v1.x,v1.y,grid_color_,0);
		*/
	}
}

void qdAnimationSetPreview::set_camera_angle(int ang) {
	camera_angle_ = ang;

	float angle = _animation_set->get_index_angle(_direction) * 180.0f / M_PI;
	camera_->rotate_and_scale(camera_angle_, 0, angle, 1.0f, 1.0f, 1.0f);
}

float qdAnimationSetPreview::camera_focus() {
	return camera_->get_focus();
}

void qdAnimationSetPreview::set_camera_focus(float f) {
	camera_->set_focus(f);
}

bool qdAnimationSetPreview::set_phase(float phase) {
	if (!_animation->is_empty()) {
		if (!_animation->is_playing())
			_animation->start();

		cell_offset_ = 0.0f;
		_animation->set_time(0.0f);
		quant(_animation->length() * phase);
		return true;
	}

	return false;
}
} // namespace QDEngine
