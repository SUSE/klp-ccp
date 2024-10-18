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

import os.path

class ModuleSymvers:
    def __init__(self, filename):
        f = open(filename)
        self.symvers = {}
        for line in f:
            line = line.strip()
            record = line.split('\t')
            if len(record) < 4 or len(record) > 5:
                raise ValueError('unrecognized line in Module.symvers: \"' +
                                 repr(line) + '\"')
            if record[1] in self.symvers:
                raise ValueError('multiple exports of \"' + record[1] +
                                 '\" found in Module.symvers')
            self.symvers[record[1]] = os.path.basename(record[2])
