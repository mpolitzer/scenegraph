#ifndef SGN_GEOM_H
#define SGN_GEOM_H

#include <sg/sgn_base.h>
#include <sg/geometry.h>
#include <sg/material.h>
#include <tzmath/tzmath.h>

struct sgn_geom {
	struct sgn_base base;
	struct geometry *geom;
	struct material *mat;
	tzm4 localT;
};

#define sgn_geom_localT(_self) ((_self)->localT)
#define sgn_geom_material(_self) ((_self)->material)
#define sgn_geom_geom(_self)   ((_self)->geom)
#define sgn_geom_mat(_self)   ((_self)->mat)
bool
sgn_isgeom(
		struct sgn_geom *self);
void sgn_geom_init(
		struct sgn_geom *self,
		const char *name,
		struct geometry *geom,
		struct material *mat);
struct sgn_geom *
sgn_geom_dup(
		struct sgn_geom *self,
		struct sgn_geom *src);

#endif /* SGN_GEOM_H */
