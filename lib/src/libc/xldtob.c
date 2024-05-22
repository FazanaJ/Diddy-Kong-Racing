
/* The comment below is needed for this file to be picked up by generate_ld */
/* RAM_POS: 0x800D69A0 */

#include "macros.h"

#include "xprintf.h"
#include "PR/os_libc.h"

extern const double pows[];

typedef struct ldiv_t
{
    long quot;
    long rem;
} ldiv_t;

ldiv_t ldiv(long num, long denom);

void _Genld(printf_struct* px, u8 code, char* p, s16 nsig, s16 xexp) {
    const unsigned char point = '.';

    if (nsig <= 0) {
        nsig = 1;
        p = "0";
    }

    if (code == 'f' || ((code == 'g' || code == 'G') && xexp >= -4 && xexp < px->precision)) {
        xexp += 1;
        if (code != 'f') {
            if (((px->flags & 8) == 0) && nsig < px->precision) {
                px->precision = nsig;
            }

            px->precision -= xexp;
            if (px->precision < 0) {
                px->precision = 0;
            }
        }

        if (xexp <= 0) {
            px->buff[px->part2_len++] = '0';

            if (px->precision > 0 || (px->flags & 8)) {
                px->buff[px->part2_len++] = point;
            }

            if (px->precision < -xexp) {
                xexp = -px->precision;
            }

            px->num_mid_zeros = -xexp;
            px->precision += xexp;

            if (px->precision < nsig) {
                nsig = px->precision;
            }

            bcopy(p, &px->buff[px->part2_len], px->part3_len = nsig); // whitespace memes (this one is insane)
            px->num_trailing_zeros = px->precision - nsig;
        }
        else if (nsig < xexp) {
            bcopy(p, &px->buff[px->part2_len], nsig);
            px->part2_len += nsig;
            px->num_mid_zeros = xexp - nsig;
            if (px->precision > 0 || (px->flags & 8)) {
                px->buff[px->part2_len] = point;
                px->part3_len += 1;
            }

            px->num_trailing_zeros = px->precision;
        }
        else {
            bcopy(p, &px->buff[px->part2_len], xexp);
            px->part2_len += xexp;
            nsig -= xexp;

            if (px->precision > 0 || (px->flags & 8)) {
                px->buff[px->part2_len++] = point;
            }

            if (px->precision < nsig) {
                nsig = px->precision;
            }

            bcopy(&p[xexp], &px->buff[px->part2_len], nsig);
            px->part2_len += nsig;
            px->num_mid_zeros = px->precision - nsig;
        }
    }
    else {
        if (code == 'g' || code == 'G') {
            if (nsig < px->precision) {
                px->precision = nsig;
            }

            if (--px->precision < 0) {
                px->precision = 0;
            }

            if (code == 'g') {
                code = 'e';
            }
            else {
                code = 'E';
            }
        }

        px->buff[px->part2_len++] = *p++;

        if (px->precision > 0 || (px->flags & 8)) {
            px->buff[px->part2_len++] = point;
        }

        if (px->precision > 0) {
            if (px->precision < --nsig) {
                nsig = px->precision;
            }

            bcopy(p, &px->buff[px->part2_len], nsig);
            px->part2_len += nsig;
            px->num_mid_zeros = px->precision - nsig;
        }

        p = &px->buff[px->part2_len];
        *p++ = code;

        if (xexp >= 0) {
            *p++ = '+';
        }
        else {
            *p++ = '-';
            xexp = -xexp;
        }

        if (xexp >= 0x64) {
            if (xexp >= 0x3E8) {
                *p++ = (xexp / 1000) + '0', xexp %= 1000; // whitespace memes
            }
            *p++ = (xexp / 100) + '0', xexp %= 100; // whitespace memes
        }
        *p++ = (xexp / 10) + '0', xexp %= 10; // whitespace memes

        *p++ = xexp + '0';
        px->part3_len = p - &px->buff[px->part2_len];
    }

    if ((px->flags & 0x14) == 0x10) {
        s32 n = px->n0 + px->part2_len + px->num_mid_zeros + px->part3_len + px->num_trailing_zeros;

        if (n < px->width) {
            px->num_leading_zeros = px->width - n;
        }
    }
}

// float properties
#define _D0 0
#define _DBIAS 0x3ff
#define _DLONG 1
#define _DOFF 4
#define _FBIAS 0x7e
#define _FOFF 7
#define _FRND 1
#define _LBIAS 0x3ffe
#define _LOFF 15
// integer properties
#define _C2 1
#define _CSIGN 1
#define _ILONG 0
#define _MBMAX 8
#define NAN 2
#define INF 1
#define FINITE -1
#define _DFRAC ((1 << _DOFF) - 1)
#define _DMASK (0x7fff & ~_DFRAC)
#define _DMAX ((1 << (15 - _DOFF)) - 1)
#define _DNAN (0x8000 | _DMAX << _DOFF | 1 << (_DOFF - 1))
#define _DSIGN 0x8000
#define _D1 1 // big-endian order
#define _D2 2
#define _D3 3

s16 _Ldunscale(s16* pex, printf_struct* px) {
    u16* ps = (u16 *) px;
    s16 xchar = (ps[_D0] & _DMASK) >> _DOFF;


    if (xchar == _DMAX) {
        *pex = 0;

        return (ps[_D0] & _DFRAC) || ps[_D1] || ps[_D2] || ps[_D3] ? 2 : 1;
    }
    else if (xchar > 0) {
        ps[_D0] = (ps[_D0] & ~_DMASK) | 0x3FF0;
        *pex = xchar - 0x3FE;
        return -1;
    }
    else if (xchar < 0) {
        return 2;
    }
    else {
        *pex = 0;
        return 0;
    }
}

void _Ldtob(printf_struct* px, u8 code) {
    char buff[32];
    char *p;
    f64 ldval;
    short err;
    short nsig;
    short xexp;
    p = buff;
    ldval = px->value.f64;
    if (px->precision < 0) {
        px->precision = 6;
    } else if (px->precision == 0 && (code == 'g' || code == 'G')) {
        px->precision = 1;
    }
    err = _Ldunscale(&xexp, px);
    if (err > 0) {
        bcopy(err == 2 ? "NaN" : "Inf", px->buff, px->part2_len = 3);
        return;
    } else if (err == 0) {
        nsig = 0;
        xexp = 0;
    } else {
        {
            int i;
            int n;
            if (ldval < 0) {
                ldval = -ldval;
            }
            if ((xexp = xexp * 30103 / 100000 - 4) < 0) {
                n = (((u32)(-xexp) + ((4)-1)) & ~((4)-1)), xexp = -n;
                for (i = 0; n > 0; n >>= 1, i++) {
                    if (n & 1) {
                        ldval *= pows[i];
                    }
                }
            } else if (xexp > 0) {
                f64 factor = 1;
                xexp &= ~3;
                for (n = xexp, i = 0; n > 0; n >>= 1, i++) {
                    if (n & 1) {
                        factor *= pows[i];
                    }
                }
                ldval /= factor;
            }
        }
        {
            int gen = px->precision + ((code == 'f') ? 10 + xexp : 6);
            if (gen > 0x13) {
                gen = 0x13;
            }
            for (*p++ = '0'; gen > 0 && 0 < ldval; p += 8) {
                int j;
                long lo = ldval;
                if ((gen -= 8) > 0) {
                    ldval = (ldval - lo) * 1e8;
                }
                for (p += 8, j = 8; lo > 0 && --j >= 0;) {
                    ldiv_t qr = ldiv(lo, 10);
                    *--p = qr.rem + '0', lo = qr.quot;
                }
                while (--j >= 0) {
                    *--p = '0';
                }
            }
            gen = p - &buff[1];
            for (p = &buff[1], xexp += 7; *p == '0'; p++) {
                --gen, --xexp;
            }
            nsig = px->precision + ((code == 'f') ? xexp + 1 : ((code == 'e' || code == 'E') ? 1 : 0));
            if (gen < nsig) {
                nsig = gen;
            }
            if (nsig > 0) {
                const char drop = nsig < gen && '5' <= p[nsig] ? '9' : '0';
                int n;
                for (n = nsig; p[--n] == drop;) {
                    --nsig;
                }
                if (drop == '9') {
                    ++p[n];
                }
                if (n < 0) {
                    --p, ++nsig, ++xexp;
                }
            }
        }
    }
    _Genld(px, code, p, nsig, xexp);
}
