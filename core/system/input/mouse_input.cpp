/* ---------------------------- INCLUDE SECTION ----------------------------- */
#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "common/textconsole.h"
#include "qdengine/core/qd_precomp.h"
#include "qdengine/core/system/input/mouse_input.h"
#include "qdengine/core/system/graphics/gr_dispatcher.h"
/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

mouseDispatcher::mouseDispatcher() : events_(0), active_events_(0), mouse_x_(0), mouse_y_(0), button_status_(0) {
	for (int i = 0; i < EV_MOUSE_MOVE + 1; i++)
		event_handlers_[i] = 0;
}

mouseDispatcher::~mouseDispatcher() {
}

mouseDispatcher *mouseDispatcher::instance() {
	static mouseDispatcher dsp;
	return &dsp;
}

bool mouseDispatcher::handle_event(mouseEvent ev, int x, int y, int flags) {
	if (x >= grDispatcher::instance()->Get_SizeX())
		x = grDispatcher::instance()->Get_SizeX() - 1;
	if (y >= grDispatcher::instance()->Get_SizeY())
		y = grDispatcher::instance()->Get_SizeY() - 1;

	if (event_handlers_[ev])
		(*event_handlers_[ev])(x, y, flags);

	warning("STUB: mouseDispatcher::handle_event()");
#if 0
	if (flags & MK_LBUTTON) button_status_ |= 1 << (ID_BUTTON_LEFT);
	else button_status_ &= ~(1 << ID_BUTTON_LEFT);

	if (flags & MK_MBUTTON) button_status_ |= 1 << (ID_BUTTON_MIDDLE);
	else button_status_ &= ~(1 << ID_BUTTON_MIDDLE);

	if (flags & MK_RBUTTON) button_status_ |= 1 << (ID_BUTTON_RIGHT);
	else button_status_ &= ~(1 << ID_BUTTON_RIGHT);

	toggle_event(ev);
	mouse_x_ = x;
	mouse_y_ = y;

	return true;
#endif
}
