#ifndef SPS_H
#define SPS_H

#define Extended_SAR (255)

struct vui_parameters {
	unsigned int aspect_ratio_info_present_flag;
	unsigned int aspect_ratio_idc;
	unsigned int sar_width;
	unsigned int sar_height;
	unsigned int overscan_info_present_flag;
	unsigned int overscan_appropriate_flag;
	unsigned int video_signal_type_present_flag;
	unsigned int video_format;
	unsigned int video_full_range_flag;
	unsigned int colour_description_present_flag;
	unsigned int colour_primaries;
	unsigned int transfer_characteristics;
	unsigned int matrix_coefficients;
	unsigned int chroma_loc_info_present_flag;
	unsigned int chroma_sample_loc_type_top_field;
	unsigned int chroma_sample_loc_type_bottom_field;
	unsigned int timing_info_present_flag;
	unsigned int num_units_in_tick;
	unsigned int time_scale;
	unsigned int fixed_frame_rate_flag;
	unsigned int nal_hrd_parameters_present_flag;
	// hrd_parameters
	unsigned int vcl_hrd_parameters_present_flag;
	// hrd_parameters
	unsigned int low_delay_hrd_flag;
	unsigned int pic_struct_present_flag;
	unsigned int bitstream_restriction_flag;
	unsigned int motion_vectors_over_pic_boundaries_flag;
	unsigned int max_bytes_per_pic_denom;
	unsigned int max_bits_per_mb_denom;
	unsigned int log2_max_mv_length_horizontal;
	unsigned int log2_max_mv_length_vertical;
	unsigned int max_num_reorder_frames;
	unsigned int max_dec_frame_buffering;
};

struct sps {
	unsigned int profile_idc;
	unsigned int constraint_set0_flag;
	unsigned int constraint_set1_flag;
	unsigned int constraint_set2_flag;
	unsigned int constraint_set3_flag;
	unsigned int constraint_set4_flag;
	unsigned int constraint_set5_flag;
	unsigned int reserved_zero_2bits;
	unsigned int level_idc;
	unsigned int seq_parameter_set_id;
	unsigned int chroma_format_idc;
	unsigned int separate_colour_plane_flag;
	unsigned int bit_depth_luma_minus8;
	unsigned int bit_depth_chroma_minus8;
	unsigned int qpprime_y_zero_transform_bypass_flag;
	unsigned int seq_scaling_matrix_present_flag;
	unsigned int seq_scaling_list_present_flag[12];
	// scaling_list?
	unsigned int log2_max_frame_num_minus4;
	unsigned int pic_order_cnt_type;
	unsigned int log2_max_pic_order_cnt_lsb_minus4;
	unsigned int delta_pic_order_always_zero_flag;
	unsigned int offset_for_non_ref_pic;
	unsigned int offset_for_top_to_bottom_field;
	unsigned int num_ref_frames_in_pic_order_cnt_cycle;
	unsigned int offset_for_ref_frame; // ?
	unsigned int max_num_ref_frames;
	unsigned int gaps_in_frame_num_value_allowed_flag;
	unsigned int pic_width_in_mbs_minus1;
	unsigned int pic_height_in_map_units_minus1;
	unsigned int frame_mbs_only_flag;
	unsigned int mb_adaptive_frame_field_flag;
	unsigned int direct_8x8_inference_flag;
	unsigned int frame_cropping_flag;
	unsigned int frame_crop_left_offset;
	unsigned int frame_crop_right_offset;
	unsigned int frame_crop_top_offset;
	unsigned int frame_crop_bottom_offset;
	unsigned int vui_parameters_present_flag;
	struct vui_parameters vui;
};

extern struct sps sps;

#endif