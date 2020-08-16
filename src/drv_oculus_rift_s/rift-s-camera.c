#include "rift-s-camera.h"

#include "rift-s-hmd.h"
#include "rift-s-view.h"

#include <libuvc/libuvc.h>
#include <stdlib.h>
#include <unistd.h>

static void rift_s_camera_callback(uvc_frame_t *frame, void *ptr) {
	uvc_error_t res;
	LOGI("Received Camera frame");
	rift_s_show_frame(frame);
}

static uvc_error_t rift_s_camera_find(uvc_context_t *ctx, uvc_device_t **dev) {
	uvc_error_t res;
	for (int i = 0; i < 5; i++) {
		res = uvc_find_device(
			ctx,
			dev,
			0, // Vendor ID
			0, // Product ID
			NULL // Serial number
		);
		if (res == UVC_SUCCESS) {
			return res;
		}
	}
	return res;
}

static uvc_error_t rift_s_camera_do_open(rift_s_hmd_t *hmd) {
	hmd->cameras.ctx = NULL;
	hmd->cameras.dev = NULL;
	hmd->cameras.devh = NULL;

	uvc_error_t res = uvc_init(&hmd->cameras.ctx, NULL);
	if (res != UVC_SUCCESS) {
		LOGE("Could not initialize UVC context: %s", uvc_strerror(res));
		return res;
	}

	res = rift_s_camera_find(
		hmd->cameras.ctx,
		&hmd->cameras.dev
	);
	if (res != UVC_SUCCESS) {
		LOGE("Could not find UVC device: %s", uvc_strerror(res));
		return res;
	}

	res = uvc_open(hmd->cameras.dev, &hmd->cameras.devh);
	if (res != UVC_SUCCESS) {
		LOGE("Could not open UVC device: %s", uvc_strerror(res));
		return res;
	}

	uvc_print_diag(hmd->cameras.devh, stdout);

	uvc_stream_ctrl_t ctrl;
	res = uvc_get_stream_ctrl_format_size(
		hmd->cameras.devh,
		&ctrl,
		UVC_FRAME_FORMAT_MJPEG,
		3200,
		856,
		60
	);
	if (res != UVC_SUCCESS) {
		LOGE("Could not set UVC format: %s", uvc_strerror(res));
		return res;
	}

	uvc_print_stream_ctrl(&ctrl, stdout);

	res = uvc_start_streaming(
		hmd->cameras.devh,
		&ctrl,
		&rift_s_camera_callback,
		hmd, 0
	);
	if (res != UVC_SUCCESS) {
		LOGE("Could not start streaming: %s", uvc_strerror(res));
		return res;
	}

	return 0;
}

int rift_s_camera_open(rift_s_hmd_t *hmd) {
	uvc_error_t res;
	//for (int i = 0; i < 3; i++) {
		res = rift_s_camera_do_open(hmd);
		/*if (res == UVC_SUCCESS) {
			return res;
		} else {
			rift_s_camera_close(hmd);
			sleep(1);
		}
	}*/
	return res;

//fail_open:
	//uvc_unref_device(hmd->cameras.dev);
//fail_find_device:
	//uvc_exit(hmd->cameras.ctx);
}

void rift_s_camera_close(rift_s_hmd_t *hmd) {
	LOGI("Closing UVC devices and context");
	if (hmd->cameras.devh != NULL) {
		uvc_close(hmd->cameras.devh);
		hmd->cameras.devh = NULL;
	}
	if (hmd->cameras.dev != NULL) {
		uvc_unref_device(hmd->cameras.dev);
		hmd->cameras.dev = NULL;
	}
	if (hmd->cameras.ctx != NULL) {
		uvc_exit(hmd->cameras.ctx);
		hmd->cameras.ctx = NULL;
	}
}
