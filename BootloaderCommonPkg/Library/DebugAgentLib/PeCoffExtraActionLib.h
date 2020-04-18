/** @file
  PE/Coff Extra Action library instances, it will report image debug info.

  Copyright (c) 2013 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PE_COFF_EXTRA_ACTION_LIB_H_
#define _PE_COFF_EXTRA_ACTION_LIB_H_

#include <Base.h>
#include <Library/DebugAgentLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/PeImage.h>
#include <Library/LitePeCoffLib.h>

#include <ImageDebugSupport.h>

#define DEBUG_LOAD_IMAGE_METHOD_IO_HW_BREAKPOINT    1
#define DEBUG_LOAD_IMAGE_METHOD_SOFT_INT3           2

#define IO_HW_BREAKPOINT_VECTOR_NUM                 1
#define SOFT_INT_VECTOR_NUM                         3

#define PE_COFF_IMAGE_ALIGN_SIZE                    4

extern UINTN  AsmInterruptHandle;

/**
  Read IDT entry to check if IDT entries are setup by Debug Agent.

  @param[in]  IdtDescriptor      Pointer to IDT Descriptor.
  @param[in]  InterruptType      Interrupt type.

  @retval  TRUE     IDT entries were setup by Debug Agent.
  @retval  FALSE    IDT entries were not setuo by Debug Agent.

**/
BOOLEAN
CheckDebugAgentHandler (
  IN  VOID                       *IdtDescriptor,
  IN  UINTN                      InterruptType
  );

/**
  Save IDT entry for INT1 and update it.

  @param[in]  IdtDescriptor      Pointer to IDT Descriptor.
  @param[out] SavedIdtEntry      Original IDT entry returned.

**/
VOID
SaveAndUpdateIdtEntry1 (
  IN  VOID                       *IdtDescriptor,
  OUT VOID                       *SavedIdtEntry
  );

/**
  Restore IDT entry for INT1.

  @param[in]  IdtDescriptor      Pointer to IDT Descriptor.
  @param[in]  RestoredIdtEntry   IDT entry to be restored.

**/
VOID
RestoreIdtEntry1 (
  IN  VOID                       *IdtDescriptor,
  IN  VOID                       *RestoredIdtEntry
  );

#endif
