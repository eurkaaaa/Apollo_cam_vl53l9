/*
 * vd6g_vtpatch.h
 *
 *  Created on: Oct 4, 2025
 *      Author: Lenovo
 */

#ifndef VD6G_VTPATCH_H_
#define VD6G_VTPATCH_H_
#define VT_REVISION 17
struct vtram_area {
	int offset;
	int size;
};

extern struct vtram_area vtpatch_desc[];
extern int vtpatch_area_nb;
extern const uint8_t vtpatch[];
#endif /* VD6G_VTPATCH_H_ */
