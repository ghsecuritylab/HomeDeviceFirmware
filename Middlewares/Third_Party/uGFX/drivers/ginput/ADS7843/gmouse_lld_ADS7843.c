/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if (GFX_USE_GINPUT && GINPUT_NEED_MOUSE)

#define GMOUSE_DRIVER_VMT		GMOUSEVMT_ADS7843
#include "../../../src/ginput/ginput_driver_mouse.h"

// Get the hardware interface
#include "gmouse_lld_ADS7843_board.h"

#define CMD_X				0xD1
#define CMD_Y				0x91
#define CMD_ENABLE_IRQ		0x80

extern bool_t LoadMouseCalibration(unsigned instance, void *data, size_t sz);
extern bool_t SaveMouseCalibration(unsigned instance, const void *data, size_t sz);

static bool_t MouseXYZ(GMouse* m, GMouseReading* pdr)
{
    (void)m;

    // No buttons
    pdr->buttons = 0;
    pdr->z = 0;

    if (getpin_pressed(m)) {
        pdr->z = 1;                     // Set to Z_MAX as we are pressed

        aquire_bus(m);
        get_raw(m, &pdr->x, &pdr->y);
        release_bus(m);
    }
    return TRUE;
}

const GMouseVMT const GMOUSE_DRIVER_VMT[1] = {{
	{
		GDRIVER_TYPE_TOUCH,
		GMOUSE_VFLG_TOUCH | GMOUSE_VFLG_CALIBRATE | GMOUSE_VFLG_CAL_TEST |
			GMOUSE_VFLG_ONLY_DOWN | GMOUSE_VFLG_POORUPDOWN,
		sizeof(GMouse)+GMOUSE_ADS7843_BOARD_DATA_SIZE,
		_gmouseInitDriver,
		_gmousePostInitDriver,
		_gmouseDeInitDriver
	},
	1,				// z_max - (currently?) not supported
	0,				// z_min - (currently?) not supported
	1,				// z_touchon
	0,				// z_touchoff
	{				// pen_jitter
		GMOUSE_ADS7843_PEN_CALIBRATE_ERROR,			// calibrate
		GMOUSE_ADS7843_PEN_CLICK_ERROR,				// click
		GMOUSE_ADS7843_PEN_MOVE_ERROR				// move
	},
	{				// finger_jitter
		GMOUSE_ADS7843_FINGER_CALIBRATE_ERROR,		// calibrate
		GMOUSE_ADS7843_FINGER_CLICK_ERROR,			// click
		GMOUSE_ADS7843_FINGER_MOVE_ERROR			// move
	},
	init_board, 	// init
	0,				// deinit
	MouseXYZ,		// get
	SaveMouseCalibration,				// calsave
	LoadMouseCalibration				// calload
}};

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */

