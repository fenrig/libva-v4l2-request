/* SPDX-License-Identifier: GPL-2.0 */
/*
 * These are the HEVC state controls for use with stateless HEVC
 * codec drivers.
 *
 * It turns out that these structs are not stable yet and will undergo
 * more changes. So keep them private until they are stable and ready to
 * become part of the official public API.
 */

#ifndef _HEVC_CTRLS_H_
#define _HEVC_CTRLS_H_

/* The pixel format isn't stable at the moment and will likely be renamed. */
#define V4L2_PIX_FMT_HEVC_SLICE v4l2_fourcc('S', '2', '6', '5') /* HEVC parsed slices */

#define V4L2_CID_MPEG_VIDEO_HEVC_SPS		(V4L2_CID_MPEG_BASE + 1008)
#define V4L2_CID_MPEG_VIDEO_HEVC_PPS		(V4L2_CID_MPEG_BASE + 1009)
#define V4L2_CID_MPEG_VIDEO_HEVC_SLICE_PARAMS	(V4L2_CID_MPEG_BASE + 1010)

/* enum v4l2_ctrl_type type values */
#define V4L2_CTRL_TYPE_HEVC_SPS 0x0120
#define V4L2_CTRL_TYPE_HEVC_PPS 0x0121
#define V4L2_CTRL_TYPE_HEVC_SLICE_PARAMS 0x0122

#define V4L2_HEVC_SLICE_TYPE_B	0
#define V4L2_HEVC_SLICE_TYPE_P	1
#define V4L2_HEVC_SLICE_TYPE_I	2

#define V4L2_HEVC_DPB_ENTRY_RPS_ST_CURR_BEFORE	0x01
#define V4L2_HEVC_DPB_ENTRY_RPS_ST_CURR_AFTER	0x02
#define V4L2_HEVC_DPB_ENTRY_RPS_LT_CURR		0x03

#define V4L2_HEVC_DPB_ENTRIES_NUM_MAX		16

#endif
