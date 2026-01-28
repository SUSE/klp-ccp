/*
 * Copyright (C) 2026  SUSE Software Solutions Germany GmbH
 *
 * This file is part of klp-ccp.
 *
 * klp-ccp is free software: you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * klp-ccp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
 */

{
  .name = "march=",
  .code = opt_code_aarch64_march,
  .comp = gcc_cmdline_parser::option::comp_target,
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = "march="
},
{
  .name = "mbranch-protection=",
  .code = opt_code_aarch64_unused,
  .comp = gcc_cmdline_parser::option::comp_target,
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
},
{
  .name = "mcmodel=",
  .code = opt_code_aarch64_unused,
  .comp = gcc_cmdline_parser::option::comp_target,
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = "mcmodel="
},
{
  .name = "mgeneral-regs-only",
  .code = opt_code_aarch64_unused,
  .comp = gcc_cmdline_parser::option::comp_target,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = true,
  .alias = {},
  .negative = nullptr,
  .min_gcc_version = {7, 1, 0},
},
{
  .name = "mlittle-endian",
  .code = opt_code_aarch64_unused,
  .comp = gcc_cmdline_parser::option::comp_target,
  .arg = gcc_cmdline_parser::option::arg_none,
},
{
  .name = "mstrict-align",
  .code = opt_code_aarch64_unused,
  .comp = gcc_cmdline_parser::option::comp_target,
  .arg = gcc_cmdline_parser::option::arg_none,
  .reject_negative = false,
  .alias = {},
  .negative = nullptr,
},
{
  .name = "mtune=",
  .code = opt_code_aarch64_unused,
  .comp = gcc_cmdline_parser::option::comp_target,
  .arg = gcc_cmdline_parser::option::arg_joined,
  .reject_negative = true,
  .alias = {},
  .negative = "mtune="
},
