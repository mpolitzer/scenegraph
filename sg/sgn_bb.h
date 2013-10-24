#ifndef SGN_BB_H
#define SGN_BB_H

#include <sg/sgn_base.h>

struct sgn_bb {
	struct sgn_base base;
	struct geometry *geom;
	struct material *mat;
	struct texture  *tex;
	tzm4 localT, localI;
};

#define sgn_bb_localT(_self)   (&(_self)->localT)
#define sgn_bb_material(_self) (&(_self)->material)
#define sgn_bb_geom(_self)     (&(_self)->geom)
#define sgn_bb_mat(_self)      (&(_self)->mat)
#define sgn_bb_tex(_self)      (&(_self)->tex)

void sgn_bb_init(
		struct sgn_bb *self,
		const char *name,
		struct geometry *geom,
		struct material *mat,
		struct texture *tex);

#endif /* SGN_BB_H */
