#include <math.h>
#include <sg/cam_ctl.h>
#include <sg/sgn_cam.h>

void cam_ctl_init(struct cam_ctl *ctl) {
	ctl->x     = 0;
	ctl->y     = 0;
	ctl->z     = 0;
	ctl->r     = 1.0;
	ctl->phi   = 1;
	ctl->theta = M_PI/2;
}

void cam_ctl_update(struct sgn_cam *cam, struct cam_ctl *ctl) {
	float sp = sin(ctl->phi),   cp = cos(ctl->phi),
	      st = sin(ctl->theta), ct = cos(ctl->theta),
	      r  = ctl->r;

	tzm4_lookat(&sgn_base_I(&cam->base),
			tzv4_mkp(r*ct*sp,   r*cp, r*sp*st),
			tzv4_mkp( ctl->x, ctl->y,  ctl->z),
			tzv4_mkp(      0,      1,       0));
	tzm4_inverse(&sgn_base_T(&cam->base), &sgn_base_I(&cam->base));
}
