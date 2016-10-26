/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "webrtc/common_video/h264/h264_bitstream_parser.h"

#include <memory>
#include <vector>

#include "webrtc/base/bitbuffer.h"
#include "webrtc/base/bytebuffer.h"
#include "webrtc/base/checks.h"

#include "webrtc/common_video/h264/h264_common.h"
#include "webrtc/base/logging.h"

namespace webrtc {

#define RETURN_FALSE_ON_FAIL(x)       \
  if (!(x)) {                         \
    LOG_F(LS_ERROR) << "FAILED: " #x; \
    return false;                     \
  }

H264BitstreamParser::H264BitstreamParser() {}
H264BitstreamParser::~H264BitstreamParser() {}

bool H264BitstreamParser::ParseNonParameterSetNalu(const uint8_t* source,
                                                   size_t source_length,
                                                   uint8_t nalu_type) {
  RTC_CHECK(sps_);
  RTC_CHECK(pps_);
  last_slice_qp_delta_ = rtc::Optional<int32_t>();
  std::unique_ptr<rtc::Buffer> slice_rbsp(
      H264::ParseRbsp(source, source_length));
  rtc::BitBuffer slice_reader(slice_rbsp->data() + H264::kNaluTypeSize,
                              slice_rbsp->size() - H264::kNaluTypeSize);
  // Check to see if this is an IDR slice, which has an extra field to parse
  // out.
  bool is_idr = (source[0] & 0x0F) == H264::NaluType::kIdr;
  uint8_t nal_ref_idc = (source[0] & 0x60) >> 5;
  uint32_t golomb_tmp;
  uint32_t bits_tmp;

  // first_mb_in_slice: ue(v)
  RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&golomb_tmp));
  // slice_type: ue(v)
  uint32_t slice_type;
  RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&slice_type));
  // slice_type's 5..9 range is used to indicate that all slices of a picture
  // have the same value of slice_type % 5, we don't care about that, so we map
  // to the corresponding 0..4 range.
  slice_type %= 5;
  // pic_parameter_set_id: ue(v)
  RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&golomb_tmp));
  if (sps_->separate_colour_plane_flag == 1) {
    // colour_plane_id
    RETURN_FALSE_ON_FAIL(slice_reader.ReadBits(&bits_tmp, 2));
  }
  // frame_num: u(v)
  // Represented by log2_max_frame_num_minus4 + 4 bits.
  RETURN_FALSE_ON_FAIL(
      slice_reader.ReadBits(&bits_tmp, sps_->log2_max_frame_num_minus4 + 4));
  uint32_t field_pic_flag = 0;
  if (sps_->frame_mbs_only_flag == 0) {
    // field_pic_flag: u(1)
    RETURN_FALSE_ON_FAIL(slice_reader.ReadBits(&field_pic_flag, 1));
    if (field_pic_flag != 0) {
      // bottom_field_flag: u(1)
      RETURN_FALSE_ON_FAIL(slice_reader.ReadBits(&bits_tmp, 1));
    }
  }
  if (is_idr) {
    // idr_pic_id: ue(v)
    RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&golomb_tmp));
  }
  // pic_order_cnt_lsb: u(v)
  // Represented by sps_.log2_max_pic_order_cnt_lsb_minus4 + 4 bits.
  if (sps_->pic_order_cnt_type == 0) {
    RETURN_FALSE_ON_FAIL(slice_reader.ReadBits(
        &bits_tmp, sps_->log2_max_pic_order_cnt_lsb_minus4 + 4));
    if (pps_->bottom_field_pic_order_in_frame_present_flag &&
        field_pic_flag == 0) {
      // delta_pic_order_cnt_bottom: se(v)
      RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&golomb_tmp));
    }
  }
  if (sps_->pic_order_cnt_type == 1 &&
      !sps_->delta_pic_order_always_zero_flag) {
    // delta_pic_order_cnt[0]: se(v)
    RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&golomb_tmp));
    if (pps_->bottom_field_pic_order_in_frame_present_flag && !field_pic_flag) {
      // delta_pic_order_cnt[1]: se(v)
      RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&golomb_tmp));
    }
  }
  if (pps_->redundant_pic_cnt_present_flag) {
    // redundant_pic_cnt: ue(v)
    RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&golomb_tmp));
  }
  if (slice_type == H264::SliceType::kB) {
    // direct_spatial_mv_pred_flag: u(1)
    RETURN_FALSE_ON_FAIL(slice_reader.ReadBits(&bits_tmp, 1));
  }
  switch (slice_type) {
    case H264::SliceType::kP:
    case H264::SliceType::kB:
    case H264::SliceType::kSp:
      uint32_t num_ref_idx_active_override_flag;
      // num_ref_idx_active_override_flag: u(1)
      RETURN_FALSE_ON_FAIL(
          slice_reader.ReadBits(&num_ref_idx_active_override_flag, 1));
      if (num_ref_idx_active_override_flag != 0) {
        // num_ref_idx_l0_active_minus1: ue(v)
        RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&golomb_tmp));
        if (slice_type == H264::SliceType::kB) {
          // num_ref_idx_l1_active_minus1: ue(v)
          RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(&golomb_tmp));
        }
      }
      break;
    default:
      break;
  }
  // assume nal_unit_type != 20 && nal_unit_type != 21:
  RTC_CHECK_NE(nalu_type, 20);
  RTC_CHECK_NE(nalu_type, 21);
  // if (nal_unit_type == 20 || nal_unit_type == 21)
  //   ref_pic_list_mvc_modification()
  // else
  {
    // ref_pic_list_modification():
    // |slice_type| checks here don't use named constants as they aren't named
    // in the spec for this segment. Keeping them consistent makes it easier to
    // verify that they are both the same.
    if (slice_type % 5 != 2 && slice_type % 5 != 4) {
      // ref_pic_list_modification_flag_l0: u(1)
      uint32_t ref_pic_list_modification_flag_l0;
      RETURN_FALSE_ON_FAIL(
          slice_reader.ReadBits(&ref_pic_list_modification_flag_l0, 1));
      if (ref_pic_list_modification_flag_l0) {
        uint32_t modification_of_pic_nums_idc;
        do {
          // modification_of_pic_nums_idc: ue(v)
          RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(
              &modification_of_pic_nums_idc));
          if (modification_of_pic_nums_idc == 0 ||
              modification_of_pic_nums_idc == 1) {
            // abs_diff_pic_num_minus1: ue(v)
            RETURN_FALSE_ON_FAIL(
                slice_reader.ReadExponentialGolomb(&golomb_tmp));
          } else if (modification_of_pic_nums_idc == 2) {
            // long_term_pic_num: ue(v)
            RETURN_FALSE_ON_FAIL(
                slice_reader.ReadExponentialGolomb(&golomb_tmp));
          }
        } while (modification_of_pic_nums_idc != 3);
      }
    }
    if (slice_type % 5 == 1) {
      // ref_pic_list_modification_flag_l1: u(1)
      uint32_t ref_pic_list_modification_flag_l1;
      RETURN_FALSE_ON_FAIL(
          slice_reader.ReadBits(&ref_pic_list_modification_flag_l1, 1));
      if (ref_pic_list_modification_flag_l1) {
        uint32_t modification_of_pic_nums_idc;
        do {
          // modification_of_pic_nums_idc: ue(v)
          RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(
              &modification_of_pic_nums_idc));
          if (modification_of_pic_nums_idc == 0 ||
              modification_of_pic_nums_idc == 1) {
            // abs_diff_pic_num_minus1: ue(v)
            RETURN_FALSE_ON_FAIL(
                slice_reader.ReadExponentialGolomb(&golomb_tmp));
          } else if (modification_of_pic_nums_idc == 2) {
            // long_term_pic_num: ue(v)
            RETURN_FALSE_ON_FAIL(
                slice_reader.ReadExponentialGolomb(&golomb_tmp));
          }
        } while (modification_of_pic_nums_idc != 3);
      }
    }
  }
  // TODO(pbos): Do we need support for pred_weight_table()?
  RTC_CHECK(
      !((pps_->weighted_pred_flag && (slice_type == H264::SliceType::kP ||
                                      slice_type == H264::SliceType::kSp)) ||
        (pps_->weighted_bipred_idc != 0 && slice_type == H264::SliceType::kB)))
      << "Missing support for pred_weight_table().";
  // if ((weighted_pred_flag && (slice_type == P || slice_type == SP)) ||
  //    (weighted_bipred_idc == 1 && slice_type == B)) {
  //  pred_weight_table()
  // }
  if (nal_ref_idc != 0) {
    // dec_ref_pic_marking():
    if (is_idr) {
      // no_output_of_prior_pics_flag: u(1)
      // long_term_reference_flag: u(1)
      RETURN_FALSE_ON_FAIL(slice_reader.ReadBits(&bits_tmp, 2));
    } else {
      // adaptive_ref_pic_marking_mode_flag: u(1)
      uint32_t adaptive_ref_pic_marking_mode_flag;
      RETURN_FALSE_ON_FAIL(
          slice_reader.ReadBits(&adaptive_ref_pic_marking_mode_flag, 1));
      if (adaptive_ref_pic_marking_mode_flag) {
        uint32_t memory_management_control_operation;
        do {
          // memory_management_control_operation: ue(v)
          RETURN_FALSE_ON_FAIL(slice_reader.ReadExponentialGolomb(
              &memory_management_control_operation));
          if (memory_management_control_operation == 1 ||
              memory_management_control_operation == 3) {
            // difference_of_pic_nums_minus1: ue(v)
            RETURN_FALSE_ON_FAIL(
                slice_reader.ReadExponentialGolomb(&golomb_tmp));
          }
          if (memory_management_control_operation == 2) {
            // long_term_pic_num: ue(v)
            RETURN_FALSE_ON_FAIL(
                slice_reader.ReadExponentialGolomb(&golomb_tmp));
          }
          if (memory_management_control_operation == 3 ||
              memory_management_control_operation == 6) {
            // long_term_frame_idx: ue(v)
            RETURN_FALSE_ON_FAIL(
                slice_reader.ReadExponentialGolomb(&golomb_tmp));
          }
          if (memory_management_control_operation == 4) {
            // max_long_term_frame_idx_plus1: ue(v)
            RETURN_FALSE_ON_FAIL(
                slice_reader.ReadExponentialGolomb(&golomb_tmp));
          }
        } while (memory_management_control_operation != 0);
      }
    }
  }
  // cabac not supported: entropy_coding_mode_flag == 0 asserted above.
  // if (entropy_coding_mode_flag && slice_type != I && slice_type != SI)
  //   cabac_init_idc
  int32_t last_slice_qp_delta;
  RETURN_FALSE_ON_FAIL(
      slice_reader.ReadSignedExponentialGolomb(&last_slice_qp_delta));
  last_slice_qp_delta_ = rtc::Optional<int32_t>(last_slice_qp_delta);
  return true;
}

void H264BitstreamParser::ParseSlice(const uint8_t* slice, size_t length) {
  H264::NaluType nalu_type = H264::ParseNaluType(slice[0]);
  switch (nalu_type) {
    case H264::NaluType::kSps: {
      sps_ = SpsParser::ParseSps(slice + H264::kNaluTypeSize,
                                 length - H264::kNaluTypeSize);
      if (!sps_)
        FATAL() << "Unable to parse SPS from H264 bitstream.";
      break;
    }
    case H264::NaluType::kPps: {
      pps_ = PpsParser::ParsePps(slice + H264::kNaluTypeSize,
                                 length - H264::kNaluTypeSize);
      if (!pps_)
        FATAL() << "Unable to parse PPS from H264 bitstream.";
      break;
    }
    default:
      RTC_CHECK(ParseNonParameterSetNalu(slice, length, nalu_type))
          << "Failed to parse picture slice.";
      break;
  }
}

void H264BitstreamParser::ParseBitstream(const uint8_t* bitstream,
                                         size_t length) {
  std::vector<H264::NaluIndex> nalu_indices =
      H264::FindNaluIndices(bitstream, length);
  RTC_CHECK(!nalu_indices.empty());
  for (const H264::NaluIndex& index : nalu_indices)
    ParseSlice(&bitstream[index.payload_start_offset], index.payload_size);
}

bool H264BitstreamParser::GetLastSliceQp(int* qp) const {
  if (!last_slice_qp_delta_ || !pps_)
    return false;
  *qp = 26 + pps_->pic_init_qp_minus26 + *last_slice_qp_delta_;
  return true;
}

}  // namespace webrtc