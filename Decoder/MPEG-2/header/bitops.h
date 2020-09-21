#ifndef BITOPS_H
#define BITOPS_H

#define BIT(n)				(1<<(n))
#define BITMASK(w)			(BIT(w) - 1)
#define BITGET(x, b, a)		(((x) >> (a)) & (BITMASK((b) - (a) + 1)))

#endif