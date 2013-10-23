#ifndef LIGHT_H
#define LIGHT_H

#include <tzmath/tzmath.h>

struct light {
	tzv4 ambient;
	tzv4 diffuse;
	tzv4 specular;
	tzv4 position;

	tzv4 attenuation;

	tzv4 spot_direction;
	float spot_exponent;
	float spot_cutoff;
};

void light_init(struct light *self);
void light_load(struct light *self, tzm4 *T);
void light_unload(struct light *l);
int  lights_count(void);
int  lights_max(void);
void lights_disable(void);

#endif /* LIGHT_H */
