from elftools.elf.elffile import ELFFile

class TargetModElf:
    def __init__(self, filename):
        self._f = open(filename, 'rb')
        self.elf = ELFFile(self._f)

        self.modinfo_deps = None
        modinfo_secndx = self.elf.get_section_index('.modinfo')
        if not modinfo_secndx is None:
            modinfo_sec = self.elf.get_section(modinfo_secndx)
            for line in modinfo_sec.data().split(b'\x00'):
                line = line.decode('utf-8')
                if not line:
                    continue
                line = line.split('=')
                if line[0] != 'depends':
                    continue
                if len(line) != 2:
                    raise ValueError('malformed .modinfo \"depends\" entry')
                self.modinfo_deps = line[1].split(',')
                break
            if self.modinfo_deps is None:
                raise ValueError('no \"depends\" entry in .modinfo')

        self.elf_syms = {}
        for symtab_sec in self.elf.iter_sections('SHT_SYMTAB'):
            for sym in symtab_sec.iter_symbols():
                st_info = sym['st_info']
                if not sym.name or st_info['type'] == 'STT_SECTION':
                    continue
                if not sym.name in self.elf_syms:
                    self.elf_syms[sym.name] = [sym]
                else:
                    self.elf_syms[sym.name].append(sym)

