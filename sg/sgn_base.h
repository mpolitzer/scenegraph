#ifndef SG_NODE_H
#define SG_NODE_H

#include <stdbool.h>
#include <tzmath/tzmath.h>
#include <sg/ds/tzarray_p.h>

/* they exist somewhere */
struct sgn_base;
struct scene;

struct sgn_vtbl {
	void (*pre_draw) (struct sgn_base *base, struct scene *scene);
	void (*draw)     (struct sgn_base *base, struct scene *scene);
	void (*post_draw)(struct sgn_base *base, struct scene *scene);
	bool (*isvisible)(struct sgn_base *base, struct scene *scene);
	void (*addchild) (struct sgn_base *base, struct sgn_base *child);

	void (*update)   (struct sgn_base *base, uint16_t ms);
};

struct sgn_base {
	struct sgn_vtbl *vtbl;
	const char *name;
	struct sgn_base *parent;
	struct tzarray_p_t kids;
	tzm4 T, I;	/* local. */
	tzm4 to, from;  /* world. */
};

#define sgn_base_name(_self)   ((_self)->name)
#define sgn_base_parent(_self) ((_self)->parent)
#define sgn_base_T(_self)      ((_self)->T)
#define sgn_base_I(_self)      ((_self)->I)
#define sgn_base_to(_self)     ((_self)->to)
#define sgn_base_from(_self)   ((_self)->from)
#define sgn_base_vtbl(_self)   ((_self)->vtbl)

void             sgn_base_init   (struct sgn_base *self, struct sgn_vtbl *vtbl);
void             sgn_base_setname(struct sgn_base *self, const char *name);
const char      *sgn_base_getname(struct sgn_base *self);
bool             sgn_isbase      (struct sgn_base *self);
struct sgn_vtbl *sgn_base_getvtbl(struct sgn_base *self);
void             sgn_base_setvtbl(struct sgn_base *self, struct sgn_vtbl *tbl);

struct sgn_base *sgn_translate(struct sgn_base *self,               tzv4 v);
struct sgn_base *sgn_rotate   (struct sgn_base *self, double theta, tzv4 v);
struct sgn_base *sgn_scale    (struct sgn_base *self,               tzv4 v);

/* base virtual methods */
void sgn_base_pre_draw (struct sgn_base *self, struct scene *scene);
void sgn_base_draw     (struct sgn_base *self, struct scene *scene);
void sgn_base_post_draw(struct sgn_base *self, struct scene *scene);
bool sgn_base_isvisible(struct sgn_base *self, struct scene *scene);
void sgn_base_addchild (struct sgn_base *self, struct sgn_base *kid);
void sgn_base_update   (struct sgn_base *self, uint16_t ms);

/* corresponding overload */
void sgn_pre_draw (struct sgn_base *base, struct scene *scene);
void sgn_draw     (struct sgn_base *base, struct scene *scene);
void sgn_post_draw(struct sgn_base *base, struct scene *scene);
bool sgn_isvisible(struct sgn_base *base, struct scene *scene);
void sgn_addchild (struct sgn_base *base, struct sgn_base *child);
void sgn_update   (struct sgn_base *base, uint16_t ms);

#endif /* SG_NODE_H */
