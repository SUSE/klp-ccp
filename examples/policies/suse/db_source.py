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

# SymbolSource for the case that the information about the symbols
# comes from the klp_data Python bindings.
#
# The database records the ELF attributes of the symbols the policy
# looks at, so the same decisions can be made as with the ELF file of
# the patched object. Every question goes to the database, it is fast
# enough for nothing to be kept around.

from .symbol_entry import (SectionFlags, SectionType, SymbolBinding,
                           SymbolEntry, SymbolType)
from .symbol_source import SymbolSource


class DbSource(SymbolSource):
    def __init__(self, codestream, arch, obj_name):
        """Open the database and check that it knows the patched object.

        obj_name is "vmlinux" or the normalized module name.

        Raises ImportError if klp_data is not available,
        FileNotFoundError if there is no symbols.db, ValueError or
        RuntimeError if symbols.db is unusable (e.g. created by an
        older klp-data, before the ELF attributes were recorded) and
        klp_data.KlpDataError (a KeyError) if the codestream/arch or
        the module is not in the database.
        """
        import klp_data

        self._codestream = codestream
        self._arch = arch
        self._obj_name = obj_name
        self._label = 'klp_data ' + codestream + '/' + arch
        self._kd = klp_data.KlpData()

        # Fail now, rather than at the first query, if the patched
        # object is not in the database.
        if obj_name == 'vmlinux':
            self._kd.get_sym(codestream, arch, '_printk')
        else:
            self._kd.get_module(codestream, arch, obj_name)

    @property
    def label(self):
        return self._label

    def exporter(self, name):
        occs = self._kd.get_sym(self._codestream, self._arch, name,
                                module='all')
        exporters = {o.module for o in occs if o.defined and o.is_exported}
        if len(exporters) > 1:
            raise ValueError('multiple exports of \"' + name +
                             '\" found in klp_data: ' +
                             ', '.join(sorted(exporters)))
        return next(iter(exporters), None)

    def modinfo_deps(self):
        if self._obj_name == 'vmlinux':
            return []
        return list(self._kd.get_module(self._codestream, self._arch,
                                        self._obj_name).depends)

    def entries(self, name):
        occs = self._kd.get_sym(self._codestream, self._arch, name,
                                module=self._obj_name)
        return [self._entry(o) for o in occs]

    @staticmethod
    def _entry(occ):
        # The attributes are reported by their names (without the ELF
        # prefixes), which the enums parse.
        section_type = occ.section_type
        if section_type is not None:
            section_type = SectionType(section_type)
        section_flags = occ.section_flags
        if section_flags is not None:
            section_flags = SectionFlags(section_flags)
        return SymbolEntry(occ.name, occ.defined, SymbolType(occ.type),
                           SymbolBinding(occ.bind), occ.value,
                           section_type, section_flags)
