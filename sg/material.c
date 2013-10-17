#include <sg/gl/backend.h>
#include <sg/material.h>

#include <sg/ds/tzarray_p.h>
#include <sg/ds/tzarray_p.inl>

#include <sg/ds/tzaa_p.h>
#include <sg/ds/tzaa_p.inl>


struct material *
material_default(struct material *self) {
	self->ambient   = tzv4_mkp(0.80, 0.90, 0.80);
	self->diffuse   = tzv4_mkp(0.50, 0.50, 0.50);
	self->specular  = tzv4_mkp(0.10, 0.60, 0.10);
	self->emission  = tzv4_mkp(0.00, 0.00, 0.00);
	self->shininess = 1.0;
	return self;
}

struct material *material_init(struct material *self,
		tzv4 ambient,
		tzv4 diffuse,
		tzv4 specular) {
	material_default(self);
	self->ambient  = ambient;
	self->diffuse  = diffuse;
	self->specular = specular;
	return self;
}

void material_load(struct material *m) {
	glPushAttrib(GL_LIGHTING_BIT);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   m->ambient.f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   m->diffuse.f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m->specular.f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  m->emission.f);
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m->shininess);
}

void material_unload(struct material *m) {
	glPopAttrib();
}
