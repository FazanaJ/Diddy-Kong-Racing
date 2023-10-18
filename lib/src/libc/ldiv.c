/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D7470 */

#include <stdlib.h>

ldiv_t ldiv(long num, long denom)
{
    ldiv_t ret;

    ret.quot = num / denom;
    ret.rem = num - denom * ret.quot;
    if (ret.quot < 0 && ret.rem > 0)
    {
        ret.quot++;
        ret.rem -= denom;
    }

    return ret;
}

lldiv_t lldiv(long long num, long long denom)
{
    lldiv_t ret;

    ret.quot = num / denom;
    ret.rem = num - denom * ret.quot;
    if (ret.quot < 0 && ret.rem > 0)
    {
        ret.quot++;
        ret.rem -= denom;
    }

    return ret;
}
