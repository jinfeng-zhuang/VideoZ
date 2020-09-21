#include "main.h"

struct pic_parameter_set_rbsp pps;

int pic_pararmeter_set_rbsp_parse(unsigned char* buffer, unsigned int length)
{
	int i;
	int iGroup;

	bitstream_init(buffer, length);

	memset(&pps, 0, sizeof(struct pic_parameter_set_rbsp));

	pps.pic_parameter_set_id = ue();
	pps.seq_parameter_set_id = ue();
	pps.entropy_coding_mode_flag = u(1);
	pps.bottom_field_pic_order_in_frame_present_flag = u(1);
	pps.num_slice_groups_minus1 = ue();
	if (pps.num_slice_groups_minus1) {
		pps.slice_group_map_type = ue();
		if (0 == pps.slice_group_map_type) {
			for (iGroup = 0; iGroup <= pps.num_slice_groups_minus1; iGroup++)
				pps.run_length_minus1[iGroup] = ue();
		}
		else if (2 == pps.slice_group_map_type) {
			for (iGroup = 0; iGroup <= pps.num_slice_groups_minus1; iGroup++) {
				pps.top_left[iGroup] = ue();
				pps.bottom_right[iGroup] = ue();
			}
		}
		else if ((3 == pps.slice_group_map_type) ||
			(4 == pps.slice_group_map_type) ||
			(5 == pps.slice_group_map_type)) {
			pps.slice_group_change_direction_flag = u(1);
			pps.slice_group_change_rate_minus1 = ue();
		}
		else if (6 == pps.slice_group_map_type) {
			pps.pic_size_in_map_units_minus1 = ue();
			for (i = 0; i < pps.pic_size_in_map_units_minus1; i++)
				pps.size_group_id[i] = ue();
		}
	}

	pps.num_ref_idx_10_default_active_minus1 = ue();
	pps.num_ref_idx_11_default_active_minus1 = ue();
	pps.weighted_pred_flag = u(1);
	pps.weighted_bipred_idc = u(2);
	pps.pic_init_qp_minus26 = se();
	pps.pic_init_qs_minus26 = se();
	pps.chroma_qp_index_offset = se();
	pps.deblocking_filter_control_present_flag = u(1);
	pps.constrained_intra_pred_flag = u(1);
	pps.redundant_pic_cnt_present_flag = u(1);
	
	if (more_rbsp_data()) {
		pps.transform_8x8_mode_flag = u(1);
		pps.pic_scaling_matrix_present_flag = u(1);
		if (pps.pic_scaling_matrix_present_flag) {
			for (i = 0; i < 6 + ((sps.chroma_format_idc != 3) ? 2 : 6) & pps.transform_8x8_mode_flag; i++) {
				pps.pic_scaling_list_present_flag[i] = u(1);
				if (pps.pic_scaling_list_present_flag[i]) {
					if (i < 6)
						printf("not implemented\n");
					else
						printf("not implemented\n");
				}
			}
			pps.second_chroma_qp_index_offset = se();
		}
	}
	rbsp_trailing_bits();

	return 0;
}
