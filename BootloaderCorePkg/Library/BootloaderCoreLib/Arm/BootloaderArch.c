/** @file

  Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>

/**
  This function sets the Loader global data pointer.

  @param[in] LoaderData       Loader data pointer.

**/
VOID
EFIAPI
SetLoaderGlobalDataPointer (
  IN LOADER_GLOBAL_DATA   *LoaderData
  )
{
  ArmWriteTpidrurw ((UINTN)LoaderData);
}

/**
  This function gets the Loader global data pointer.

**/
LOADER_GLOBAL_DATA *
EFIAPI
GetLoaderGlobalDataPointer (
  VOID
  )
{
  return (LOADER_GLOBAL_DATA *)ArmReadTpidrurw ();
}
