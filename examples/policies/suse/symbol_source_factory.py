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
# Configured by the environment:
#
# $KCP_SYMBOL_SOURCE selects the source:
# - "auto" (the default): use klp_data if $KCP_CODESTREAM is set and
#   klp_data is usable, otherwise fall back to Module.symvers and the
#   patched object's ELF (with a warning if klp_data was asked for by
#   setting $KCP_CODESTREAM).
# - "klp_data": use klp_data, fail if that's not possible.
# - "elf": use Module.symvers and the patched object's ELF.
#
# $KCP_CODESTREAM, $KCP_ARCH: the codestream (e.g. "15.5u10") and the
# architecture (e.g. "x86_64") to look up in klp_data. The database is
# located by $KLP_DATA_DIR.
#
# $KCP_MOD_SYMVERS: the Module.symvers file, for the "elf" source.
#
# The source which got chosen is logged to stderr.

import os
import sys

from .db_source import DbSource
from .elf_source import ElfSource


def _describe(exc):
    # The message of a KeyError is repr()'d by str().
    if isinstance(exc, KeyError) and exc.args:
        return str(exc.args[0])
    return str(exc)


def _select_symbol_source(obj_name, patched_obj_filename, env):
    mode = env.get('KCP_SYMBOL_SOURCE') or 'auto'
    if mode not in ('auto', 'klp_data', 'elf'):
        raise KeyError('invalid value for $KCP_SYMBOL_SOURCE: \"' + mode +
                       '\"')

    codestream = env.get('KCP_CODESTREAM')
    arch = env.get('KCP_ARCH')

    if mode == 'klp_data' or (mode == 'auto' and codestream):
        try:
            if not codestream:
                raise KeyError('$KCP_CODESTREAM not set')
            if not arch:
                raise KeyError('$KCP_ARCH not set')
            return DbSource(codestream, arch, obj_name)
        except (ImportError, FileNotFoundError, KeyError, ValueError,
                RuntimeError) as e:
            # Note klp_data.KlpDataError is a KeyError, and symbols.db
            # being unusable is reported as ValueError or RuntimeError.
            if mode == 'klp_data':
                raise
            print('warning: klp_data not usable (' + _describe(e) +
                  '), falling back to Module.symvers and ELF',
                  file=sys.stderr)

    mod_symvers_filename = env.get('KCP_MOD_SYMVERS')
    if not mod_symvers_filename:
        raise KeyError('$KCP_MOD_SYMVERS not set')

    return ElfSource(mod_symvers_filename, patched_obj_filename)


def make_symbol_source(obj_name, patched_obj_filename, env=os.environ):
    source = _select_symbol_source(obj_name, patched_obj_filename, env)
    print('info: getting symbol information from ' + source.label,
          file=sys.stderr)
    return source
