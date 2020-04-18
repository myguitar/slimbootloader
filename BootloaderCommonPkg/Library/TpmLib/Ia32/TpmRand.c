#include <Library/BaseLib.h>

BOOLEAN
EFIAPI
ReadRand32 (
  OUT     UINT32                    *Rand
  )
{
  return AsmRdRand32 (Rand);
}
