/*
 * Copyright (C) 2007 Intel Corporation
 * Copyright (C) 2016 Florent Revest <florent.revest@free-electrons.com>
 * Copyright (C) 2018 Paul Kocialkowski <paul.kocialkowski@bootlin.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mpeg2.h"
#include "context.h"
#include "request.h"
#include "surface.h"

#include <assert.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/videodev2.h>
#include <hevc-ctrls.h>

#include "v4l2.h"

#define H265_NAL_UNIT_TYPE_SHIFT		1
#define H265_NAL_UNIT_TYPE_MASK			((1 << 6) - 1)
#define H265_NUH_TEMPORAL_ID_PLUS1_SHIFT	0
#define H265_NUH_TEMPORAL_ID_PLUS1_MASK		((1 << 3) - 1)

static void h265_fill_pps(VAPictureParameterBufferHEVC *picture,
			  VASliceParameterBufferHEVC *slice,
			  struct v4l2_ctrl_hevc_pps *pps)
{
	memset(pps, 0, sizeof(*pps));

	/* TO DO: h265 pps assigments*/
}

static void h265_fill_sps(VAPictureParameterBufferHEVC *picture,
			  struct v4l2_ctrl_hevc_sps *sps)
{
	memset(sps, 0, sizeof(*sps));

	/* TO DO: h265 sps assigments */
}

static void h265_fill_slice_params(VAPictureParameterBufferHEVC *picture,
				   VASliceParameterBufferHEVC *slice,
				   struct object_heap *surface_heap,
				   void *source_data,
				   struct v4l2_ctrl_hevc_slice_params *slice_params)
{
	struct object_surface *surface_object;
	VAPictureHEVC *hevc_picture;
	uint8_t nal_unit_type;
	uint8_t nuh_temporal_id_plus1;
	uint32_t data_bit_offset;
	uint8_t pic_struct;
	uint8_t field_pic;
	uint8_t slice_type;
	unsigned int num_active_dpb_entries;
	unsigned int num_rps_poc_st_curr_before;
	unsigned int num_rps_poc_st_curr_after;
	unsigned int num_rps_poc_lt_curr;
	uint8_t *b;
	unsigned int count;
	unsigned int o, i, j;

	/* Extract the missing NAL header information. */

	b = source_data + slice->slice_data_offset;

	nal_unit_type = (b[0] >> H265_NAL_UNIT_TYPE_SHIFT) &
			H265_NAL_UNIT_TYPE_MASK;
	nuh_temporal_id_plus1 = (b[1] >> H265_NUH_TEMPORAL_ID_PLUS1_SHIFT) &
				H265_NUH_TEMPORAL_ID_PLUS1_MASK;

	/*
	 * VAAPI only provides a byte-aligned value for the slice segment data
	 * offset, although it appears that the offset is not always aligned.
	 * Search for the first one bit in the previous byte, that marks the
	 * start of the slice segment to correct the value.
	 */

	b = source_data + (slice->slice_data_offset +
			   slice->slice_data_byte_offset) - 1;

	for (o = 0; o < 8; o++)
		if (*b & (1 << o))
			break;

	/* Include the one bit. */
	o++;

	data_bit_offset = (slice->slice_data_offset +
			   slice->slice_data_byte_offset) * 8 - o;

	memset(slice_params, 0, sizeof(*slice_params));
	/* TO DO slice params assigments */
}

int h265_set_controls(struct request_data *driver_data,
		      struct object_context *context_object,
		      struct object_surface *surface_object)
{
	VAPictureParameterBufferHEVC *picture =
		&surface_object->params.h265.picture;
	VASliceParameterBufferHEVC *slice =
		&surface_object->params.h265.slice;
	VAIQMatrixBufferHEVC *iqmatrix =
		&surface_object->params.h265.iqmatrix;
	bool iqmatrix_set = surface_object->params.h265.iqmatrix_set;
	struct v4l2_ctrl_hevc_pps pps;
	struct v4l2_ctrl_hevc_sps sps;
	struct v4l2_ctrl_hevc_slice_params slice_params;
	int rc;

	h265_fill_pps(picture, slice, &pps);

	rc = v4l2_set_control(driver_data->video_fd, surface_object->request_fd,
			      V4L2_CID_MPEG_VIDEO_HEVC_PPS, &pps, sizeof(pps));
	if (rc < 0)
		return VA_STATUS_ERROR_OPERATION_FAILED;

	h265_fill_sps(picture, &sps);

	rc = v4l2_set_control(driver_data->video_fd, surface_object->request_fd,
			      V4L2_CID_MPEG_VIDEO_HEVC_SPS, &sps, sizeof(sps));
	if (rc < 0)
		return VA_STATUS_ERROR_OPERATION_FAILED;

	h265_fill_slice_params(picture, slice, &driver_data->surface_heap,
			       surface_object->source_data, &slice_params);

	rc = v4l2_set_control(driver_data->video_fd, surface_object->request_fd,
			      V4L2_CID_MPEG_VIDEO_HEVC_SLICE_PARAMS,
			      &slice_params, sizeof(slice_params));
	if (rc < 0)
		return VA_STATUS_ERROR_OPERATION_FAILED;

	return 0;
}
