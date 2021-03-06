#ifndef MATERIAL_H
#define MATERIAL_H

#include <tzmath/tzmath.h>

/* http://www.kynd.info/log/?p=201 */
struct material {
	tzv4 ambient;
	tzv4 diffuse;
	tzv4 specular;
	tzv4 emission;
	float shininess;
	float opacity;
};

struct material *material_init(struct material *self,
		tzv4 ambient,
		tzv4 diffuse,
		tzv4 specular,
		tzv4 emission,
		float shi);

struct material *
material_default(struct material *m);
void
material_load   (struct material *m);
void
material_unload (struct material *m);

#endif /* MATERIAL_H */
