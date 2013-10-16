#ifndef SGN_GEOM_H
#define SGN_GEOM_H

#include <sg/sgn_base.h>
#include <tzmath/tzmath.h>

struct vertex_332 {
	tzv3 pos;
	tzv3 normal;
	tzv2 texcoord;
};

struct sgn_geom {
	struct sgn_base base;
	struct _geom {
		uint16_t ibo, vbo, n;
		uint16_t material, type;
		tzv4 AA, BB;
		tzm4 localT;
	} geom;
};

#define sgn_geom_localT(_self) ((_self)->geom.localT)
bool
sgn_isgeom(
		struct sgn_geom *self);
struct sgn_geom *
sgn_geom_mksphere(
		struct sgn_geom *self,
		const char *name,
		uint16_t lat,
		uint16_t lon);
struct sgn_geom *
sgn_geom_mkplane(
		struct sgn_geom *self,
		const char *name,
		uint16_t slices);
struct sgn_geom *
sgn_geom_mkcylinder(
		struct sgn_geom *self,
		const char *name,
		uint16_t theta,
		uint16_t slices);
struct sgn_geom *
sgn_geom_mkcone(
		struct sgn_geom *self,
		const char *name,
		uint16_t theta,
		uint16_t slices);
struct sgn_geom *
sgn_geom_dup(
		struct sgn_geom *self,
		struct sgn_geom *src);
tzm4 *
sgn_geom_getT(
		struct sgn_geom *self);

#endif /* SGN_GEOM_H */
