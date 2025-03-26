#
# Copyright (C) 2024  SUSE Software Solutions Germany GmbH
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

from ccp import LpCreationPolicyAbc
from ccp import SymbolModification
from ccp import ExternalizedSymbolModification
import re

class LpCreationPolicy(LpCreationPolicyAbc):
    def __init__(self, patched_funcs):
        self._re_fun_name = re.compile('(p[use]_)?(e[use](p)?(r)?_)?.*')
        self._re_obj_name = re.compile('(e[use](p)?(r)?_)?.*')

    def is_patched_fun(self, function_name):
        m = self._re_fun_name.match(function_name)
        return m is not None and m.group(1) is not None

    def is_header_eligible(self, header, is_pre_include):
        return re.compile('.*internal').match(header) is None

    def is_function_externalizable(self,
                                   function_name,
                                   linkage,
                                   definition_loc):
        m = self._re_fun_name.match(function_name)
        espec = None
        if m is not None:
            espec = m.group(2)
        return espec is not None and espec is not None

    def is_function_externalization_preferred(self,
                                              function_name,
                                              linkage,
                                              defined_in_eligible_header,
                                              definition_loc):
        return not defined_in_eligible_header

    def shall_externalize_object(self,
                                 object_name,
                                 linkage,
                                 declaration_loc):
        m = self._re_obj_name.match(object_name)
        espec = None
        if m is not None:
            espec = m.group(1)
        return espec is not None and espec is not None

    def get_sym_mod_for_externalized_fun(self,
                                         function_name,
                                         allocated_ids):
        m = self._re_fun_name.match(function_name)
        assert(m is not None)
        espec = m.group(2)
        assert(len(espec) != 0)
        linkage_change = None
        match espec[1]:
            case 'u':
                linkage_change = SymbolModification.LinkageChange.NONE
            case 's':
                linkage_change = SymbolModification.LinkageChange.MAKE_STATIC
            case 'e':
                linkage_change = SymbolModification.LinkageChange.MAKE_EXTERN

        make_pointer = False
        if m.group(3) is not None:
            make_pointer = True

        new_name = function_name
        if m.group(4) is not None:
            new_name = 'klpe_' + function_name

        sym_mod = SymbolModification(new_name, linkage_change)
        return ExternalizedSymbolModification(sym_mod, make_pointer)

    def get_sym_mod_for_patched_fun(self,
                                    function_name,
                                    allocated_ids):
        m = self._re_fun_name.match(function_name)
        assert(m is not None)
        pspec = m.group(1)
        assert(len(pspec) != 0)
        linkage_change = None
        match pspec[1]:
            case 'u':
                linkage_change = SymbolModification.LinkageChange.NONE
            case 's':
                linkage_change = SymbolModification.LinkageChange.MAKE_STATIC
            case 'e':
                linkage_change = SymbolModification.LinkageChange.MAKE_EXTERN

        new_name = 'klpp_' + function_name

        return SymbolModification(new_name, linkage_change)

    def rename_rewritten_closure_fun(self,
                                     function_name,
                                     allocated_ids):
        return 'klpr_' + function_name

    def get_sym_mod_for_externalized_obj(self,
                                         object_name,
                                         allocated_ids):
        m = self._re_obj_name.match(object_name)
        assert(m is not None)
        espec = m.group(1)
        assert(len(espec) != 0)
        linkage_change = None
        match espec[1]:
            case 'u':
                linkage_change = SymbolModification.LinkageChange.NONE
            case 's':
                linkage_change = SymbolModification.LinkageChange.MAKE_STATIC
            case 'e':
                linkage_change = SymbolModification.LinkageChange.MAKE_EXTERN

        make_pointer = False
        if m.group(2) is not None:
            make_pointer = True

        new_name = object_name
        if m.group(3) is not None:
            new_name = 'klpe_' + object_name

        sym_mod = SymbolModification(new_name, linkage_change)
        return ExternalizedSymbolModification(sym_mod, make_pointer)
