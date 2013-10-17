#ifndef SGN_LIGHT_H
#define SGN_LIGHT_H

#include <sg/sgn_base.h>
#include <sg/light.h>

struct sgn_light {
	struct sgn_base base;
	struct light light;
};

struct sgn_light *sgn_light_init(struct sgn_light *l, const char *name);

#endif /* SGN_LIGHT_H */
