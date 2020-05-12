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
  return FALSE;
}
