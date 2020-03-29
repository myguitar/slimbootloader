/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/SerialPortLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/ContainerLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/SerialPortInfoGuid.h>
#include <ConfigDataDefs.h>

UINT8
GetSerialPortStrideSize (
  VOID
);

UINT32
GetSerialPortBase (
  VOID
  );

VOID
EnableLegacyRegions (
  VOID
);

/**
  Board specific hook point.

  Implement board specific initialization during the boot flow.

  @param[in] InitPhase             Current phase in the boot flow.

**/
VOID
BoardInit (
  IN  BOARD_INIT_PHASE    InitPhase
)
{
  GEN_CFG_DATA        *GenericCfgData;

  switch (InitPhase) {
  case PostPciEnumeration:
    GenericCfgData = (GEN_CFG_DATA *)FindConfigDataByTag (CDATA_GEN_TAG);
    if (GenericCfgData != NULL) {
      if (GenericCfgData->PayloadId == AUTO_PAYLOAD_ID_SIGNATURE) {
          SetPayloadId (0);
      } else {
        SetPayloadId (GenericCfgData->PayloadId);
      }
    }
    break;
  default:
    break;
  }
}

/**
  Update serial port information to global HOB data structure.

  @param SerialPortInfo  Pointer to global HOB data structure.
 **/
VOID
EFIAPI
UpdateSerialPortInfo (
  IN  SERIAL_PORT_INFO  *SerialPortInfo
)
{
  SerialPortInfo->Type     = 1;
  SerialPortInfo->BaseAddr = GetSerialPortBase();
  SerialPortInfo->RegWidth = GetSerialPortStrideSize();
}

/**
 Update the OS boot option

 @param OsBootOptionList pointer to boot option list.
 **/
VOID
EFIAPI
UpdateOsBootMediumInfo (
  OUT  OS_BOOT_OPTION_LIST   *OsBootOptionList
)
{
  FillBootOptionListFromCfgData (OsBootOptionList);
}

/**
 Update Hob Info with platform specific data

 @param  Guid          The GUID to tag the customized HOB.
 @param  HobInfo       The start address of GUID HOB data.
 **/
VOID
EFIAPI
PlatformUpdateHobInfo (
  IN CONST EFI_GUID              *Guid,
  OUT      VOID                  *HobInfo
)
{
  ASSERT (Guid != NULL);
  ASSERT (HobInfo != NULL);

  if (Guid == &gLoaderSerialPortInfoGuid) {
    UpdateSerialPortInfo (HobInfo);
  } else if (Guid == &gOsBootOptionGuid) {
    UpdateOsBootMediumInfo (HobInfo);
  }
}

/**
  Update PCH NVS and SA NVS area address and size in ACPI table

  @param[in] Current       Pointer to ACPI description header

  @retval    EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
PlatformUpdateAcpiTable (
  IN UINT8                   *Current
)
{
  return EFI_SUCCESS;
}

/**
  Dynamic update of Global NVS data

  @param[in] GnvsIn       Pointer to global nvs region

**/
VOID
EFIAPI
PlatformUpdateAcpiGnvs (
  IN VOID  *GnvsIn
)
{
}
