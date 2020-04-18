/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

EFI_STATUS
MemoryInit (
  OUT  VOID   **HobList
  )
{
  EFI_STATUS  Status;

  Status = CallFspMemoryInit (PCD_GET32_WITH_ADJUST (PcdFSPMBase), HobList);
  FspResetHandler (Status);

  return Status;
}

EFI_STATUS
GetArchReservedMemory (
  IN   VOID     *HobList,
  OUT  UINTN    *MemBase,
  OUT  UINTN    *MemSize
  )
{
  UINT64  Base;
  UINT64  Size;

  Base = GetFspReservedMemoryFromGuid (HobList, &Size, &gFspReservedMemoryResourceHobGuid);
  if (MemBase != NULL) {
    *MemBase = Base;
  }
  if (MemSize != NULL) {
    *MemSize = Size;
  }
  return EFI_SUCCESS;
}
