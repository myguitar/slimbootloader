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
#include <Library/FdtLib.h>

#define DEVICE_TREE_BASE_ADDR       0x40000000
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
  ARM_MEMORY_REGION_DESCRIPTOR  *VirtualMemoryTable;
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
  VirtualMemoryTable = AllocatePages (EFI_SIZE_TO_PAGES (Length));
  CopyMem (VirtualMemoryTable, Table, Length);

  (VOID)PcdSet32S (PcdVirtualMemoryTableBase, (UINT64)(UINTN)VirtualMemoryTable);
}

VOID
PlatformMemoryInit (
  VOID
  )
{
  UINT64        SystemMemoryBase;
  UINT64        SystemMemorySize;
  INT32         MemoryNode;
  INT32         AddressCells;
  INT32         SizeCells;
  INT32         Length;
  VOID          *DeviceTreeBlob;
  CONST INT32   *Prop;

  DeviceTreeBlob = (VOID *)(UINTN)DEVICE_TREE_BASE_ADDR;
  if (fdt_check_header (DeviceTreeBlob) != 0) {
    return;
  }

  //
  // Look for a node called "memory" at the lowest level of the tree
  //
  MemoryNode = fdt_path_offset (DeviceTreeBlob, "/memory");
  if (MemoryNode <= 0) {
    return;
  }

  //
  // Retrieve the #address-cells and #size-cells properties
  // from the root node, or use the default if not provided.
  //
  AddressCells = 1;
  SizeCells = 1;

  Prop = fdt_getprop (DeviceTreeBlob, 0, "#address-cells", &Length);
  if (Length == 4) {
    AddressCells = fdt32_to_cpu (*Prop);
  }

  Prop = fdt_getprop (DeviceTreeBlob, 0, "#size-cells", &Length);
  if (Length == 4) {
    SizeCells = fdt32_to_cpu (*Prop);
  }

  //
  // Now find the 'reg' property of the /memory node, and read the first
  // range listed.
  //
  Prop = fdt_getprop (DeviceTreeBlob, MemoryNode, "reg", &Length);

  if (Length < (AddressCells + SizeCells) * sizeof (INT32)) {
    return;
  }

  SystemMemoryBase = fdt32_to_cpu (Prop[0]);
  if (AddressCells > 1) {
    SystemMemoryBase = (SystemMemoryBase << 32) | fdt32_to_cpu (Prop[1]);
  }
  Prop += AddressCells;

  SystemMemorySize = fdt32_to_cpu (Prop[0]);
  if (SizeCells > 1) {
    SystemMemorySize = (SystemMemorySize << 32) | fdt32_to_cpu (Prop[1]);
  }

  (VOID)PcdSet64S (PcdSystemMemoryBase, SystemMemoryBase);
  (VOID)PcdSet64S (PcdSystemMemorySize, SystemMemorySize);
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
    // Use PreMemoryInit for now
    PlatformMemoryInit ();
    VirtualMemoryMapInit ();
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
