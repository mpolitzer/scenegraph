#ifndef GEOM_H
#define GEOM_H

#include <stdint.h>
#include <tzmath/tzmath.h>

struct geometry {
	uint16_t ibo, vbo, n;
	uint16_t type;
	tzv4 AA, BB;
};

struct geometry *geometry_mksphere  (struct geometry *geom, uint16_t lat, uint16_t lon);
struct geometry *geometry_mkcylinder(struct geometry *geom, uint16_t lon, uint16_t slices);
struct geometry *geometry_mkplane   (struct geometry *geom, uint16_t slices);
struct geometry *geometry_mkcone    (struct geometry *geom, uint16_t lon, uint16_t slices);
void             geometry_draw      (struct geometry *geom, tzm4 *to_world);

#endif /* GEOM_H */
