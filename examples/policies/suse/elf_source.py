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
# comes from Module.symvers and the ELF file of the patched object.

from .mod_symvers import ModuleSymvers
from .symbol_entry import (SectionFlags, SectionType, SymbolBinding,
                           SymbolEntry, SymbolType)
from .symbol_source import SymbolSource
from .target_mod_elf import TargetModElf


class ElfSource(SymbolSource):
    def __init__(self, mod_symvers_filename, patched_obj_filename):
        self._label = 'ELF \"' + patched_obj_filename + '\"'
        self._mod_symvers = ModuleSymvers(mod_symvers_filename)
        self._elf = TargetModElf(patched_obj_filename)

    @property
    def label(self):
        return self._label

    def exporter(self, name):
        return self._mod_symvers.symvers.get(name)

    def modinfo_deps(self):
        # No .modinfo, as in vmlinux, means no dependencies.
        return self._elf.modinfo_deps or []

    def entries(self, name):
        return [self._entry(elf_sym)
                for elf_sym in self._elf.elf_syms.get(name, ())]

    def _entry(self, elf_sym):
        section_type = None
        section_flags = None
        section_error = None

        shndx = elf_sym['st_shndx']
        if shndx != 'SHN_UNDEF' and shndx != 'SHN_ABS':
            try:
                shndx = int(shndx)
            except ValueError:
                section_error = ('ELF sym \"' + elf_sym.name +
                                 '\" shndx not an int')
            else:
                if shndx >= self._elf.elf.num_sections():
                    section_error = ('ELF sym \"' + elf_sym.name +
                                     '\" shndx out of bounds')
                else:
                    elf_sec = self._elf.elf.get_section(shndx)
                    section_type = SectionType(
                        str(elf_sec['sh_type']).removeprefix('SHT_')
                    )
                    section_flags = SectionFlags(elf_sec['sh_flags'])

        st_info = elf_sym['st_info']
        return SymbolEntry(
            elf_sym.name,
            elf_sym['st_shndx'] != 'SHN_UNDEF',
            SymbolType(str(st_info['type']).removeprefix('STT_')),
            SymbolBinding(str(st_info['bind']).removeprefix('STB_')),
            elf_sym['st_value'],
            section_type,
            section_flags,
            section_error
        )
