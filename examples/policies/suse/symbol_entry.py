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

# One entry of a symbol table, in the vocabulary of the klp_data Python
# bindings, which is what the symbol sources translate their data to.

import enum
from dataclasses import dataclass
from typing import Optional


class _OtherIfUnknown(enum.Enum):
    # Base class for enums which have an OTHER member standing for
    # anything the policy doesn't need to tell apart.
    @classmethod
    def _missing_(cls, value):
        return cls.OTHER


class SymbolType(_OtherIfUnknown):
    # ELF symbol type (STT_*).
    FUNC = 'FUNC'
    OBJECT = 'OBJECT'
    NOTYPE = 'NOTYPE'
    OTHER = 'OTHER'


class SymbolBinding(_OtherIfUnknown):
    # ELF symbol binding (STB_*).
    LOCAL = 'LOCAL'
    GLOBAL = 'GLOBAL'
    WEAK = 'WEAK'
    OTHER = 'OTHER'


class SectionType(_OtherIfUnknown):
    # ELF section type (SHT_*), the ones which matter.
    PROGBITS = 'PROGBITS'
    NOBITS = 'NOBITS'
    OTHER = 'OTHER'


class SectionFlags(enum.IntFlag):
    # ELF section flags (SHF_*), the ones which matter. Bits not
    # listed are retained.
    WRITE = 0x1
    ALLOC = 0x2
    EXECINSTR = 0x4


@dataclass(frozen=True)
class SymbolEntry:
    name: str
    # False exactly for undefined references (SHN_UNDEF). Absolute
    # symbols (SHN_ABS) are defined, but have no section.
    defined: bool
    # Reported as found: an assembly function typed STT_NOTYPE stays
    # NOTYPE, what it really is, is told by its section.
    type: SymbolType
    bind: SymbolBinding
    # The raw st_value. An address in vmlinux, an offset within the
    # symbol's section in a module.
    value: int
    # Type and flags of the section the symbol is in. Both None if the
    # symbol is not relative to a section (undefined, absolute, common).
    section_type: Optional[SectionType] = None
    section_flags: Optional[SectionFlags] = None
    # Set instead of the section if it could not be determined, to a
    # message telling why (broken ELF file).
    section_error: Optional[str] = None
