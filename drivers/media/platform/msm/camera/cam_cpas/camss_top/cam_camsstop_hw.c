/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "cam_cpas_hw_intf.h"
#include "cam_cpas_hw.h"
#include "cam_cpas_soc.h"

int cam_camsstop_get_hw_info(struct cam_hw_info *cpas_hw,
	struct cam_cpas_hw_caps *hw_caps)
{
	struct cam_cpas *cpas_core = (struct cam_cpas *) cpas_hw->core_info;
	struct cam_hw_soc_info *soc_info = &cpas_hw->soc_info;
	int32_t reg_indx = cpas_core->regbase_index[CAM_CPAS_REG_CAMSS];
	uint32_t reg_value;

	if (reg_indx == -1)
		return -EINVAL;

	hw_caps->camera_family = CAM_FAMILY_CAMERA_SS;

	reg_value = cam_io_r_mb(soc_info->reg_map[reg_indx].mem_base + 0x0);
	hw_caps->camera_version.major =
		BITS_MASK_SHIFT(reg_value, 0xf0000000, 0x1c);
	hw_caps->camera_version.minor =
		BITS_MASK_SHIFT(reg_value, 0xfff0000, 0x10);
	hw_caps->camera_version.incr =
		BITS_MASK_SHIFT(reg_value, 0xffff, 0x0);

	CPAS_CDBG("Family %d, version %d.%d.%d\n",
		hw_caps->camera_family, hw_caps->camera_version.major,
		hw_caps->camera_version.minor, hw_caps->camera_version.incr);

	return 0;
}

int cam_camsstop_setup_regbase_indices(struct cam_hw_soc_info *soc_info,
	int32_t regbase_index[], int32_t num_reg_map)
{
	uint32_t index;
	int rc;

	if (num_reg_map > CAM_CPAS_REG_MAX) {
		pr_err("invalid num_reg_map=%d\n", num_reg_map);
		return -EINVAL;
	}

	if (soc_info->num_mem_block > CAM_SOC_MAX_BLOCK) {
		pr_err("invalid num_mem_block=%d\n", soc_info->num_mem_block);
		return -EINVAL;
	}

	rc = cam_cpas_util_get_string_index(soc_info->mem_block_name,
		soc_info->num_mem_block, "cam_camss", &index);
	if ((rc == 0) && (index < num_reg_map)) {
		regbase_index[CAM_CPAS_REG_CAMSS] = index;
	} else {
		pr_err("regbase not found for CAM_CPAS_REG_CAMSS\n");
		return -EINVAL;
	}

	return 0;
}

int cam_camsstop_get_internal_ops(struct cam_cpas_internal_ops *internal_ops)
{
	if (!internal_ops) {
		pr_err("invalid NULL param\n");
		return -EINVAL;
	}

	internal_ops->get_hw_info = cam_camsstop_get_hw_info;
	internal_ops->init_hw_version = NULL;
	internal_ops->handle_irq = NULL;
	internal_ops->setup_regbase = cam_camsstop_setup_regbase_indices;
	internal_ops->power_on_settings = NULL;

	return 0;
}