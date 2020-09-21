#include "main.h"

nalu_callback g_nalu_callback[NALU_TYPE_MAX] =
{
	NULL, // 0
	slice_layer_without_partitioning_rbsp_parse,
	NULL,
	NULL,
	NULL,
	NULL, // 5
	NULL,
	sps_proc,
	pic_pararmeter_set_rbsp_parse,
	NULL,
	NULL, // 10
	NULL,
	NULL,
	NULL,
	NULL,
	NULL, // 15
	NULL,
	NULL,
	NULL,
	NULL,
	NULL, // 20
	NULL,
	NULL,
	NULL,
	NULL,
	NULL, // 25
	NULL,
	NULL,
	NULL,
	NULL,
	NULL, // 30
	NULL
};
