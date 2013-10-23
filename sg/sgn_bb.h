#ifndef SGN_BB_H
#define SGN_BB_H

#include <sg/sgn_base.h>
#include <sg/sgn_geom.h>

void sgn_bb_init(
		struct sgn_geom *self,
		const char *name,
		struct geometry *geom,
		struct material *mat,
		struct texture *tex);

#endif /* SGN_BB_H */
