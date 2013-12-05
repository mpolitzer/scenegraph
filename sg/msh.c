#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <sg/msh.h>
#include <sg/gl/backend.h>

struct geometry *geometry_loadmsh(struct geometry *self,
		const char *fname) {
	GLuint vbo=0, ibo=0, sz=0;
	struct vertex_332 *vert, *pvert;
	uint16_t *inds, *pinds;

	int i, nverts, ninds;
	char buf[256];
	FILE *in = fopen(fname, "r");
	assert(in && "failed to open .msh file");

	/* read header */
	fgets(buf, sizeof buf, in);
	sscanf(buf, "%d %d\n", &nverts, &ninds); /* grab nverts and ntris */
	ninds *= 3; /* 3 for each triangle */
	sz = nverts*sizeof(*vert);
	printf("%d %d\n", nverts, ninds);
	vert = malloc(sz);
	inds = malloc( ninds*sizeof(*inds));
	assert(vert && inds && "failed to allocate for .msh");

	for (i=0, pvert = vert; i<nverts; i++, pvert++) {
		int _;
		fgets(buf, sizeof buf, in);
		sscanf(buf, " %d %f %f %f %f %f %f\n", &_,
				&pvert->pos.m.x,
				&pvert->pos.m.y,
				&pvert->pos.m.z,
				&pvert->normal.m.x,
				&pvert->normal.m.y,
				&pvert->normal.m.z);
	}

	for (i=0, pinds = inds; i<ninds/3; i++, pinds += 3) {
		int _;
		fgets(buf, sizeof buf, in);
		sscanf(buf, "%u %hu %hu %hu\n", &_, pinds+0, pinds+1, pinds+2);
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
	self->type= GL_TRIANGLES;

	return self;
}
