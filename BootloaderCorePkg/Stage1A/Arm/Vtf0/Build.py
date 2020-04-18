## @file
#  Automate the process of building the various reset vector types
#
#  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

import glob
import os
import subprocess
import sys

def RunCommand(commandLine):
    return subprocess.call(commandLine)

for filename in glob.glob(os.path.join('Bin', '*.raw')):
    os.remove(filename)

arch = sys.argv[1]
debugType = None
output = os.path.join('Bin', 'ResetVector')
output += '.' + arch
if debugType is not None:
    output += '.' + debugType
output += '.raw'

bfv = 'BFV_ADDR=%s' % sys.argv[2]
xcompile = os.environ['GCC5_ARM_PREFIX']

commandLine = (
     '%sgcc' % xcompile,
     '-c',
     'ResetVector.S',
     '-D',
     bfv,
     '-o',
     'Bin/ResetVector.o'
     )
ret = RunCommand(commandLine)
if ret != 0: sys.exit(ret)

commandLine = (
     '%sobjcopy' % xcompile,
     '-O',
     'binary',
     'Bin/ResetVector.o',
     output
     )
ret = RunCommand(commandLine)
print ('\tASM\t' + output)
if ret != 0: sys.exit(ret)

for filename in glob.glob(os.path.join('Bin', '*.o')):
    os.remove(filename)
