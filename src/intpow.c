#include <stdint.h>
#include "intpow.h"

int64_t cbson_pow(int64_t ap, int32_t n)
{
  int64_t pow;

  if(n != 0)
  {
    if (n<0)
    {
      if ((ap!=1)&&(ap!=-1))
      {
        return(0);
      }

      n = -n;
    }

    pow=1;

    for( ; ; )
    {
      if(n & 01)
      {
        pow = pow * ap;
      }

      if(n >>= 1)
      {
        ap = ap * ap;
      }
      else
      {
        return(pow);
      }
    }
  }
  else return(1);
}

