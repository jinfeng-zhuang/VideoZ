#ifndef PPS_H
#define PPS_H

struct pic_parameter_set_rbsp {
	unsigned int pic_parameter_set_id;
	unsigned int seq_parameter_set_id;
	unsigned int entropy_coding_mode_flag;
	unsigned int bottom_field_pic_order_in_frame_present_flag;
	unsigned int num_slice_groups_minus1;
	unsigned int slice_group_map_type;
	unsigned int run_length_minus1[1]; // ?
	unsigned int top_left[1]; // ?
	unsigned int bottom_right[1]; // ?
	unsigned int slice_group_change_direction_flag;
	unsigned int slice_group_change_rate_minus1;
	unsigned int pic_size_in_map_units_minus1;
	unsigned int size_group_id[1]; // ?
	unsigned int num_ref_idx_10_default_active_minus1;
	unsigned int num_ref_idx_11_default_active_minus1;
	unsigned int weighted_pred_flag;
	unsigned int weighted_bipred_idc;
	unsigned int pic_init_qp_minus26;
	unsigned int pic_init_qs_minus26;
	unsigned int chroma_qp_index_offset;
	unsigned int deblocking_filter_control_present_flag;
	unsigned int constrained_intra_pred_flag;
	unsigned int redundant_pic_cnt_present_flag;
	unsigned int transform_8x8_mode_flag;
	unsigned int pic_scaling_matrix_present_flag;
	unsigned int pic_scaling_list_present_flag[12];
	// scaling list
	unsigned int second_chroma_qp_index_offset;
};

extern struct pic_parameter_set_rbsp pps;

#endif