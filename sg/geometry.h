#ifndef GEOM_H
#define GEOM_H

#include <stdint.h>
#include <tzmath/tzmath.h>

/* for shadow volume. */
struct vertex_300 {
	tzv3 pos;
};

struct vertex_332 {
	tzv3 pos;
	tzv3 normal;
	tzv3 texcoord;
};

struct geometry {
	uint32_t ibo, vbo, n;
	uint16_t type;
	tzv4 AA, BB;
};

struct geometry *geometry_mksphere  (struct geometry *geom, uint16_t lat, uint16_t lon);
struct geometry *geometry_mkcylinder(struct geometry *geom, uint16_t lon, uint16_t slices);
struct geometry *geometry_mkplane   (struct geometry *geom, uint16_t slices);
struct geometry *geometry_mktiled_plane   (struct geometry *geom, uint16_t slices);
struct geometry *geometry_mkcone    (struct geometry *geom, uint16_t lon, uint16_t slices);
void             geometry_draw      (struct geometry *geom, tzm4 *to_world);

#endif /* GEOM_H */
