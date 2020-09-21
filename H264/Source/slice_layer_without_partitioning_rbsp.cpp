#include "main.h"

enum slice_type {
	SLICE_P,
	SLICE_B,
	SLICE_I,
	SLICE_SP,
	SLICE_SI
};

struct ref_pic_list_modification {
	unsigned int ref_pic_list_modification_flag_l0;
	unsigned int modification_of_pic_nums_idc;
	unsigned int abs_diff_pic_num_minus1;
	unsigned int long_term_pic_num;

	unsigned int ref_pic_list_modification_flag_l1;
};

struct pred_weight_table {
	unsigned int luma_log2_weight_denom;
	unsigned int chroma_log2_weight_denom;

	unsigned int luma_weight_l0_flag;
	unsigned int luma_weight_l0[1]; // ?
	unsigned int luma_offset_l0[1]; // ?
	unsigned int chroma_weight_l0_flag;
	unsigned int chroma_weight_l0[1][2];
	unsigned int chroma_offset_l0[1][2];

	unsigned int luma_weight_l1_flag;
	unsigned int luma_weight_l1[1]; // ?
	unsigned int luma_offset_l1[1]; // ?
	unsigned int chroma_weight_l1_flag;
	unsigned int chroma_weight_l1[1][2];
	unsigned int chroma_offset_l1[1][2];
};

struct dec_ref_pic_marking {
	unsigned int no_output_of_prior_pics_flag;
	unsigned int long_term_reference_flag;
	unsigned int adaptive_ref_pic_marking_mode_flag;
	unsigned int memory_management_control_operation;
	unsigned int difference_of_pic_nums_minus1;
	unsigned int long_term_pic_num;
	unsigned int long_term_frame_idx;
	unsigned int max_long_term_frame_idx_plus1;
};

struct slice_header {
	unsigned int first_mb_in_slice;
	unsigned int slice_type;
	unsigned int pic_parameter_set_id;
	unsigned int frame_num;
	unsigned int field_pic_flag;
	unsigned int bottom_field_flag;
	unsigned int idr_pic_id;
	unsigned int pic_order_cnt_lsb;
	unsigned int delta_pic_order_cnt_bottom;
	unsigned int delta_pic_order_cnt[2];
	unsigned int redundant_pic_cnt;
	unsigned int direct_spatial_mv_pred_flag;
	unsigned int num_ref_idx_active_override_flag;
	unsigned int num_ref_idx_10_active_minus1;
	unsigned int num_ref_idx_11_active_minus1;
	// ref_pic_list_reordering
	// pred_weight_table
    struct dec_ref_pic_marking dec_ref_pic_marking;
	unsigned int cabac_init_idc;
	unsigned int slice_qp_delta;
	unsigned int sp_for_switch_flag;
	unsigned int slice_qs_delta;
	unsigned int disable_deblocking_filter_idc;
	unsigned int slice_alpha_c0_offset_div2;
	unsigned int slice_beta_offset_div2;
	unsigned int slice_group_change_cycle;
};

struct mb_pred {
	unsigned int prev_intra4x4_pred_mode_flag[16];
	unsigned int rem_intra4x4_pred_mode[16];
	unsigned int prev_intra8x8_pred_mode_flag[4];
	unsigned int rem_intra8x8_pred_mode[4];
	unsigned int intra_chroma_pred_mode;
	unsigned int ref_idx_l0[1]; // ?
	unsigned int ref_idx_l1[1]; // ?
	unsigned int mvd_l0[1][1][2]; // ?
	unsigned int mvd_l1[1][1][2]; // ?
};

struct sub_mb_pred {
	unsigned int sub_mb_type[4];
	unsigned int ref_idx_l0[1]; // ?
	unsigned int ref_idx_l1[1]; // ?
	unsigned int mvd_l0[1][1][2]; // ?
	unsigned int mvd_l1[1][1][2]; // ?
};

struct residual {

};

struct residual_luma {

};

struct residual_block_cavlc {
	unsigned int coeff_token;
	unsigned int trailing_ones_sign_flag;
	unsigned int level_prefix;
	unsigned int level_suffix;
	unsigned int total_zeros;
	unsigned int run_before;
};

struct residual_block_cabac {
	unsigned int coded_block_flag;
	unsigned int significant_coeff_flag;
	unsigned int last_significant_coeff_flag;
	unsigned int coeff_abs_level_minus1[1]; // ?
	unsigned int coeff_sign_flag[1]; // ?
};

struct macroblock_layer {
	unsigned int mb_type;
	unsigned int pcm_alignment_zero_bit;
	unsigned int pcm_sample_luma[256];
	unsigned int pcm_sample_chroma[256];
	unsigned int transform_size_8x8_flag;
	unsigned int coded_block_pattern;
	unsigned int mb_qp_delta;
};

struct slice_data {
	unsigned int cabac_alignment_one_bit;
	unsigned int mb_skip_run;
	unsigned int mb_skip_flag;
	unsigned int mb_field_decoding_flag;
	unsigned int end_of_slice_flag;

    struct macroblock_layer mb;
};

struct slice_layer_without_partitioning_rbsp {
	struct slice_header header;
	struct slice_data data;
};

enum {
    I_NxN = 0,
    I_PCM = 25,
};

static struct slice_layer_without_partitioning_rbsp slice;

// just process I frame, to make it clear, then add more
int slice_layer_without_partitioning_rbsp_parse(unsigned char* buffer, unsigned int length)
{
	bitstream_init(buffer, length);

	memset(&slice, 0, sizeof(struct slice_layer_without_partitioning_rbsp));

	slice.header.first_mb_in_slice = ue();
	slice.header.slice_type = ue();

	slice.header.slice_type = slice.header.slice_type % 5;

	slice.header.pic_parameter_set_id = ue();

	slice.header.frame_num = u(sps.log2_max_frame_num_minus4 + 4);

    if (!sps.frame_mbs_only_flag)
        slice.header.field_pic_flag = u(1);

	if (0 == sps.pic_order_cnt_type) {
		slice.header.pic_order_cnt_lsb = u(sps.log2_max_pic_order_cnt_lsb_minus4 + 4);
		if (pps.bottom_field_pic_order_in_frame_present_flag && (!slice.header.field_pic_flag))
			slice.header.delta_pic_order_cnt_bottom = se();
	}

    slice.header.dec_ref_pic_marking.adaptive_ref_pic_marking_mode_flag = u(1);

    slice.header.slice_qp_delta = se();

    if (pps.deblocking_filter_control_present_flag) {
        slice.header.disable_deblocking_filter_idc = ue();

        if (1 != slice.header.disable_deblocking_filter_idc) {
            slice.header.slice_alpha_c0_offset_div2 = se();
            slice.header.slice_beta_offset_div2 = se();
        }
    }

    // Slice Data
    if (pps.entropy_coding_mode_flag) {
        while (!byte_aligned()) {
            slice.data.cabac_alignment_one_bit = u(1);
        }
    }

    int MbaffFrameFlag = 0; // TODO

    unsigned int CurrMbAddr = slice.header.first_mb_in_slice * (1 + MbaffFrameFlag);
    int moreDataFlag = 1;
    int prevMbSkipped = 0;

    do {
        if (moreDataFlag) {
            // macroblock_layer()
            slice.data.mb.mb_type = ue();
            if (I_PCM == slice.data.mb.mb_type) {

            }
            else {
                if (slice.data.mb.mb_type != I_NxN) {

                }
                else {
                    if (pps.transform_8x8_mode_flag && slice.data.mb.mb_type == I_NxN) {
                        slice.data.mb.transform_size_8x8_flag = u(1);
                    }

                    // mb_pred()
                }

                if (1) {
                    //slice.data.mb.coded_block_pattern = me(); // me ?
                }
            }
        }

        if (!pps.entropy_coding_mode_flag) {
            //
        }
        else {
            // 
            //slice.data.end_of_slice_flag = ae();
        }

        //CurrMbAddr = NextMbAddress(CurrMbAddr);
    } while (moreDataFlag);

	return 0;
}
