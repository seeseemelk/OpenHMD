#ifndef RIFT_S_CAMERA_H_
#define RIFT_S_CAMERA_H_

#include "rift-s.h"

#include "libuvc/libuvc.h"

typedef struct rift_s_camera_s rift_s_camera_t;

struct rift_s_camera_s {
	uvc_context_t *ctx;
	uvc_device_t *dev;
	uvc_device_handle_t *devh;
	//uvc_stream_ctrl_t ctrl;
};

int rift_s_camera_open(rift_s_hmd_t *hmd);
void rift_s_camera_close(rift_s_hmd_t *hmd);

#endif
