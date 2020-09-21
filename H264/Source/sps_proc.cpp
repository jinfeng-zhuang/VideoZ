#include "main.h"

struct sps sps;

int sps_proc(unsigned char *buffer, unsigned int length)
{
	int i;

	bitstream_init(buffer, length);

	memset(&sps, 0, sizeof(struct sps));

	sps.profile_idc = u(8);
	sps.constraint_set0_flag = u(1);
	sps.constraint_set1_flag = u(1);
	sps.constraint_set2_flag = u(1);
	sps.constraint_set3_flag = u(1);
	sps.constraint_set4_flag = u(1);
	sps.constraint_set5_flag = u(1);
	sps.reserved_zero_2bits = u(2);
	sps.level_idc = u(8);
	sps.seq_parameter_set_id = ue();

	if (sps.profile_idc == 100 || sps.profile_idc == 110 ||
		sps.profile_idc == 122 || sps.profile_idc == 244 || sps.profile_idc == 44 ||
		sps.profile_idc == 83 || sps.profile_idc == 86 || sps.profile_idc == 118 ||
		sps.profile_idc == 128 || sps.profile_idc == 138 || sps.profile_idc == 139 ||
		sps.profile_idc == 134 || sps.profile_idc == 135) {
		sps.chroma_format_idc = ue();
		if (3 == sps.chroma_format_idc)
			sps.separate_colour_plane_flag = u(1);
		sps.bit_depth_luma_minus8 = ue();
		sps.bit_depth_chroma_minus8 = ue();
		sps.qpprime_y_zero_transform_bypass_flag = u(1);
		sps.seq_scaling_matrix_present_flag = u(1);
		if (sps.seq_scaling_matrix_present_flag) {
			for (i = 0; i < ((sps.chroma_format_idc != 3) ? 8 : 12); i++) {
				sps.seq_scaling_list_present_flag[i] = u(1);
				if (sps.seq_scaling_list_present_flag[i]) {
					if (i < 6)
						printf("not implemented\n");
					else
						printf("not implemented2\n");
				}
			}
		}
	}

	sps.log2_max_frame_num_minus4 = ue();
	sps.pic_order_cnt_type = ue();
	if (sps.pic_order_cnt_type == 0)
		sps.log2_max_pic_order_cnt_lsb_minus4 = ue();
	else if (sps.pic_order_cnt_type == 1) {
		sps.delta_pic_order_always_zero_flag = u(1);
		sps.offset_for_non_ref_pic = se();
		sps.offset_for_top_to_bottom_field = se();
		sps.num_ref_frames_in_pic_order_cnt_cycle = ue();
		for (i = 0; i < (int)sps.num_ref_frames_in_pic_order_cnt_cycle; i++)
			printf("not implemented\n");
	}

	sps.max_num_ref_frames = ue();
	sps.gaps_in_frame_num_value_allowed_flag = u(1);
	sps.pic_width_in_mbs_minus1 = ue();
	sps.pic_height_in_map_units_minus1 = ue();
	sps.frame_mbs_only_flag = u(1);
	if (!sps.frame_mbs_only_flag)
		sps.mb_adaptive_frame_field_flag = u(1);

	sps.direct_8x8_inference_flag = u(1);
	sps.frame_cropping_flag = u(1);
	if (sps.frame_cropping_flag) {
		sps.frame_crop_left_offset = ue();
		sps.frame_crop_right_offset = ue();
		sps.frame_crop_top_offset = ue();
		sps.frame_crop_bottom_offset = ue();
	}

	sps.vui_parameters_present_flag = u(1);

	// vui parameters
	if (sps.vui_parameters_present_flag) {
		sps.vui.aspect_ratio_info_present_flag = u(1);
		if (sps.vui.aspect_ratio_info_present_flag) {
			sps.vui.aspect_ratio_idc = u(8);
			if (sps.vui.aspect_ratio_idc == Extended_SAR) {
				sps.vui.sar_width = u(16);
				sps.vui.sar_height = u(16);
			}
		}

		sps.vui.overscan_info_present_flag = u(1);
		if (sps.vui.overscan_info_present_flag)
			sps.vui.overscan_appropriate_flag = u(1);

		sps.vui.video_signal_type_present_flag = u(1);
		if (sps.vui.video_signal_type_present_flag) {
			sps.vui.video_format = u(3);
			sps.vui.video_full_range_flag = u(1);
			sps.vui.colour_description_present_flag = u(1);
			if (sps.vui.colour_description_present_flag) {
				sps.vui.colour_primaries = u(8);
				sps.vui.transfer_characteristics = u(8);
				sps.vui.matrix_coefficients = u(8);
			}
		}

		sps.vui.chroma_loc_info_present_flag = u(1);
		if (sps.vui.chroma_loc_info_present_flag) {
			sps.vui.chroma_sample_loc_type_top_field = ue();
			sps.vui.chroma_sample_loc_type_bottom_field = ue();
		}

		sps.vui.timing_info_present_flag = u(1);
		if (sps.vui.timing_info_present_flag) {
			sps.vui.num_units_in_tick = u(32);
			sps.vui.time_scale = u(32);
			sps.vui.fixed_frame_rate_flag = u(1);
		}

		sps.vui.nal_hrd_parameters_present_flag = u(1);
		if (sps.vui.nal_hrd_parameters_present_flag) {
			printf("not implemented\n");
		}

		sps.vui.vcl_hrd_parameters_present_flag = u(1);
		if (sps.vui.vcl_hrd_parameters_present_flag) {
			printf("not implemented\n");
		}

		if (sps.vui.nal_hrd_parameters_present_flag || sps.vui.vcl_hrd_parameters_present_flag)
			sps.vui.low_delay_hrd_flag = u(1);

		sps.vui.pic_struct_present_flag = u(1);
		sps.vui.bitstream_restriction_flag = u(1);
		if (sps.vui.bitstream_restriction_flag) {
			sps.vui.motion_vectors_over_pic_boundaries_flag = u(1);
			sps.vui.max_bytes_per_pic_denom = ue();
			sps.vui.max_bits_per_mb_denom = ue();
			sps.vui.log2_max_mv_length_horizontal = ue();
			sps.vui.log2_max_mv_length_vertical = ue();
			sps.vui.max_num_reorder_frames = ue();
			sps.vui.max_dec_frame_buffering = ue();
		}
	}

	return 0;
}
