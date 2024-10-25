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

import ccp

from .mod_symvers import ModuleSymvers
from .target_mod_elf import TargetModElf
from .ipa_clones import IpaClones

import os
import sys
import re
from dataclasses import dataclass
from elftools.elf.constants import SH_FLAGS as ELF_SH_FLAGS

@dataclass
class ExternalizableSymbol:
    sym_name: str
    providing_mod: str
    is_exported: bool

class KlpPolicy(ccp.LpCreationPolicyAbc):
    def __init__(self):
        self._cfg_mod_symvers_filename = os.getenv('KCP_MOD_SYMVERS')
        if not self._cfg_mod_symvers_filename:
            raise KeyError('$KCP_MOD_SYMVERS not set')

        self._cfg_patched_obj_filename = os.getenv('KCP_PATCHED_OBJ')
        if not self._cfg_patched_obj_filename:
            raise KeyError('$KCP_PATCHED_OBJ not set')

        self._cfg_patched_src_ipa_clones_filename = \
            os.getenv('KCP_IPA_CLONES_DUMP')
        if not self._cfg_patched_src_ipa_clones_filename:
            raise KeyError('$KCP_IPA_CLONES_DUMP not set')

        self._cfg_kbuild_sdir = os.getenv('KCP_KBUILD_SDIR')
        if not self._cfg_kbuild_sdir:
            raise KeyError('$KCP_KBUILD_SDIR not set')

        self._cfg_kbuild_odir = os.getenv('KCP_KBUILD_ODIR')
        if not self._cfg_kbuild_odir:
            raise KeyError('$KCP_KBUILD_ODIR not set')

        self._cfg_sym_rename_prefix = os.getenv('KCP_RENAME_PREFIX')
        if not self._cfg_sym_rename_prefix:
            raise KeyError('$KCP_RENAME_PREFIX not set')

        use_klp_convert_exts = os.getenv('KCP_KLP_CONVERT_EXTS')
        if use_klp_convert_exts:
            try:
                self._cfg_use_klp_convert_exts = bool(int(use_klp_convert_exts))
            except ValueError:
                raise KeyError('invalid value for $KCP_KLP_CONVERT_EXTS')
        else:
            self._cfg_use_klp_convert_exts = False

        ext_blacklist = os.getenv('KCP_EXT_BLACKLIST')
        if ext_blacklist:
            self._cfg_ext_blacklist = set(map(lambda e: e.strip(),
                                              ext_blacklist.split(',')))
        else:
            self._cfg_ext_blacklist = set()

        patched_funcs = os.getenv('KCP_FUNC')
        if patched_funcs:
            self._cfg_patched_funcs = set(map(lambda e: e.strip(),
                                              patched_funcs.split(',')))
        else:
            self._cfg_patched_funcs = []

        self._cfg_work_dir = os.getenv('KCP_WORK_DIR')
        if not self._cfg_work_dir:
            raise KeyError('$KCP_WORK_DIR not set')


        self._mod_symvers = ModuleSymvers(self._cfg_mod_symvers_filename)
        self._patched_obj_elf = TargetModElf(self._cfg_patched_obj_filename)
        self._patched_src_ipa_clones = \
            IpaClones(self._cfg_patched_src_ipa_clones_filename)


        no_complete_patched_funcs = os.getenv('KCP_NO_COMPLETE_PATCHED_FUNCS')
        if no_complete_patched_funcs:
            try:
                no_complete_patched_funcs = bool(int(no_complete_patched_funcs))
            except ValueError:
                raise KeyError(
                    'invalid value for $KCP_NO_COMPLETE_PATCHED_FUNCS'
                )
        self._patched_funcs = set(self._cfg_patched_funcs)
        if not no_complete_patched_funcs:
            for f in self._cfg_patched_funcs:
                spawns = self._patched_src_ipa_clones.find_spawns_recursive(f)
                if not spawns:
                    continue
                for (f, optimized) in spawns.items():
                    if not optimized:
                        self._patched_funcs.add(f)
                    else:
                        print('warning: optimized function \"' +
                              f +'\" in callgraph', file=sys.stderr)


        patched_obj_ko = os.path.basename(self._cfg_patched_obj_filename)
        if patched_obj_ko.startswith('vmlinux'):
            self._patched_obj_name = 'vmlinux'
        else:
            patched_obj_ko = os.path.splitext(patched_obj_ko)
            if patched_obj_ko[1] != '.ko':
                raise ValueError('invalid module filename extension in \"' +
                                 self._cfg_patched_obj_filename + '\"')
            self._patched_obj_name = \
                patched_obj_ko[0].translate({ord('-') : ord('_')})


        self._re_eligible_header_filename = []
        self._re_eligible_header_filename.append(
            re.compile(re.escape(os.path.abspath(self._cfg_kbuild_sdir)) +
                       '/include/')
        )
        self._re_eligible_header_filename.append(
            re.compile(re.escape(os.path.abspath(self._cfg_kbuild_sdir)) +
                       '/arch/[^/]+/include/')
        )
        self._re_eligible_header_filename.append(
            re.compile(re.escape(os.path.abspath(self._cfg_kbuild_odir)) +
                       '/include/generated/')
        )
        self._re_eligible_header_filename.append(
            re.compile(re.escape(os.path.abspath(self._cfg_kbuild_odir)) +
                       '/arch/[^/]+/include/generated/')
        )
        self._re_eligible_header_filename.append(
            re.compile('/usr/lib64/gcc/')
        )

        self._externalizable_funcs = {}
        self._externalizable_objs = {}
        self._externalized_funcs = []
        self._externalized_objs = []

        # Store away externalizations made for the post-processing to
        # consume.
        self._externalized_funcs_fd = open(self._cfg_work_dir + '/fun_exts',
                                           'w')
        self._externalized_objs_fd = open(self._cfg_work_dir + '/obj_exts',
                                          'w')


    def is_patched_fun(self, function_name):
        return function_name in self._patched_funcs

    def is_header_eligible(self, header, is_pre_include):
        header = os.path.abspath(header)
        is_eligible = any(r.match(header)
                          for r in self._re_eligible_header_filename)
        if is_eligible:
            return True
        else:
            return (False,
                    ccp.LpCreationPolicyWarning(
                        'header \"' + header + '\" non-eligible for inclusion'
                    ))

    @classmethod
    def _elf_sym_is_undef(elf_sym):
        return elf_sym['st_shndx'] == 'SHN_UNDEF'

    def _elf_sym_is_fun_def_sym(self, elf_sym):
        t = elf_sym['st_info']['type']
        if t == 'STT_FUNC':
            assert(elf_sym['st_shndx'] != 'SHN_UNDEF')
            return True
        elif t != 'STT_NOTYPE':
            return False

        # So the symbol is STT_NOTYPE, it could be anything, e.g. a
        # function definition implemented in assembly. Check if it's
        # provided from an SHF_EXECINSTR section.
        shndx = elf_sym['st_shndx']
        if shndx == 'SHN_UNDEF' or shndx == 'SHN_ABS':
            return False

        try:
            shndx = int(shndx)
        except:
            return (False,
                    ccp.LpCreationPolicyWarning(
                        'ELF sym \"' + elf_sym.name + '\" shndx not an int'
                    ))
        if shndx >= self._patched_objs_elf.elf.num_sections():
            return (False,
                    ccp.LpCreationPolicyWarning(
                        'ELF sym \"' + elf_sym.name + '\" shndx out of bounds'
                    ))
        elf_sec = self._patched_objs_elf.get_section(shndx)
        if elf_sec['sh_type'] != 'SHT_PROGBITS':
            return False
        elif not elf_sec['sh_flags'] & ELF_SH_FLAGS.SHF_EXECINSTR:
            return False

        return True

    def _elf_sym_is_obj_def_sym(self, elf_sym):
        t = elf_sym['st_info']['type']
        if t == 'STT_OBJECT':
            assert(elf_sym['st_shndx'] != 'SHN_UNDEF')
            return True
        elif t != 'STT_NOTYPE':
            return False

        # So the symbol is STT_NOTYPE, it could be anything, e.g. a
        # function definition implemented in assembly. Check if it's
        # provided from some section which is looking like it could
        # contain data.
        shndx = elf_sym['st_shndx']
        if shndx == 'SHN_UNDEF' or shndx == 'SHN_ABS':
            return False

        try:
            shndx = int(shndx)
        except:
            return (False,
                    ccp.LpCreationPolicyWarning(
                        'ELF sym \"' + elf_sym.name + '\" shndx not an int'
                    ))
        if shndx >= self._patched_objs_elf.elf.num_sections():
            return (False,
                    ccp.LpCreationPolicyWarning(
                        'ELF sym \"' + elf_sym.name + '\" shndx out of bounds'
                    ))
        elf_sec = self._patched_objs_elf.get_section(shndx)
        sh_type = elf_sec['sh_type']
        if sh_type != 'SHT_PROGBITS' and sh_type != 'SHT_NOBITS':
            return False
        elif elf_sec['sh_flags'] & ELF_SH_FLAGS.SHF_EXECINSTR:
            return False
        elif not elf_sec['sh_flags'] & ELF_SH_FLAGS.SHF_ALLOC:
            return False

        return True

    def is_function_externalizable(self,
                                   function_name,
                                   linkage,
                                   definition_loc):
        if function_name in self._externalized_funcs:
            return True

        match linkage:
            case ccp.Linkage.NONE:
                return False
            case ccp.Linkage.INTERNAL:
                assert(definition_loc)
            case ccp.Linkage.NESTED_FUN_AUTO:
                return False

        if function_name in self._cfg_ext_blacklist:
            return False

        exporter = None
        if function_name in self._mod_symvers.symvers:
            exporter = self._mod_symvers.symvers[function_name]

        # If linkage is internal, then a definition must be available
        # in the compilation unit.
        if (linkage == ccp.Linkage.INTERNAL and
            (self._patched_src_ipa_clones.all_instances_removed
             (function_name))):
            return False

        if not function_name in self._patched_obj_elf.elf_syms:
            if linkage == ccp.Linkage.EXTERNAL:
                return (False,
                        ccp.LpCreationPolicyWarning(
                            'no ELF symbol entry in \"' + self._patched_obj_name
                            + '\" for extern \"' + function_name + '\"'
                        ))
            return False

        warnings = []
        elf_syms = self._patched_obj_elf.elf_syms[function_name]
        any_is_fun_def = False
        any_is_non_fun_def = False
        any_is_undef = False
        bindings = set()
        sym_value = None
        conflicting_definitions = False
        for elf_sym in elf_syms:
            is_fun_def = self._elf_sym_is_fun_def_sym(elf_sym)
            if isinstance(is_fun_def, tuple):
                warnings += is_fun_def[1]
                is_fun_def = is_fun_def[0]
            if is_fun_def:
                any_is_fun_def = True
                bindings.add(elf_sym['st_info']['bind'])
                if sym_value is None:
                    sym_value = elf_sym['st_value']
                elif sym_value != elf_sym['st_value']:
                    conflicting_definitions = True
            else:
                any_is_non_fun_def = True
            any_is_undef |= elf_sym['st_shndx'] == 'SHN_UNDEF'

        if any_is_fun_def and any_is_non_fun_def:
            conflicting_definitions = True

        if conflicting_definitions:
            warnings.append(
                ccp.LpCreationPolicyWarning(
                    'conflicting definitions for symbol \"' +
                    function_name + '\" found in ELF'
                ))

        assert(not any_is_fun_def or bindings)
        if not conflicting_definitions and len(bindings) > 1:
            warnings.append(
                ccp.LpCreationPolicyWarning(
                    'conflicting bindings for symbol \"' +
                    function_name + '\" found in ELF \"' +
                    self._cfg_patched_obj_filename + '\"'
                ))
        if linkage == ccp.Linkage.INTERNAL:
            if not any_is_fun_def:
                return (False, warnings, )

            if not 'STB_LOCAL' in bindings:
                warnings.append(
                    ccp.LpCreationPolicyWarning(
                        'expected STB_LOCAL binding for symbol \"' +
                        function_name + '\" in ELF \"' +
                        self._cfg_patched_obj_filename + '\"'
                    ))

            if exporter and exporter == self._patched_obj_name:
                warnings.append(
                    ccp.LpCreationPolicyWarning(
                        'unexpected local linkage for \"' +
                        function_name + '\" exported by \"' +
                        self._cfg_patched_obj_filename + '\"'
                    ))
            else:
                # A symbol with the same name might have been exported
                # from somewhere else, but we do not want that!
                exporter = None

            self._externalizable_funcs[function_name] = \
                ExternalizableSymbol(
                    function_name,
                    self._patched_obj_name,
                    exporter is not None
                )

            return (True, warnings, )

        else:
            assert(linkage == ccp.Linkage.EXTERNAL)
            if not any_is_undef and not any_is_fun_def:
                # Yeah well, how then could the target module link to
                # the symbol then?
                warnings.append(
                    ccp.LpCreationPolicyWarning(
                        'no ELF symbol entry for \"' +
                        function_name + '\" in \"' +
                        self._cfg_patched_obj_filename + '\"' +
                        ' (hint: blacklist if subject to dead code elimination)'
                ))
                return (False, warnings, )

            if definition_loc and not any_is_fun_def:
                warnings.append(
                    ccp.LpCreationPolicyWarning(
                        'expected ELF symbol entry for extern \"' +
                        function_name + '\" with definition in \"' +
                        self._cfg_patched_obj_filename + '\"'
                    ))

            if any_is_fun_def:
                if not 'STB_GLOBAL' in bindings and not 'STB_WEAK' in bindings:
                    warnings.append(
                        ccp.LpCreationPolicyWarning(
                            'expected STB_GLOBAL or STB_LOCAL binding'
                            ' for symbol \"' + function_name + '\" in ELF \"' +
                            self._cfg_patched_obj_filename + '\"'
                        ))

                if exporter and exporter != self._patched_obj_name:
                    # A symbol with the same name has been exported
                    # from somewhere else, but we do not want that!
                    exporter = None

                self._externalizable_funcs[function_name] = \
                    ExternalizableSymbol(
                        function_name,
                        self._patched_obj_name,
                        exporter is not None
                    )

            else:
                assert(any_is_undef)
                if exporter is None:
                    warnings.append(
                        ccp.LpCreationPolicyWarning(
                            'no export for undefined symbol \"' + object_name +
                            '\" from \"' + self._cfg_patched_obj_filename +
                            '\" found'
                    ))
                    return (False, warnings, )

                if exporter != 'vmlinux' and \
                   not exporter in self._patched_obj_elf.modinfo_deps:
                    warnings.append(
                        ccp.LpCreationPolicyWarning(
                            'symbol \"' + function_name +
                            '\" exported from \"' + exporter +
                            '\", which is  not among the modinfo' +
                            ' dependencies of \"' +
                            self._cfg_patched_obj_filename + '\"'
                    ))
                    return (False, warnings, )

                self._externalizable_funcs[function_name] = \
                    ExternalizableSymbol(
                        function_name, exporter, True
                    )

            return (True, warnings, )

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
        if linkage == ccp.Linkage.NONE:
            return False

        if object_name in self._cfg_ext_blacklist:
            return False

        if object_name == '__this_module':
            self._externalizable_objs[object_name] = \
                        ExternalizableSymbol(
                            object_name, 'vmlinux', True
                        )
        assert(linkage == ccp.Linkage.INTERNAL or
               linkage == ccp.Linkage.EXTERNAL)

        exporter = None
        if object_name in self._mod_symvers.symvers:
            exporter = self._mod_symvers.symvers[object_name]

        if not object_name in self._patched_obj_elf.elf_syms:
            if linkage == ccp.Linkage.EXTERNAL:
                return (False,
                        ccp.LpCreationPolicyWarning(
                            'no ELF symbol entry in \"' + self._patched_obj_name
                            + '\" for extern \"' + object_name + '\"'
                        ))
            return False

        warnings = []
        elf_syms = self._patched_obj_elf.elf_syms[object_name]
        any_is_obj_def = False
        any_is_non_obj_def = False
        any_is_undef = False
        bindings = set()
        sym_value = None
        conflicting_definitions = False
        for elf_sym in elf_syms:
            is_obj_def = self._elf_sym_is_obj_def_sym(elf_sym)
            if isinstance(is_obj_def, tuple):
                warnings += is_obj_def[1]
                is_obj_def = is_obj_def[0]
            if is_obj_def:
                any_is_obj_def = True
                bindings.add(elf_sym['st_info']['bind'])
                if sym_value is None:
                    sym_value = elf_sym['st_value']
                elif sym_value != elf_sym['st_value']:
                    conflicting_definitions = True
            else:
                any_is_non_obj_def = True
            any_is_undef |= elf_sym['st_shndx'] == 'SHN_UNDEF'

        if any_is_obj_def and any_is_non_obj_def:
            conflicting_definitions = True

        if conflicting_definitions:
            warnings.append(
                ccp.LpCreationPolicyWarning(
                    'conflicting definitions for symbol \"' +
                    object_name + '\" found in ELF'
                ))

        assert(not any_is_obj_def or bindings)
        if not conflicting_definitions and len(bindings) > 1:
            warnings.append(
                ccp.LpCreationPolicyWarning(
                    'conflicting bindings for symbol \"' +
                    object_name + '\" found in ELF \"' +
                    self._cfg_patched_obj_filename + '\"'
                ))

        if linkage == ccp.Linkage.INTERNAL:
            if not any_is_obj_def:
                warnings.append(
                    ccp.LpCreationPolicyWarning(
                        'object symbol \"' + object_name +
                        '\" with internal linkage not found in ELF \"' +
                        self._cfg_patched_obj_filename + '\" ' +
                        ' (hint: blacklist if subject to dead code elimination)'
                    ))
                return (False, warnings, )

            if not 'STB_LOCAL' in bindings:
                warnings.append(
                    ccp.LpCreationPolicyWarning(
                        'expected STB_LOCAL binding for symbol \"' +
                        object_name + '\" in ELF \"' +
                        self._cfg_patched_obj_filename + '\"'
                    ))

            if exporter and exporter == self._patched_obj_name:
                warnings.append(
                    ccp.LpCreationPolicyWarning(
                        'unexpected local linkage for \"' +
                        object_name + '\" exported by \"' +
                        self._cfg_patched_obj_filename + '\"'
                    ))
            else:
                # A symbol with the same name might have been exported
                # from somewhere else, but we do not want that!
                exporter = None

            self._externalizable_objs[object_name] = \
                ExternalizableSymbol(
                    object_name,
                    self._patched_obj_name,
                    exporter is not None
                )

            return (True, warnings, )

        else:
            assert(linkage == ccp.Linkage.EXTERNAL)
            if not any_is_undef and not any_is_obj_def:
                # Yeah well, how then could the target module link to
                # the symbol then?
                warnings.append(
                    ccp.LpCreationPolicyWarning(
                        'no ELF symbol entry for \"' +
                        object_name + '\" in \"' +
                        self._cfg_patched_obj_filename + '\"' +
                        ' (hint: blacklist if subject to dead code elimination)'
                ))
                return (False, warnings, )

            if any_is_obj_def:
                if not 'STB_GLOBAL' in bindings and not 'STB_WEAK' in bindings:
                    warnings.append(
                        ccp.LpCreationPolicyWarning(
                            'expected STB_GLOBAL or STB_LOCAL binding'
                            ' for symbol \"' + object_name + '\" in ELF \"' +
                            self._cfg_patched_obj_filename + '\"'
                    ))

                if exporter and exporter != self._patched_obj_name:
                    # A symbol with the same name has been exported
                    # from somewhere else, but we do not want that!
                    exporter = None

                self._externalizable_objs[object_name] = \
                    ExternalizableSymbol(
                        object_name,
                        self._patched_obj_name,
                        exporter is not None
                    )

            else:
                assert(any_is_undef)
                if exporter is None:
                    warnings.append(
                        ccp.LpCreationPolicyWarning(
                            'no export for undefined symbol \"' + object_name +
                            '\" from \"' + self._cfg_patched_obj_filename +
                            '\" found'
                    ))
                    return (False, warnings, )

                if exporter != 'vmlinux' and \
                   not exporter in self._patched_obj_elf.modinfo_deps:
                    warnings.append(
                        ccp.LpCreationPolicyWarning(
                            'symbol \"' + object_name +
                            '\" exported from \"' + exporter +
                            '\", which is  not among the modinfo' +
                            ' dependencies of \"' +
                            self._cfg_patched_obj_filename + '\"'
                    ))
                    return (False, warnings, )

                self._externalizable_objs[object_name] = \
                    ExternalizableSymbol(
                        object_name, exporter, True
                    )

            return (True, warnings, )

    def get_sym_mod_for_externalized_fun(self,
                                         function_name,
                                         allocated_ids):
        return self._get_sym_mod_for_externalized(
            self._externalizable_funcs[function_name],
            allocated_ids,
            self._externalized_funcs_fd
        )

    def get_sym_mod_for_patched_fun(self,
                                    function_name,
                                    allocated_ids):
        return ccp.SymbolModification(
            self._gen_unique_rename(
                self._cfg_sym_rename_prefix + 'p_' + function_name,
                allocated_ids
            ),
            ccp.SymbolModification.LinkageChange.MAKE_STATIC
        )


    def rename_rewritten_closure_fun(self,
                                     function_name,
                                     allocated_ids):
        return self._gen_unique_rename(
            self._cfg_sym_rename_prefix + 'r_' + function_name,
            allocated_ids
        )

    def get_sym_mod_for_externalized_obj(self,
                                         object_name,
                                         allocated_ids):
        return self._get_sym_mod_for_externalized(
            self._externalizable_objs[object_name],
            allocated_ids,
            self._externalized_objs_fd
        )

    def _get_sym_mod_for_externalized(self,
                                      ext_sym,
                                      allocated_ids,
                                      externalized_syms_fd):
        use_export = False
        if ext_sym.is_exported and ext_sym.providing_mod == 'vmlinux':
            use_export = True

        if use_export:
            method = 'EXPORT'
            sym_ext_mod = ccp.ExternalizedSymbolModification(
                ccp.SymbolModification(
                    None,
                    ccp.SymbolModification.LinkageChange.MAKE_EXTERN
                ),
                False
            )
        elif self._cfg_use_klp_convert_exts:
            method = 'KLP_CONVERT'
            sym_ext_mod = ccp.ExternalizedSymbolModification(
                ccp.SymbolModification(
                    None,
                    ccp.SymbolModification.LinkageChange.MAKE_EXTERN
                ),
                False
            )
        else:
            method = 'KALLSYMS'
            sym_ext_mod = ccp.ExternalizedSymbolModification(
                ccp.SymbolModification(
                    self._gen_unique_rename(
                        self._cfg_sym_rename_prefix + 'e_' + ext_sym.sym_name,
                        allocated_ids
                    ),
                    ccp.SymbolModification.LinkageChange.MAKE_STATIC
                ),
                True
            )

        externalized_syms_fd.write(
            '{} {} {} {}\n'.format(method,
                                   ext_sym.sym_name,
                                   ext_sym.sym_name,
                                   ext_sym.providing_mod)
        )

        return sym_ext_mod

    def _gen_unique_rename(self, base, allocated_ids):
        if not allocated_ids.contains(base):
            return base

        i = 0
        while True:
            if not allocated_ids.contains(base + str(i)):
                return base + str(i)
            i += 1
