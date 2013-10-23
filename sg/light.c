#include <sg/light.h>
#include <sg/gl/backend.h>

typedef struct light T;
static int _lights;

/* make a default light */
void light_init(T *self) {
	self->ambient        = tzv4_mkp(0.3, 0.3, 0.3);
	self->diffuse        = tzv4_mkp(0.3, 0.3, 0.3);
	self->specular       = tzv4_mkp(0.3, 0.3, 0.3);
	self->position       = tzv4_mkp(0  , 0  , 0  );
	self->attenuation    = tzv4_mkp(0  , 0  , 0  );
	self->spot_direction = tzv4_mkp(0  , 1  , 0  );
	self->spot_exponent  =   1.0;
	self->spot_cutoff    = 180.0;
}

/* backend ops */
void light_load(T *self, tzm4 *T) {
	glLoadMatrixf(T->f);
	glPushAttrib(GL_LIGHTING_BIT);
	glEnable (GL_LIGHT0 + _lights);
	glLightfv(GL_LIGHT0 + _lights, GL_AMBIENT,       self->ambient.f);
	glLightfv(GL_LIGHT0 + _lights, GL_DIFFUSE,       self->diffuse.f);
	glLightfv(GL_LIGHT0 + _lights, GL_SPECULAR,      self->specular.f);
	glLightfv(GL_LIGHT0 + _lights, GL_POSITION,      self->position.f);
	glLightf (GL_LIGHT0 + _lights, GL_SPOT_EXPONENT, self->spot_exponent);
	glLightf (GL_LIGHT0 + _lights, GL_SPOT_CUTOFF,   self->spot_cutoff);
	glLightfv(GL_LIGHT0 + _lights, GL_SPOT_DIRECTION,self->spot_direction.f);
	++_lights;
}

void light_unload(T *l) {
	--_lights;
	glPopAttrib();
}

int light_count(void) {
	return _lights;
}

int light_max(void) {
	int nlights=0;
	glGetIntegerv(GL_MAX_LIGHTS, &nlights);
	return nlights;
}

void lights_disable(void) {
	int i;
	for (i=0; i<_lights; i++) {
		glPopAttrib();
		glDisable(GL_LIGHT0+i);
	}
	_lights=0;
}
