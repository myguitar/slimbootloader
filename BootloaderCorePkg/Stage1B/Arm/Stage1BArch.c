/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/ArmLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/ArmMmuLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BoardInitLib.h>

#define RESERVED_MEMORY_SIZE                        SIZE_128MB

EFI_STATUS
EFIAPI
MemoryInit (
  OUT  VOID    **HobList
  )
{
  LOADER_GLOBAL_DATA                   *LdrGlobal;
  VOID                                 *OldLdrHobList;
  UINTN                                 SystemMemoryBase;
  UINT64                                SystemMemoryTop;
  UINTN                                 ReservedMemBase;
  UINTN                                 ReservedMemSize;
  EFI_RESOURCE_ATTRIBUTE_TYPE           ResourceAttributes;

  LdrGlobal = (LOADER_GLOBAL_DATA *)GetLoaderGlobalDataPointer ();
  ASSERT (LdrGlobal != NULL);

  OldLdrHobList = LdrGlobal->LdrHobList;

  SystemMemoryBase  = (UINTN)PcdGet64 (PcdSystemMemoryBase);
  SystemMemoryTop   = SystemMemoryBase + PcdGet64 (PcdSystemMemorySize);
  ReservedMemSize   = RESERVED_MEMORY_SIZE;
  ASSERT (ReservedMemSize < SIZE_256MB);
  ReservedMemBase = SystemMemoryBase + SIZE_256MB - ReservedMemSize;
  DEBUG ((DEBUG_INFO, "SystemMemoryBase:0x%08X SystemMemorySize:0x%08X\n",
                       (UINT32)SystemMemoryBase, (UINT32)PcdGet64 (PcdSystemMemorySize)));

  BuildHobHandoffInfoTable (0, ReservedMemBase, ReservedMemSize);

  BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_RESERVED,
      EFI_RESOURCE_ATTRIBUTE_PRESENT     |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_TESTED,
      0x00000000,
      SystemMemoryBase
      );

  ResourceAttributes = (
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_TESTED
  );

  BuildResourceDescriptorHob (
      EFI_RESOURCE_SYSTEM_MEMORY,
      ResourceAttributes,
      SystemMemoryBase,
      ReservedMemBase - SystemMemoryBase
      );

  //
  // Reserved for FSP Hob mimic
  //
  BuildResourceDescriptorHob (
      EFI_RESOURCE_SYSTEM_MEMORY,
      ResourceAttributes,
      ReservedMemBase,
      ReservedMemSize
      );

  //
  // Rest of Memory
  //
  BuildResourceDescriptorHob (
      EFI_RESOURCE_SYSTEM_MEMORY,
      ResourceAttributes,
      ReservedMemBase + ReservedMemSize,
      SystemMemoryTop - (SystemMemoryBase + ReservedMemSize)
      );

  InvalidateDataCacheRange ((VOID*)(UINTN)ReservedMemBase, ReservedMemSize);

  *HobList = LdrGlobal->LdrHobList;

  LdrGlobal->LdrHobList = OldLdrHobList;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetArchReservedMemory (
  IN   VOID     *HobList,
  OUT  UINTN    *MemBase,
  OUT  UINTN    *MemSize
  )
{
  UINTN Base;
  UINTN Size;

  Size = RESERVED_MEMORY_SIZE;
  Base = (UINTN)PcdGet64 (PcdSystemMemoryBase) + SIZE_256MB - Size;

  if (MemBase) {
    *MemBase = Base;
  }
  if (MemSize) {
    *MemSize = Size;
  }

  return EFI_SUCCESS;
}
