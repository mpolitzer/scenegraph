#ifndef SCENE_H
#define SCENE_H

#include <sg/sgn_base.h>
#include <sg/sgn_cam.h>
#include <tzmath/tzmath.h>
#include <sg/ds/tzarray_m4.h>

struct scene {
	struct sgn_base root;
	struct sgn_base *active_cam;
};

enum render_pass {
	render_pass_setup_lightmaps,
	render_pass_setup_transformations,
	render_pass_setup_camera,
	render_pass_geometry
};

void             scene_init     (struct scene *scene);
struct sgn_base *scene_getroot  (struct scene *scene);
tzm4            *scene_getmstack(struct scene *scene);
void             scene_draw     (struct scene *scene);
void             scene_update   (struct scene *scene, uint16_t ms);
void             scene_setcam   (struct scene *scene, struct sgn_base *cam);

#endif /* SCENE_H */
