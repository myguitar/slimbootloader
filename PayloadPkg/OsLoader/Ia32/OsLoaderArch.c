#include <Guid/LoaderPlatformInfoGuid.h>
#include <Register/Intel/Msr/ArchitecturalMsr.h>

/**
  Checks whether the platform is a pre-production part; or in manufacturing mode or debug mode.

  @retval  TRUE    Platform is a pre-production part; or in manufacturing mode or debug mode.
  @retval  FALSE   Platfomr is not a pre-production part; or in manufacturing mode or debug mode.
**/
BOOLEAN
PlatformDebugStateEnabled (
  IN UINT16 HwState
  )
{
  MSR_IA32_FEATURE_CONTROL_REGISTER Ia32FeatureControlMsr;
  MSR_IA32_DEBUG_INTERFACE_REGISTER DebugInterfaceMsr;

  //
  // Check for Platform in Pre-production or in Manufaturing mode or secure debug mode
  //
  if((HwState & HWSTATE_IN_MANU_SECURE_DEBUG_MODE) || (HwState & HWSTATE_PLATFORM_PRE_PRODUCTION)){
    return TRUE;
  }

  //
  // Check for Sample part
  //
  Ia32FeatureControlMsr.Uint64 = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);
  if (Ia32FeatureControlMsr.Bits.Lock == 0) {
    return TRUE;
  }

  //
  // Check for Debug mode
  //
  DebugInterfaceMsr.Uint64 = AsmReadMsr64 (MSR_IA32_DEBUG_INTERFACE);
  if (DebugInterfaceMsr.Bits.Enable) {
    return TRUE;
  }

  return FALSE;
}
