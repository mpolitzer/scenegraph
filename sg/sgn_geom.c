#include <sg/scene.h>
#include <sg/sgn_geom.h>
#include <sg/gl/backend.h>

typedef struct sgn_geom T;

/* vertex for surfaces generation. mksurface will convert to the final type. */
struct vertex {
	tzv4 pos, normal, texcoord;
};

static void geom_init(T *self);
static void geom_draw(T *self);

static void sgn_geom_pre_draw(struct sgn_base *_self, struct scene *scene) {
	/* find the to world transformation. */
	tzm4_mulm(      &sgn_base_to(_self),
			&sgn_base_to(_self->parent),
			&sgn_base_T(_self));
	sgn_base_pre_draw(_self, scene);
}

static void sgn_geom_draw(struct sgn_base *_self, struct scene *scene) {
	tzm4 tmp;
	T *self = (T *)_self;
	assert(sgn_isgeom(self) && "node is not a sgn_geom, fix vtbl.");
	/* apply the local transformation. */
	tzm4_mulm(&tmp, &sgn_base_to(_self),
			&sgn_geom_localT(self));
	glLoadMatrixf(tmp.f);
	geom_draw(self);
	sgn_base_draw(&self->base, scene);
}

static void sgn_geom_post_draw(struct sgn_base *_self, struct scene *scene) {
	sgn_base_post_draw(_self, scene);
}

static struct sgn_vtbl sgn_geom_vtbl = {
	.pre_draw  = sgn_geom_pre_draw,
	.draw      = sgn_geom_draw,
	.post_draw = sgn_geom_post_draw,
	.isvisible = sgn_base_isvisible,
	.addchild  = sgn_base_addchild,
	.update    = sgn_base_update,
};

static void sgn_geom_init(T *self, const char *name) {
	sgn_base_init(&self->base, &sgn_geom_vtbl);
	sgn_base_setname(&self->base, name);
	geom_init(self);
}

bool sgn_isgeom(T *self) {
	return self->base.vtbl == &sgn_geom_vtbl;
}

static void geom_init(T *self) {
	self->geom.ibo  = 0;
	self->geom.vbo  = 0;
	self->geom.n    = 0;
	self->geom.type = 0;
	self->geom.AA   = tzv4_mkv(0, 0, 0);
	self->geom.BB   = tzv4_mkv(0, 0, 0);
	tzm4_mkiden(&self->geom.localT);
}

static void mksurface(T *self, uint16_t dx, uint16_t dy,
		/* a surface from (-0.5 -> 0.5, -0.5 -> 0.5) */
		void (*fn)(float dx, float dy, struct vertex *vert)) {
	/* store the AABB bounding box of the surface. */
	tzv4 min = tzv4_mk1f(0.0), max = tzv4_mk1f(0.0);
	int16_t i, j;
	uint32_t vbo=0, ibo=0;
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
			struct vertex v;
			fn(fj-0.5, fi-0.5, &v);
			v.pos = tzm4_mulv(&self->geom.localT, v.pos);
			max = tzv4_max(max, v.pos);
			min = tzv4_min(min, v.pos);
			/* make the vertex_332 */
			pvert->pos = tzv4_to3(v.pos);
			pvert->normal = tzv4_to3(v.normal);
			pvert->texcoord = tzv4_to2(v.texcoord);

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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	free(inds);
	free(vert);

	self->geom.vbo = vbo;
	self->geom.ibo = ibo;
	self->geom.n   = ninds;
	self->geom.AA  = min;
	self->geom.BB  = max;
}

static void sphere(float theta, float phi, struct vertex *vert) {
	/* phi -> [0, PI], theta -> [-PI, PI] */
	float r  = 0.5;
	float sp = sin(M_PI*(phi+0.5)), st = sin(2*M_PI*theta);
	float cp = cos(M_PI*(phi+0.5)), ct = cos(2*M_PI*theta);
	vert->pos      = tzv4_mkp(-r*ct*sp,  -r*cp,  -r*sp*st);
	vert->normal   = tzv4_mkp( -ct*sp,    -cp,    -sp*st);
	vert->texcoord = tzv4_mkp(r*ct*sp+r, r*sp*st+r, 0);
}

static void plane(float di, float dj, struct vertex *vert) {
	vert->pos      = tzv4_mkp(di, 0, dj);
	vert->normal   = tzv4_mkp(0,  1,  0);
	vert->texcoord = tzv4_mkp(di, dj, 0);
}

static void cylinder(float theta, float slices, struct vertex *vert) {
	float st  = sin(2*M_PI*(theta));
	float ct  = cos(2*M_PI*(theta));
	float  r = 0.5;

	vert->pos      = tzv4_mkp(r*ct, slices, r*st);   /* -0.5 to 0.5 */
	vert->normal   = tzv4_mkp(  ct,   0,      st);   /* -1.0 to 1.0 */
	vert->texcoord = tzv4_mkp(ct+0.5, st+0.5, 0);    /*  0   to 1.0 */
}

static void cone(float theta, float slices, struct vertex *vert) {
	float st  = sin(2*M_PI*(theta));
	float ct  = cos(2*M_PI*(theta));

	slices = 0.5*(slices-0.5);
	vert->pos = tzv4_mkp(slices*ct, slices+0.5, slices*st); /* -0.5 to 0.5 */
	vert->normal   = tzv4_mkp(-ct, 0.5, -st);            /* -1.0 to 1.0 */
	vert->texcoord = tzv4_mkp(ct+0.5, st+0.5, 0);           /*  0   to 1.0 */
}

T *sgn_geom_mksphere(T *self, const char *name, uint16_t lat, uint16_t lon) {
	sgn_geom_init(self, name);
	mksurface(self, lat, lon, sphere);
	return self;
}

T *sgn_geom_mkplane(T *self, const char *name, uint16_t slices) {
	sgn_geom_init(self, name);
	mksurface(self, slices, slices, plane);
	return self;
}

T *sgn_geom_mkcylinder(T *self, const char *name, uint16_t theta, uint16_t slices) {
	sgn_geom_init(self, name);
	mksurface(self, theta, slices, cylinder);
	return self;
}

T *sgn_geom_mkcone(T *self, const char *name, uint16_t theta, uint16_t slices) {
	sgn_geom_init(self, name);
	mksurface(self, theta, slices, cone);
	return self;
}

T *sgn_geom_dup(T *self, T *src) {
	*self = *src;
	return self;
}

/* get the member offset */
#define _t(_member) (&((struct vertex_332*)0)->_member)
static void geom_draw(T *self) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->geom.ibo);
	glBindBuffer(GL_ARRAY_BUFFER, self->geom.vbo);

	glVertexPointer  (3, GL_FLOAT, sizeof(struct vertex_332), _t(pos));
	glNormalPointer  (   GL_FLOAT, sizeof(struct vertex_332), _t(normal));
	glTexCoordPointer(2, GL_FLOAT, sizeof(struct vertex_332), _t(texcoord));

	glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glDrawElements(GL_TRIANGLE_STRIP, self->geom.n, GL_UNSIGNED_SHORT, 0);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
#undef _t
