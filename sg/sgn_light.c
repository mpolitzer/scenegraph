#include <sg/sgn_light.h>
#include <sg/gl/backend.h>

typedef struct sgn_light T;

static void light_init  (T *self);
static void light_load  (T *self, tzm4 *T);
static void light_unload(T *self);

/* num of lights in use */
static int _lights;

void sg_core_axes(void) {
	glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3f(1, 0, 0); glVertex3f(0, 0, 0);glVertex3f(1, 0, 0);
        glColor3f(0, 1, 0); glVertex3f(0, 0, 0);glVertex3f(0, 1, 0);
        glColor3f(0, 0, 1); glVertex3f(0, 0, 0);glVertex3f(0, 0, 1);
        glEnd();
	glEnable(GL_LIGHTING);
}

void sgn_light_pre_draw(struct sgn_base *_self, struct scene *scene) {
	T *self = (T *)_self;
	sgn_base_pre_draw(_self, scene);
	light_load(self, &sgn_base_to(_self));
	sg_core_axes();
}

void sgn_light_post_draw(struct sgn_base *_self, struct scene *scene) {
	T *self = (T *)_self;
	sgn_base_post_draw(_self, scene);
	light_unload(self);
}

static struct sgn_vtbl sgn_light_vtbl = {
	.pre_draw  = sgn_light_pre_draw,
	.draw      = sgn_base_draw,
	.post_draw = sgn_light_post_draw,
	.isvisible = sgn_base_isvisible,
	.addchild  = sgn_base_addchild,
	.update    = sgn_base_update,
};

T *sgn_light_init(T *self, const char *name) {
	sgn_base_init(&self->base, &sgn_light_vtbl);
	sgn_base_name(&self->base) = name;
	light_init(self);
	return self;
}

/* make a default light */
static void light_init(T *self) {
	self->light.ambient        = tzv4_mkp(0.2, 0.2, 0.2);
	self->light.diffuse        = tzv4_mkp(0.2, 0.2, 0.2);
	self->light.specular       = tzv4_mkp(0.2, 0.2, 0.2);
	self->light.position       = tzv4_mkp(0  , 0  , 0  );
	self->light.attenuation    = tzv4_mkp(0  , 0  , 0  );
	self->light.spot_direction = tzv4_mkp(0  , 1  , 0  );
	self->light.spot_exponent  =   0.0;
	self->light.spot_cutoff    = 180.0;
}

/* backend ops */
static void light_load(T *self, tzm4 *T) {
	glLoadMatrixf(T->f);
	glPushAttrib(GL_LIGHTING_BIT);
	glEnable (GL_LIGHT0 + _lights);
	glLightfv(GL_LIGHT0 + _lights, GL_AMBIENT,       self->light.ambient.f);
	glLightfv(GL_LIGHT0 + _lights, GL_DIFFUSE,       self->light.diffuse.f);
	glLightfv(GL_LIGHT0 + _lights, GL_SPECULAR,      self->light.specular.f);
	glLightfv(GL_LIGHT0 + _lights, GL_POSITION,      self->light.position.f);
	glLightf (GL_LIGHT0 + _lights, GL_SPOT_EXPONENT, self->light.spot_exponent);
	glLightf (GL_LIGHT0 + _lights, GL_SPOT_CUTOFF,   self->light.spot_cutoff);
	glLightfv(GL_LIGHT0 + _lights, GL_SPOT_DIRECTION,self->light.spot_direction.f);
	++_lights;
}

static void light_unload(T *l) {
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
