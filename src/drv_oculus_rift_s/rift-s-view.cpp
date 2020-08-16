extern "C" {
	#include "rift-s-view.h"
}

#include <opencv2/core/core_c.h>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui_c.h>

extern "C" void rift_s_show_frame(uvc_frame_t *frame) {
	IplImage *cvImg = cvCreateImageHeader(
		cvSize(frame->width, frame->height),
		IPL_DEPTH_8U,
		3
	);

	cvSetData(cvImg, frame->data, frame->width * 3);
	cvNamedWindow("Rift S", CV_WINDOW_AUTOSIZE);
	cvShowImage("Rift S", cvImg);
	cvWaitKey(5);
	cvReleaseImageHeader(&cvImg);
}
