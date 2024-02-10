#ifndef _KERNEL_UTILS_H
#define _KERNEL_UTILS_H

#define MAX(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

#define DIV_ROUND_UP(x,y) ({__typeof__ (y) _y = (y); (x + _y - 1) / _y;})

#endif
