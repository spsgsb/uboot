/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "minui.h"

// TODO: lose the function pointers.
typedef struct {
	// Initializes the backend and returns a GRSurface* to draw into.
	GRSurface* (*init)(void);

	// Causes the current drawing surface (returned by the most recent
	// call to flip() or init()) to be displayed, and returns a new
	// drawing surface.
	GRSurface* (*flip)(void);

	// Blank (or unblank) the screen.
	void (*blank)( bool);

	// Device cleanup when drawing is done.
	void (*exit)(void);
} minui_backend;

minui_backend* open_fbdev(void);

#endif
