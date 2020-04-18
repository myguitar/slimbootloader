//
//  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
//
//  SPDX-License-Identifier: BSD-2-Clause-Patent
//

#include <AsmMacroIoLib.h>

.text

GCC_ASM_IMPORT(PreSecStartup)
GCC_ASM_IMPORT(PcdGet32(PcdStage1DataSize))

GCC_ASM_EXPORT(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):
  MOV32 (r8, FixedPcdGet64(PcdCPUCoresStackBase))
  MOV32 (r9, FixedPcdGet64(PcdCPUCoresStackBase) + FixedPcdGet32(PcdCPUCorePrimaryStackSize))
  mov   r0, r8
  mov   r1, r9

  //
  // Fill Stack Pattern
  //
  MOV32 (r9, 0x5AA55AA5)
  mov   r10, r9
  mov   r11, r9
  mov   r12, r9
0:stm   r8!, {r9-r12}
  cmp   r8, r1
  blt   0b

  //
  // Setup stack
  // r0: Bootloader stack base
  // r1: Bootloader stack top
  //
  ldr   r8, =ASM_PFX(PcdGet32(PcdStage1DataSize))
  add   r8, r0
  mov   sp, r8

  //
  // Param
  //
  MOV32 (r9, 0x0)         // TempValue
  mov   r2, r9            // TimeStamp
  mov   r3, r9            // TimeStamp
  mov   r4, r9            // Status
  mov   r5, r9            // Status

  // Push Param to stack
  push  {r5, r4, r3, r2, r1, r0}
  mov   r0, sp

  bl    ASM_PFX(PreSecStartup)

_NeverReturn:
        b       _NeverReturn
