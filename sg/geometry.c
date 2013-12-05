#include <sg/geometry.h>
#include <sg/gl/backend.h>

void geometry_mksurface(struct geometry *self, uint16_t dx, uint16_t dy,
		/* a surface from (-0.5 -> 0.5, -0.5 -> 0.5) */
		void (*fn)(float dx, float dy, struct vertex_332 *vert)) {
	/* store the AABB bounding box of the surface. */
	tzv4 min = tzv4_mk1f(0.0), max = tzv4_mk1f(0.0);
	int16_t i, j;
	GLuint vbo=0, ibo=0;
	uint16_t *inds, *pinds;
	struct vertex_332 *vert, *pvert;
	size_t ninds, sz;
	ninds = 2*(dx+1)*(dy);
	sz = ninds * sizeof(*vert);
	pvert = vert = malloc(sz);
	pinds = inds = malloc(ninds*sizeof(uint16_t)); /* ??? */

	/* make vertices */
	for (i=0; i<=dx; ++i) {
		float fi = i/(float)dx;
		for (j=0; j<=dy; ++j, ++pvert) {
			float fj = j/(float)dy;
			fn(fj-0.5, fi-0.5, pvert);
			max = tzv4_max(max, tzv4_mkv3(pvert->pos));
			min = tzv4_min(min, tzv4_mkv3(pvert->pos));
		}
	}
	/* make indices */
	for (i=0; i<=dx-1; ++i) {
		for (j=0; j<=dy; ++j) {
			*pinds++ = (j+0)+(i+0)*(dx+1);
			*pinds++ = (j+0)+(i+1)*(dx+1);
		}
		if (++i <= dy-1) for (--j; j>=0; --j) {
			*pinds++ = (j+0)+(i+1)*(dx+1);
			*pinds++ = (j+0)+(i+0)*(dx+1);
		}
	}
	/* ibo (index buffer) */
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t)*ninds,
			inds, GL_STATIC_DRAW);

	/* vbo (vertex buffer) */
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sz, vert, GL_STATIC_DRAW);

	free(inds);
	free(vert);

	self->vbo = vbo;
	self->ibo = ibo;
	self->n   = ninds;
	self->AA  = min;
	self->BB  = max;
	self->type= GL_TRIANGLE_STRIP;
}

static void polar(float theta, float phi, struct vertex_332 *vert) {
	/* phi -> [0, PI], theta -> [-PI, PI] */
	float r  = 0.5;
	float sp  = sin(M_PI*(phi-0.5)),  st = sin(2*M_PI*theta);
	float cp  = cos(M_PI*(phi-0.5)),  ct = cos(2*M_PI*theta);

	vert->pos      = tzv3_mk(r*ct*sp,   r*cp,   r*sp*st);
	vert->normal   = tzv3_mk( ct*sp,    cp, sp*st);
	vert->texcoord = tzv3_mk(-theta+0.5,-phi+0.5, 0);
}

static void tiled_plane(float di, float dj, struct vertex_332 *vert) {
	vert->pos      = tzv3_mk(    di,      0, dj);
	vert->normal   = tzv3_mk(     0,      1,  0);
	vert->texcoord = tzv3_mk(di+0.5, dj+0.5,  0);
}

static void plane(float di, float dj, struct vertex_332 *vert) {
	vert->pos      = tzv3_mk(    di,      0, dj);
	vert->normal   = tzv3_mk(     0,      1,  0);
	vert->texcoord = tzv3_mk(di+0.5, dj+0.5,  0);
}

static void cylinder(float theta, float slices, struct vertex_332 *vert) {
	float st  = sin(2*M_PI*(theta));
	float ct  = cos(2*M_PI*(theta));
	const float  r = 0.5;

	vert->pos      = tzv3_mk(r*ct, slices, r*st);   /* -0.5 to 0.5 */
	vert->normal   = tzv3_mk(  ct,   0,      st);   /* -1.0 to 1.0 */
	vert->texcoord = tzv3_mk(ct+0.5, slices, 0);    /*  0   to 1.0 */
}

static void cone(float theta, float slices, struct vertex_332 *vert) {
	float st  = sin(2*M_PI*(theta));
	float ct  = cos(2*M_PI*(theta));

	slices = 0.5*(slices+0.5);
	vert->pos      = tzv3_mk(slices*ct, slices, slices*st);   /* -0.5 to 0.5 */
	vert->normal   = tzv3_mk(  ct,      slices,        st);   /* -1.0 to 1.0 */
	vert->texcoord = tzv3_mk(ct+0.5, st+0.5, 0);              /*  0   to 1.0 */
}

void geometry_init(struct geometry *self) {
	self->ibo = 0;
	self->vbo = 0;
	self->n   = 0;
}

struct geometry *
geometry_mksphere(struct geometry *self, uint16_t lat, uint16_t lon) {
	geometry_mksurface(self, lat, lon, polar);
	return self;
}

struct geometry *
geometry_mkplane(struct geometry *self, uint16_t slices) {
	geometry_mksurface(self, slices, slices, plane);
	return self;
}

struct geometry *
geometry_mktiled_plane(struct geometry *self, uint16_t slices) {
	geometry_mksurface(self, slices, slices, tiled_plane);
	return self;
}

struct geometry *
geometry_mkcylinder(struct geometry *self, uint16_t theta, uint16_t slices) {
	geometry_mksurface(self, theta, slices, cylinder);
	return self;
}

struct geometry *
geometry_mkcone(struct geometry *self, uint16_t theta, uint16_t slices) {
	geometry_mksurface(self, theta, slices, cone);
	return self;
}

/* get the member offset */
#define _t(_member) (&((struct vertex_332*)0)->_member)
void geometry_draw(struct geometry *self, tzm4 *to_world) {
	glLoadMatrixf(to_world->f);

	glBindBuffer(GL_ARRAY_BUFFER, self->vbo);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer  (3, GL_FLOAT, sizeof(struct vertex_332), _t(pos));
	glNormalPointer  (   GL_FLOAT, sizeof(struct vertex_332), _t(normal));
	glTexCoordPointer(3, GL_FLOAT, sizeof(struct vertex_332), _t(texcoord));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ibo);
	glDrawElements(self->type, self->n, GL_UNSIGNED_SHORT, 0);
}
#undef _t
