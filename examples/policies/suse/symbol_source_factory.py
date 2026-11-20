#
# Copyright (C) 2026  SUSE Software Solutions Germany GmbH
#
# This file is part of klp-ccp.
#
# klp-ccp is free software: you can redistribute it and/or modify it
# under the terms of version 2 of the GNU General Public License as
# published by the Free Software Foundation.
#
# klp-ccp is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
#

# Creates the SymbolSource the policy gets its symbol information from.
#
# $KCP_MOD_SYMVERS: the Module.symvers file, for the source reading
# Module.symvers and the patched object's ELF.

import os

from .elf_source import ElfSource


def make_symbol_source(patched_obj_filename, env=os.environ):
    mod_symvers_filename = env.get('KCP_MOD_SYMVERS')
    if not mod_symvers_filename:
        raise KeyError('$KCP_MOD_SYMVERS not set')

    return ElfSource(mod_symvers_filename, patched_obj_filename)
