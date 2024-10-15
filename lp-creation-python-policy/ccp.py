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

import abc
import enum

import sys;

import _ccp
from _ccp import AllocatedIds

class Linkage(enum.IntEnum):
    NONE = 0
    INTERNAL = 1
    EXTERNAL = 2
    NESTED_FUN_AUTO = 3

class SymbolModification:
    class LinkageChange(enum.IntEnum):
        NONE = 0
        MAKE_STATIC = 1
        MAKE_EXTERN = 2

    def __init__(self, new_name, linkage_change):
        self._new_name = new_name
        self._linkage_change = linkage_change


class ExternalizedSymbolModification:
    def __init__(self, symbol_mod, make_pointer):
        self._symbol_mod = symbol_mod
        self._make_pointer = make_pointer


class LpCreationPolicyWarning:
    def __init__(self, msg):
        self._msg = msg


class LpCreationPolicyError(Exception):
    def __init__(self, msg, warnings):
        super().__init__(msg)
        self._msg = msg
        self._warnings = warnings


class LpCreationPolicyAbc(abc.ABC):
    @abc.abstractmethod
    def is_patched_fun(self, function_name):
        pass

    @abc.abstractmethod
    def is_header_eligible(self, header, is_pre_include):
        pass

    @abc.abstractmethod
    def is_function_externalizable(self,
                                   function_name,
                                   linkage,
                                   definition_loc):
        pass

    @abc.abstractmethod
    def is_function_externalization_preferred(self,
                                              function_name,
                                              linkage,
                                              defined_in_eligible_header,
                                              definition_loc):
        pass

    @abc.abstractmethod
    def shall_externalize_object(self,
                                 object_name,
                                 linkage,
                                 declaration_loc):
        pass

    @abc.abstractmethod
    def get_sym_mod_for_externalized_fun(self,
                                         function_name,
                                         allocated_ids):
        pass

    @abc.abstractmethod
    def get_sym_mod_for_patched_fun(self,
                                    function_name,
                                    allocated_ids):
        pass

    @abc.abstractmethod
    def rename_rewritten_closure_fun(self,
                                     function_name,
                                     allocated_ids):
        pass

    @abc.abstractmethod
    def get_sym_mod_for_externalized_obj(self,
                                         object_name,
                                         allocated_ids):
        pass
