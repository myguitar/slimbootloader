/** @file
  This file provides payload common library interfaces.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PayloadLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/LoaderPerformanceLib.h>
#include <Library/PayloadMemoryAllocationLib.h>
#include <Library/SerialPortLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/ContainerLib.h>
#include <Library/ConfigDataLib.h>
#include <Guid/BootLoaderServiceGuid.h>
#include <Guid/BootLoaderVersionGuid.h>
#include <Guid/LoaderPlatformDataGuid.h>
#include <Guid/LoaderPlatformInfoGuid.h>
#include <Library/GraphicsLib.h>

/**
  Initialize Payload Global Data area

  @retval     none

**/
STATIC
VOID
PayloadGlobalDataInit (
  IN  VOID
  )
{
  EFI_STATUS                 Status;
  PAYLOAD_GLOBAL_DATA       *GlobalDataPtr;
  LOADER_PLATFORM_DATA      *LoaderPlatformData;
  CDATA_BLOB                *CfgDataPtr;
  DEBUG_LOG_BUFFER_HEADER   *DebugLogBufferPtr;
  CONTAINER_LIST            *ContainerListPtr;
  LOADER_LIBRARY_DATA       *LoaderLibData;
  BOOT_LOADER_SERVICES_LIST *BldServicesList;
  VOID                      *PcdDataPtr;
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;

  GlobalDataPtr = (PAYLOAD_GLOBAL_DATA *)PcdGet32 (PcdGlobalDataAddress);

  //
  // Skip Global Data init just in case of Payload restart
  //
  if (GlobalDataPtr->Signature == PLD_GDATA_SIGNATURE) {
    return;
  }

  ZeroMem (GlobalDataPtr, sizeof (PAYLOAD_GLOBAL_DATA));
  GlobalDataPtr->Signature = PLD_GDATA_SIGNATURE;

  //
  // LOADER_PLATFORM_DATA
  //
  LoaderPlatformData = (LOADER_PLATFORM_DATA *) GetGuidHobData (NULL, NULL, &gLoaderPlatformDataGuid);
  if (LoaderPlatformData != NULL) {
    // Get config data pointer
    CfgDataPtr = (CDATA_BLOB *)LoaderPlatformData->ConfigDataPtr;
    if ((CfgDataPtr != NULL) && (CfgDataPtr->Signature == CFG_DATA_SIGNATURE)) {
      GlobalDataPtr->CfgDataPtr = (VOID *)CfgDataPtr;
    }

    // Get debug log buffer pointer
    DebugLogBufferPtr  = LoaderPlatformData->DebugLogBuffer;
    if ((DebugLogBufferPtr != NULL) && (DebugLogBufferPtr->Signature == DEBUG_LOG_BUFFER_SIGNATURE)) {
      GlobalDataPtr->LogBufPtr = (VOID *)DebugLogBufferPtr;
    }

    // Get container list pointer
    ContainerListPtr = (CONTAINER_LIST *)LoaderPlatformData->ContainerList;
    if ((ContainerListPtr != NULL) && (ContainerListPtr->Signature == CONTAINER_LIST_SIGNATURE)) {
      GlobalDataPtr->ContainerList = (VOID *)ContainerListPtr;
    }
  }

  //
  // LOADER_LIBRARY_DATA
  //
  LoaderLibData = (LOADER_LIBRARY_DATA *) GetGuidHobData (NULL, NULL, &gLoaderLibraryDataGuid);
  if (LoaderLibData != NULL) {
    GlobalDataPtr->LibDataPtr = (VOID *) LoaderLibData->Data;

    // Get PCD data base
    Status = GetLibraryData (PcdGet8 (PcdPcdLibId), (VOID **)&PcdDataPtr);
    if (!EFI_ERROR (Status)) {
      GlobalDataPtr->PcdDataPtr = PcdDataPtr;
    }
  }

  //
  // BOOT_LOADER_SERVICES_LIST
  //
  BldServicesList = (BOOT_LOADER_SERVICES_LIST *) GetGuidHobData (NULL, NULL, &gBootLoaderServiceGuid);
  if (BldServicesList != NULL) {
    GlobalDataPtr->ServiceList = (VOID *)&BldServicesList->ServiceList;
  }

  //
  // LOADER_PLATFORM_INFO
  //
  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *) GetGuidHobData (NULL, NULL, &gLoaderPlatformInfoGuid);
  if (LoaderPlatformInfo != NULL) {
    GlobalDataPtr->LdrFeatures = LoaderPlatformInfo->LdrFeatures;
  }

  //
  // PLT_DEVICE_TABLE
  //
  GlobalDataPtr->DeviceTable = GetGuidHobData (NULL, NULL, &gDeviceTableHobGuid);

  //
  // GetLoaderPerformanceInfo() function
  //
  GetLoaderPerformanceInfo (&GlobalDataPtr->PerfData);
}

/**
  Initialize critical payload global data.

  @param[in]      HobList         The HOB list pointer for payload.
  @param[in, out] PldHeapBase     Payload heap base pointer.
  @param[in, out] PldStatckBase   Payload stack base pointer.
  @param[in, out] PldStatckSize   Payload stack size pointer.

**/
STATIC
VOID
PayloadInit (
  IN     VOID                   *HobList,
  IN OUT UINT32                 *PldHeapBase,
  IN OUT UINT32                 *PldStatckBase,
  IN OUT UINT32                 *PldStatckSize
  )
{
  UINT32                    HeapBase;
  UINT32                    HeapSize;
  UINT64                    RsvdBase;
  UINT64                    RsvdSize;
  UINT32                    StackBase;
  UINT32                    StackSize;
  EFI_STATUS                PcdStatus1;
  EFI_STATUS                PcdStatus2;
  UINT32                    GlobalDataBase;
  UINT32                    GlobalDataSize;
  SYSTEM_TABLE_INFO        *SystemTableInfo;

  PcdStatus1 = PcdSet32S (PcdPayloadHobList, (UINT32)HobList);

  //
  // Payload Memmap
  // +--------------------------------------------+ TOLUM
  // |   Reserved memory for Slimboot core        |
  // +--------------------------------------------+ RsvdBase + RsvdSize
  // |   Reserved memory for Payload              |
  // +--------------------------------------------+ RsvdBase
  // |   + Payload global data                    |
  // +--------------------------------------------+ GlobalDataBase
  // |   + Payload heap                           |
  // +--------------------------------------------+ HeapBase
  // |   + Payload stack                          |
  // +--------------------------------------------+ StackBase
  // |   Free memory                              |
  // +--------------------------------------------+ 0
  //

  GetPayloadReservedRamRegion (&RsvdBase, &RsvdSize);
  ASSERT ((RsvdBase & EFI_PAGE_MASK) == 0);

  GlobalDataSize = ALIGN_UP (sizeof (PAYLOAD_GLOBAL_DATA), EFI_PAGE_SIZE);
  GlobalDataBase = (UINT32)RsvdBase - GlobalDataSize;
  PcdStatus2 = PcdSet32S (PcdGlobalDataAddress, (UINT32) (UINTN)GlobalDataBase);
  ASSERT_EFI_ERROR (PcdStatus1 | PcdStatus2);

  HeapSize  = ALIGN_UP (PcdGet32 (PcdPayloadHeapSize), EFI_PAGE_SIZE);
  HeapBase = GlobalDataBase - HeapSize;

  StackSize = ALIGN_UP (PcdGet32 (PcdPayloadStackSize), EFI_PAGE_SIZE);
  StackBase = HeapBase - StackSize;

  // Add payload reserved memory region and free memory region
  AddMemoryResourceRange (HeapBase, EFI_SIZE_TO_PAGES (HeapSize), \
                          RsvdBase, EFI_SIZE_TO_PAGES ((UINT32)RsvdSize));

  //
  // Initialize Global Data
  //
  PayloadGlobalDataInit ();

  //
  // Initialize PCIE base, Acpi Timer base
  //
  SystemTableInfo = GetSystemTableInfo ();
  if (SystemTableInfo != NULL) {
    ParseAcpiTableInfo ((UINT32)SystemTableInfo->AcpiTableBase);
  }

  //
  // Initialize Serial Port
  //
  SerialPortInitialize ();

  if (PldHeapBase != NULL) {
    *PldHeapBase = HeapBase;
  }

  if (PldStatckBase != NULL) {
    *PldStatckBase = StackBase;
  }

  if (PldStatckSize != NULL) {
    *PldStatckSize = StackSize;
  }
}

/**
  The payload common Entry Point for C code.

  @param[in] Params         The HOB list pointer for payload.
  @param[in] PldBase        Address of the payload image base.

**/
VOID
EFIAPI
SecStartup (
  IN VOID                   *Params,
  IN VOID                   *PldBase
  )
{
  UINT64                    TimeStamp;
  UINT32                    StackBase;
  UINT32                    StackSize;

  TimeStamp = AsmReadTsc();

  PayloadInit (Params, NULL, &StackBase, &StackSize);

  // DEBUG will be available after PayloadInit ()
  DEBUG ((DEBUG_INIT, "\nPayload startup\n"));
  AddMeasurePointTimestamp (0x4000, TimeStamp);

  DEBUG_CODE_BEGIN ();
  // Initialize HOB/Stack region with known pattern so that the usage can be detected
  SetMem32 (
    (VOID *)StackBase,
    StackSize,
    STACK_DEBUG_FILL_PATTERN
    );
  DEBUG_CODE_END ();

  SwitchStack ((SWITCH_STACK_ENTRY_POINT)PayloadMain, &Params, PldBase, (VOID *) (UINTN)(StackBase + StackSize));
}
