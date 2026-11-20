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

# The interface through which the policy gets to know about the symbols
# of the object being patched ("patched object") and of the kernel the
# live patch will be applied to. Different sources, e.g. Module.symvers
# and the patched object's ELF, or the klp_data database, can provide
# this information.
#
# A source only provides the raw facts. It is up to the policy to
# interpret them.

import abc


class SymbolSource(abc.ABC):
    @property
    @abc.abstractmethod
    def label(self):
        """A short description of where the data comes from, for
        messages (e.g. 'ELF "my-mod.ko"')."""

    @abc.abstractmethod
    def exporter(self, name):
        """Return the name of the object exporting the symbol name
        ("vmlinux" or a normalized module name), or None if it isn't
        exported by anything."""

    @abc.abstractmethod
    def modinfo_deps(self):
        """Return the normalized names of the modules the patched object
        depends on according to its .modinfo."""

    @abc.abstractmethod
    def entries(self, name):
        """Return the SymbolEntry instances named name in the patched
        object, in symbol table order. Empty if there are none."""
