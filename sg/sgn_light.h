#ifndef SGN_LIGHT_H
#define SGN_LIGHT_H

#include <sg/sgn_base.h>
#include <sg/ds/tzarray_m4.h>

struct sgn_light {
	struct sgn_base base;
	struct _light {
		tzv4 ambient;
		tzv4 diffuse;
		tzv4 specular;
		tzv4 position;

		tzv4 attenuation;

		tzv4 spot_direction;
		float spot_exponent;
		float spot_cutoff;
	} light;
};

struct sgn_light *sgn_light_init(struct sgn_light *l, const char *name);

#endif /* SGN_LIGHT_H */
