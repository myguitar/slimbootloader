/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/CpuExceptionLib.h>
#include <Library/PagingLib.h>
#include <Library/PcdLib.h>
#include <Library/ArmMmuLib.h>

/**
  Load IDT table for current processor.

  It function initializes the exception handlers in IDT table and
  loads it into current processor.  It also try to store a specific DWORD
  data just before actual IDT base. It is used to set/get bootloader
  global data structure pointer.

  @param  IdtTable  Contain the IDT table pointer.
                    It must point to a STAGE_IDT_TABLE buffer preallocated.

  @param  Data      It contains a DWORD data that will be stored just before the actual IDT table.
                    It can be used to set/get global data using IDT pointer.

**/
VOID
LoadIdt (
  IN STAGE_IDT_TABLE   *IdtTable,
  IN UINTN              Data
  )
{
  IdtTable->LdrGlobal  = Data;
}

/**
  Copy GDT to memory and Load GDT table for current processor.

  It function initializes GDT table and loads it into current processor.

  @param[in]  GdtTable  Pointer to STAGE_GDT_TABLE structure to fill the GDT.
  @param[in]  GdtrPtr   Pointer to the source IA32_DESCRIPTOR structure.

**/
VOID
LoadGdt (
  IN STAGE_GDT_TABLE   *GdtTable,
  IN ARCH_DESCRIPTOR   *GdtrPtr
  )
{
}

/**
  Remap the current stage image.

  It will create page tables to remap current Stage1B from CAR to Memory.
  It is necessary since CAR will be destroyed during CallFspTempRamExit().
**/
/**
  Remap the current stage image.

  It will create page tables to remap current Stage1B from CAR to Memory.
  It is necessary since CAR will be destroyed during CallFspTempRamExit().
**/
VOID
RemapStage (
  VOID
  )
{
  ARM_MEMORY_REGION_DESCRIPTOR  *MemoryTable;
  VOID                          *TranslationTableBase;
  UINTN                         TranslationTableSize;
  RETURN_STATUS                 Status;

  MemoryTable = (ARM_MEMORY_REGION_DESCRIPTOR *)(UINTN)PcdGet32 (PcdVirtualMemoryTableBase);
  if (MemoryTable != NULL) {
    Status = ArmConfigureMmu (MemoryTable, &TranslationTableBase, &TranslationTableSize);
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "MMU enabled!\n"));
    } else {
      DEBUG ((DEBUG_ERROR, "Error: Failed to enable MMU\n"));
    }
  }
}

/**
  Unmap the previous mapped stage images.

  It will reload GDT and disable paging so that the memory goes back to 1:1 mapping.

**/
VOID
UnmapStage (
  VOID
  )
{
}
