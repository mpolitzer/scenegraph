#ifndef CAM_CTL_H
#define CAM_CTL_H

#include <sg/sgn_cam.h>

struct cam_ctl {
	float x, y, z;
	float r, phi, theta;
};

void cam_ctl_update(struct sgn_cam *cam, struct cam_ctl *ctl);

#endif /* CAM_CTL_H */
