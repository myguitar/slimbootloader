/** @file

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/GpioLib.h>
#include <Library/CryptoLib.h>
#include <Library/BoardInitLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/ArmLib.h>

#define MACH_VIRT_PERIPH_BASE       0x08000000
#define MACH_VIRT_PERIPH_SIZE       SIZE_128MB

CONST PLT_DEVICE  mPlatformDevices[]= {
  {{0x00000200}, OsBootDeviceSata  , 0 }
};

VOID
PlatformDeviceTableInit (
  VOID
  )
{
  PLT_DEVICE_TABLE      *PltDeviceTable;

  PltDeviceTable = (PLT_DEVICE_TABLE *)AllocatePool (sizeof (PLT_DEVICE_TABLE) + sizeof (mPlatformDevices));
  PltDeviceTable->DeviceNumber = sizeof (mPlatformDevices) /sizeof (PLT_DEVICE);
  CopyMem (PltDeviceTable->Device, mPlatformDevices, sizeof (mPlatformDevices));

  SetDeviceTable (PltDeviceTable);
}

VOID
VirtualMemoryMapInit (
  VOID
  )
{
  ARM_MEMORY_REGION_DESCRIPTOR  *Table;
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryMapTable;
  UINT32                         Index;
  UINT32                         Length;

  Index = 0;
  Table = AllocateTemporaryMemory (0);
  if (Table == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Error: Failed AllocatePool()\n", __FUNCTION__));
    return;
  }

  Table[Index].PhysicalBase = 0x00000000;
  Table[Index].VirtualBase  = Table[Index].PhysicalBase;
  Table[Index].Length       = PcdGet32 (PcdFlashSize);
  Table[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;
  Index++;

  Table[Index].PhysicalBase = PcdGet32 (PcdFlashSize);
  Table[Index].VirtualBase  = Table[Index].PhysicalBase;
  Table[Index].Length       = MACH_VIRT_PERIPH_BASE - PcdGet32 (PcdFlashSize);
  Table[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;
  Index++;

  Table[Index].PhysicalBase = MACH_VIRT_PERIPH_BASE;
  Table[Index].VirtualBase  = Table[Index].PhysicalBase;
  Table[Index].Length       = MACH_VIRT_PERIPH_SIZE;
  Table[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;
  Index++;

  Table[Index].PhysicalBase = MACH_VIRT_PERIPH_BASE + MACH_VIRT_PERIPH_SIZE;
  Table[Index].VirtualBase  = Table[Index].PhysicalBase;
  Table[Index].Length       = PcdGet64 (PcdPciExpressBaseAddress) - Table[Index].PhysicalBase;
  Table[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;
  Index++;

  Table[Index].PhysicalBase = PcdGet64 (PcdPciExpressBaseAddress);
  Table[Index].VirtualBase  = Table[Index].PhysicalBase;
  Table[Index].Length       = PcdGet64 (PcdSystemMemoryBase) - Table[Index].PhysicalBase;
  Table[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;
  Index++;

  Table[Index].PhysicalBase = PcdGet64 (PcdSystemMemoryBase);
  Table[Index].VirtualBase  = Table[Index].PhysicalBase;
  Table[Index].Length       = PcdGet64 (PcdSystemMemorySize);
  Table[Index].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;
  Index++;

  Table[Index].PhysicalBase = 0;
  Table[Index].VirtualBase  = 0;
  Table[Index].Length       = 0;
  Table[Index].Attributes   = 0;
  Index++;

  Length = sizeof (ARM_MEMORY_REGION_DESCRIPTOR) * Index;
  VirtualMemoryMapTable = AllocatePages (EFI_SIZE_TO_PAGES (Length));
  CopyMem (VirtualMemoryMapTable, Table, Length);

  (VOID)PcdSet64S (PcdVirtualMemoryMapTableBase, (UINT64)(UINTN)VirtualMemoryMapTable);
}

/**
  Board specific hook points.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
BoardInit (
  IN  BOARD_INIT_PHASE  InitPhase
)
{
  switch (InitPhase) {
  case PreConfigInit:
    PlatformDeviceTableInit ();
    break;
  case PostConfigInit:
    SetPlatformId (1);
    PlatformNameInit ();
    break;
  case PreMemoryInit:
    VirtualMemoryMapInit ();
    break;
  case PostMemoryInit:
  case PreTempRamExit:
  case PostTempRamExit:
    break;
  default:
    break;
  }
}

/**
  Load the configuration data blob from media into destination buffer.

  @param[in]    Dst        Destination address to load configuration data blob.
  @param[in]    Src        Source address to load configuration data blob.
  @param[in]    Len        The destination address buffer size.

  @retval  EFI_SUCCESS             Configuration data blob was loaded successfully.
  @retval  EFI_NOT_FOUND           Configuration data blob cannot be found.
  @retval  EFI_OUT_OF_RESOURCES    Destination buffer is too small to hold the
                                   configuration data blob.
  @retval  Others                  Failed to load configuration data blob.

**/
EFI_STATUS
EFIAPI
LoadExternalConfigData (
  IN UINT32  Dst,
  IN UINT32  Src,
  IN UINT32  Len
  )
{
  return EFI_SUCCESS;
}
