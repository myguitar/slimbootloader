/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _STAGE_1A_ARCH_H_
#define _STAGE_1A_ARCH_H_

#include <Stage1A.h>
#include <Library/ArmLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

VOID
EFIAPI
SecStartup (
  IN VOID  *Params
  );

VOID
EFIAPI
PeiCommonExceptionEntry (
  IN UINT32 Entry,
  IN UINTN  LR
  );

VOID
EFIAPI
EFIAPI PeiVectorTable (
  VOID
  );

#endif // _STAGE_1A_ARCH_H_
