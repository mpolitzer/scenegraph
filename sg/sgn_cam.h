#ifndef SGN_CAM_H
#define SGN_CAM_H

#include <sg/sgn_base.h>
#include <sg/scene.h>

struct sgn_cam {
	struct sgn_base base;
};


#define         sgn_cam_new(_n) sgn_cam_init(malloc(sizeof (struct sgn_cam)), _n)
struct sgn_cam *sgn_cam_init (struct sgn_cam *self, const char *name);

void            sgn_cam_setup(struct sgn_cam *self, struct scene *scene);
bool            sgn_iscam(struct sgn_cam *self);

#endif /* SGN_CAM_H */
