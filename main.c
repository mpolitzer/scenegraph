#include <stdlib.h>

#include <sg/scene.h>
#include <tzmath/tzmath.h>
#include <sg/gl/backend.h>

#include <sg/cache.h>
#include <sg/geometry.h>
#include <sg/msh.h>
#include <sg/material.h>
#include <sg/texture.h>

#include <sg/sgn_geom.h>
#include <sg/sgn_light.h>
#include <sg/sgn_cam.h>
#include <sg/sgn_bb.h>

#include <sg/cam_ctl.h>

#include <sg/ds/tzarray_p.h>
#include <sg/ds/tzarray_p.inl>

#include <SOIL.h>
#define DATA_DIR "data/"

static struct scene S;
static struct cam_ctl _ctl = { .r = 10, .phi = 1, .theta = M_PI/2 };
static GLFWwindow *_window;
struct tzarray_p_t _nodes;
struct sgn_geom   *_jeep, *_sphere;
struct sgn_light  *_light;
static bool _draw_volumes = false;

static void gfx_init(void);
static void gfx_fini(void);
static void reshape(GLFWwindow *win, int w, int h);
void sg_core_plane(void);

struct jeep {
	float v;
	float theta, /* wheel angle */
	      omega; /* wheel speed */
};

struct jeep jeep = {
	.v     = 10,
	.theta = 0.10,
	.omega = 0.01,
};

void mouse_callback(GLFWwindow *window, double x, double y);

static void cache_populate(void) {
	cache_init();
#define NEW_G malloc(sizeof(struct geometry))
	/* carrega jeep */
	cache_put("/geom/bunny", geometry_loadmsh(NEW_G, "data/bunny.msh"));
	cache_put("/geom/alavancas", geometry_loadmsh(NEW_G, "data/Alavancas.msh"));
	cache_put("/geom/banco", geometry_loadmsh(NEW_G, "data/Banco.msh"));
	cache_put("/geom/calota", geometry_loadmsh(NEW_G, "data/Calota.msh"));
	cache_put("/geom/farol", geometry_loadmsh(NEW_G, "data/Farol.msh"));
	cache_put("/geom/lanternas", geometry_loadmsh(NEW_G, "data/Lanternas.msh"));
	cache_put("/geom/limpador", geometry_loadmsh(NEW_G, "data/Limpador.msh"));
	cache_put("/geom/painel", geometry_loadmsh(NEW_G, "data/Painel.msh"));
	cache_put("/geom/parabrisa", geometry_loadmsh(NEW_G, "data/Parabrisa.msh"));
	cache_put("/geom/roda", geometry_loadmsh(NEW_G, "data/Roda.msh"));
	cache_put("/geom/volante", geometry_loadmsh(NEW_G, "data/Volante.msh"));
	cache_put("/geom/carroceria", geometry_loadmsh(NEW_G, "data/carroceria.msh"));


	cache_put("/geom/sphere",   geometry_mksphere  (NEW_G, 32, 32));
	cache_put("/geom/plane",    geometry_mkplane   (NEW_G, 64));
	cache_put("/geom/cylinder", geometry_mkcylinder(NEW_G, 32, 32));
	cache_put("/geom/cone",     geometry_mkcone    (NEW_G, 32, 32));
#undef NEW_G
#define NEW_M malloc(sizeof(struct material))
	cache_put("/mat/default", material_default(NEW_M));
	cache_put("/mat/white", material_init(NEW_M,
				tzv4_mkp(0.60,   0.60,   0.60),
				tzv4_mkp(0.60,   0.60,   0.60),
				tzv4_mkp(0.60,   0.60,   0.60),
				tzv4_mkp(0.00,   0.00,   0.00),
				25.6));
	cache_put("/mat/copper", material_init(NEW_M,
				tzv4_mkp(0.21,   0.13,   0.05),
				tzv4_mkp(0.71,   0.43,   0.18),
				tzv4_mkp(0.39,   0.27,   0.17),
				tzv4_mkp(0.00,   0.00,   0.00),
				25.6));
	cache_put("/mat/bplastic", material_init(NEW_M,
				tzv4_mkp(0.00,   0.00,   0.00),
				tzv4_mkp(0.00,   0.00,   0.00),
				tzv4_mkp(0.50,   0.50,   0.50),
				tzv4_mkp(0.00,   0.00,   0.00),
				32.0));
	cache_put("/mat/emerald", material_init(NEW_M,
				tzv4_mkp(0.02150, 0.17450,  0.02150),
				tzv4_mkp(0.07568, 0.61424,  0.07568),
				tzv4_mkp(0.63300, 0.727811, 0.63300),
				tzv4_mkp(0.00,   0.00,   0.00),
				76.8));
	/* jeep materials */
	cache_put("/mat/lanternas", material_init(NEW_M,
			tzv4_mkp(1.000000,0.000000,0.000000),
			tzv4_mkp(1.000000,0.000000,0.000000),
			tzv4_mkp(1.000000,1.000000,1.000000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.300000));

	cache_put("/mat/alavanvas", material_init(NEW_M,
			tzv4_mkp(0.133300,0.133300,0.133300),
			tzv4_mkp(0.133300,0.133300,0.133300),
			tzv4_mkp(1.000000,1.000000,1.000000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.220000));

	cache_put("/mat/roda", material_init(NEW_M,
			tzv4_mkp(0.133300,0.133300,0.133300),
			tzv4_mkp(0.133300,0.133300,0.133300),
			tzv4_mkp(1.000000,1.000000,1.000000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.220000));

	cache_put("/mat/limpador", material_init(NEW_M,
			tzv4_mkp(0.133300,0.133300,0.133300),
			tzv4_mkp(0.133300,0.133300,0.133300),
			tzv4_mkp(1.000000,1.000000,1.000000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.220000));

	cache_put("/mat/volante", material_init(NEW_M,
			tzv4_mkp(0.133300,0.133300,0.133300),
			tzv4_mkp(0.133300,0.133300,0.133300),
			tzv4_mkp(1.000000,1.000000,1.000000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.220000));

	cache_put("/mat/farol", material_init(NEW_M,
			tzv4_mkp(0.098000,0.098000,0.098000),
			tzv4_mkp(0.098000,0.098000,0.098000),
			tzv4_mkp(0.900000,0.900000,0.900000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.820000));
	/* Opacity=0.3 */

	cache_put("/mat/painel", material_init(NEW_M,
			tzv4_mkp(0.262700,0.262700,0.262700),
			tzv4_mkp(0.262700,0.262700,0.262700),
			tzv4_mkp(1.000000,1.000000,1.000000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.180000));

	cache_put("/mat/banco", material_init(NEW_M,
			tzv4_mkp(0.376500,0.388200,0.345100),
			tzv4_mkp(0.376500,0.388200,0.345100),
			tzv4_mkp(0.900000,0.900000,0.900000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.100000));

	cache_put("/mat/carroceria", material_init(NEW_M,
			tzv4_mkp(0.521600,0.505900,0.325500),
			tzv4_mkp(0.521600,0.505900,0.325500),
			tzv4_mkp(0.900000,0.900000,0.900000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.310000));

	cache_put("/mat/calota", material_init(NEW_M,
			tzv4_mkp(0.619600,0.619600,0.619600),
			tzv4_mkp(0.619600,0.619600,0.619600),
			tzv4_mkp(0.900000,0.900000,0.900000),
			tzv4_mkp(0.00,   0.00,   0.00),
			0.310000));
#undef NEW_M
#define NEW_T malloc(sizeof(struct texture))
	cache_put("/tex/grass", texture_init(NEW_T,
				GL_TEXTURE_2D,
				DATA_DIR "ground_grass_1024_tile.jpg"));
	cache_put("/tex/earth", texture_init(NEW_T,
				GL_TEXTURE_2D,
				DATA_DIR "earth.png"));
	cache_put("/tex/city", texture_init(NEW_T,
				GL_TEXTURE_2D,
				DATA_DIR "city1_1.png"));
	cache_put("/tex/bathroom", texture_init(NEW_T,
				GL_TEXTURE_2D,
				DATA_DIR "bathroom.jpg"));
	cache_put("/tex/cloud", texture_lum_init(NEW_T,
				GL_TEXTURE_2D,
				DATA_DIR "cloud.png"));
	cache_put("/tex/flare", texture_lum_init(NEW_T,
				GL_TEXTURE_2D,
				DATA_DIR "flare.png"));
#undef NEW_T
}

static void key_callback(GLFWwindow* window,
		int key, int scancode,
		int action, int mods) {
	static int focus=0;
	static bool fog = false;

	bool action_mask = (action == GLFW_PRESS || action == GLFW_REPEAT);

	if (key == GLFW_KEY_V && action_mask) {
		_draw_volumes = !_draw_volumes;
		printf("draw volumes %d\n", _draw_volumes);
	}
	if (key == GLFW_KEY_ESCAPE && action_mask)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_H && action_mask)     sgn_translate(&_sphere->base, tzv4_mkp(   0,   0, 0.1));
	if (key == GLFW_KEY_L && action_mask)     sgn_translate(&_sphere->base, tzv4_mkp(   0,   0,-0.1));
	if (key == GLFW_KEY_UP && action_mask)    sgn_translate(&_sphere->base, tzv4_mkp( 0.1,   0,   0));
	if (key == GLFW_KEY_DOWN && action_mask)  sgn_translate(&_sphere->base, tzv4_mkp(-0.1,   0,   0));
	if (key == GLFW_KEY_LEFT && action_mask)  sgn_translate(&_sphere->base, tzv4_mkp(   0, 0.1,   0));
	if (key == GLFW_KEY_RIGHT && action_mask) sgn_translate(&_sphere->base, tzv4_mkp(   0, -0.1,  0));
	if (key == GLFW_KEY_MINUS && action_mask) {
		_ctl.r += 0.1;
		if (_ctl.r <= 0) _ctl.r = 0.1;
		cam_ctl_update((struct sgn_cam *)S.active_cam, &_ctl);
	}
	if (key == GLFW_KEY_EQUAL && action_mask) {
		_ctl.r -= 0.1;
		cam_ctl_update((struct sgn_cam *)S.active_cam, &_ctl);
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		static int mode=0;
		glPolygonMode(GL_FRONT_AND_BACK,
				(mode==0) ? GL_POINT :
				(mode==1) ? GL_LINE  : GL_FILL);
		mode = (mode+1) % 3;
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		S.env.filter = (S.env.filter+1) % 3;
		if (fog) scene_env_fog_enable(&S.env);
		printf("fog filter %d\n", S.env.filter);
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		fog = !fog;
		printf("fog %d\n", fog);
		if (fog) scene_env_fog_enable(&S.env);
		else     scene_env_fog_disable(&S.env);
	}
	if (key == GLFW_KEY_H && action_mask) {
		S.env.end += 1;
		if (fog) scene_env_fog_enable(&S.env);
		printf("end: %f\n", S.env.end);
	}
	else if (key == GLFW_KEY_U && action_mask) {
		S.env.end -= 1;
		if (fog) scene_env_fog_enable(&S.env);
		printf("end: %f\n", S.env.end);
	}
	if (key == GLFW_KEY_W && action_mask) { jeep.omega += 0.001; }
	if (key == GLFW_KEY_S && action_mask) { jeep.omega -= 0.001; }
	if (key == GLFW_KEY_A && action_mask) { jeep.theta -= 0.01; }
	if (key == GLFW_KEY_D && action_mask) { jeep.theta += 0.01; }
	printf("jeep theta: %f\n", jeep.theta);
	if (key == GLFW_KEY_G && action_mask) {
		S.env.start += 1;
		if (fog) scene_env_fog_enable(&S.env);
		printf("start: %f\n", S.env.start);
	}
	else if (key == GLFW_KEY_Y && action_mask) {
		S.env.start -= 1;
		if (fog) scene_env_fog_enable(&S.env);
		printf("start: %f\n", S.env.start);
	}
	if (key == GLFW_KEY_R && action_mask) {
		tzm4_mkrows(sgn_base_I(S.active_cam),
				tzv4_mk( 1, 0, 0, 0),
				tzv4_mk( 0, 0, 1, 0),
				tzv4_mk( 0,-1, 0, 0),
				tzv4_mk( 0, 0, 0, 1));
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		focus = (focus+1) % tzarray_p_n(&_nodes);
		sgn_cam_attach( (struct sgn_cam *)S.active_cam,
				(struct sgn_base *)tzarray_p_getp(&_nodes)[focus]);
	}
}

void mouse_callback(GLFWwindow *window, double x, double y) {
	static float _x, _y;
	float dx = x - _x,
	      dy = y - _y;
	_x = x, _y = y;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		_ctl.phi   += -M_PI*dy/w;
		_ctl.theta += 2*M_PI*dx/h;
		cam_ctl_update((struct sgn_cam *)S.active_cam, &_ctl);
	}
}

void scroll_callback(GLFWwindow *window, double xoff, double yoff) {
	_ctl.r += 0.1*yoff;
	if (_ctl.r <= 0) _ctl.r = 0.1;
	cam_ctl_update((struct sgn_cam *)S.active_cam, &_ctl);
}

void button_callback(GLFWwindow *window, int button, int action, int mods) {
	printf("%d %d %d\n", button, action, mods);
}

static void update_body(struct sgn_geom *self) {
	sgn_rotate((struct sgn_base *)self, jeep.v*jeep.omega*jeep.theta,
			tzv4_mkp(0, 0, 1));
	sgn_translate((struct sgn_base *)self, tzv4_mkp(jeep.v*jeep.omega, 0, 0));
}

static void update_wheels(struct sgn_geom *self) {
	/* rotate around (0,0,0), this will most likely accumulate error over time... */
	sgn_translate((struct sgn_base *)self, tzv4_mkp(2.5, -0.644, 0.438));
	sgn_rotate((struct sgn_base *)self, -jeep.v*jeep.omega, tzv4_mkp(0, 1, 0));
	sgn_translate((struct sgn_base *)self, tzv4_minus(tzv4_mkp(2.5, -0.644, 0.438)));
}

static void update_fwheels(struct sgn_geom *self) {
	/* rotate around (0,0,0), this will most likely accumulate error over time... */
	sgn_translate((struct sgn_base *)self, tzv4_mkp(2.5, -0.644, 0.438));
	sgn_rotate((struct sgn_base *)self, -jeep.v*jeep.omega, tzv4_mkp(0, 1, 0));
	sgn_translate((struct sgn_base *)self, tzv4_minus(tzv4_mkp(2.5, -0.644, 0.438)));
}

struct sgn_geom *mkjeep(void) {
	struct sgn_geom *root, *t, *t1;
	struct sgn_light *l0, *l1;

	sgn_geom_init(root = malloc(sizeof(*root)), "carroceria",
			cache_get("/geom/carroceria"),
			cache_get("/mat/carroceria"), NULL);
	sgn_rotate((struct sgn_base *)root, M_PI/2, tzv4_mkp(1, 0, 0));
	sgn_translate((struct sgn_base *)root, tzv4_mkp(0, 8, 0));
	root->update = update_body;
	sgn_geom_init(t = malloc(sizeof(*root)), "bunny",
			cache_get("/geom/bunny"),
			cache_get("/mat/emerald"), NULL);
	sgn_geom_scale(t, tzv4_mkp(10, 10, 10));
	sgn_translate((struct sgn_base *)t, tzv4_mkp(0, 0, 1));
	sgn_addchild((struct sgn_base *)root, (struct sgn_base *)t);

	/* farois */
	sgn_geom_init(t = malloc(sizeof(*t)), "farol0",
			cache_get("/geom/farol"),
			cache_get("/mat/farol"), NULL);
	sgn_addchild((struct sgn_base *)root, (struct sgn_base *)t);
	sgn_geom_init(t1 = malloc(sizeof(*t1)), "farol1",
			cache_get("/geom/farol"),
			cache_get("/mat/farol"), NULL);
	sgn_translate((struct sgn_base *)t1, tzv4_mkp(0, 1.028, 0));
	sgn_addchild((struct sgn_base *)t, (struct sgn_base *)t1);

	sgn_light_init(l0 = malloc(sizeof(*l0)), "l0");
	l0->light.spot_cutoff = 50;
	l0->light.spot_exponent = 5;
	sgn_translate((struct sgn_base *)l0, tzv4_mkp(2.956, 0.514, 1.074));
	sgn_rotate((struct sgn_base *)l0, M_PI/2, tzv4_mkp(0, 0, 1));
	sgn_addchild((struct sgn_base *)t, (struct sgn_base *)l0);

	sgn_light_init(l1 = malloc(sizeof(*l1)), "l1");
	l1->light.spot_cutoff = 50;
	l1->light.spot_exponent = 5;
	sgn_translate((struct sgn_base *)l1, tzv4_mkp(1.028, 0, 0));
	sgn_addchild((struct sgn_base *)l0, (struct sgn_base *)l1);

	/* roda0 */
	sgn_geom_init(t = malloc(sizeof(*t)), "roda_f0",
			cache_get("/geom/roda"),
			cache_get("/mat/roda"), NULL);
	sgn_addchild((struct sgn_base *)root, (struct sgn_base *)t);
	sgn_geom_init(t1 = malloc(sizeof(*t1)), "calota_f0",
			cache_get("/geom/calota"),
			cache_get("/mat/calota"), NULL);
	sgn_addchild((struct sgn_base *)t, (struct sgn_base *)t1);
	t->update = update_wheels;

	/* roda1 */
	sgn_geom_init(t = malloc(sizeof(*t)), "roda_f1",
			cache_get("/geom/roda"),
			cache_get("/mat/roda"), NULL);
	sgn_translate((struct sgn_base *)t, tzv4_mkp(0, 1.288 + 0.197, 0));
	sgn_addchild((struct sgn_base *)root, (struct sgn_base *)t);
	sgn_geom_init(t1 = malloc(sizeof(*t1)), "calota_f0",
			cache_get("/geom/calota"),
			cache_get("/mat/calota"), NULL);
	sgn_translate((struct sgn_base *)t1, tzv4_mkp(0, 0.2, 0));
	sgn_addchild((struct sgn_base *)t, (struct sgn_base *)t1);
	t->update = update_fwheels;
	/* roda2 */
	sgn_geom_init(t = malloc(sizeof(*t)), "roda_r0",
			cache_get("/geom/roda"),
			cache_get("/mat/roda"), NULL);
	sgn_translate((struct sgn_base *)t, tzv4_mkp(-2.5, 0, 0));
	sgn_addchild((struct sgn_base *)root, (struct sgn_base *)t);
	sgn_geom_init(t1 = malloc(sizeof(*t1)), "calota_f0",
			cache_get("/geom/calota"),
			cache_get("/mat/calota"), NULL);
	sgn_addchild((struct sgn_base *)t, (struct sgn_base *)t1);
	t->update = update_wheels;
	/* roda3 */
	sgn_geom_init(t = malloc(sizeof(*t)), "roda_r1",
			cache_get("/geom/roda"),
			cache_get("/mat/roda"), NULL);
	sgn_translate((struct sgn_base *)t, tzv4_mkp(-2.5, 1.288 + 0.197, 0));
	sgn_addchild((struct sgn_base *)root, (struct sgn_base *)t);
	sgn_geom_init(t1 = malloc(sizeof(*t1)), "calota_f0",
			cache_get("/geom/calota"),
			cache_get("/mat/calota"), NULL);
	sgn_translate((struct sgn_base *)t1, tzv4_mkp(0, 0.2, 0));
	sgn_addchild((struct sgn_base *)t, (struct sgn_base *)t1);
	t->update = update_wheels;

	/* volante */
	sgn_geom_init(t = malloc(sizeof(*t)), "volante",
			cache_get("/geom/volante"),
			cache_get("/mat/volante"), NULL);
	sgn_addchild((struct sgn_base *)root, (struct sgn_base *)t);

	return root;
}

void mkscene(struct scene *S) {
	struct sgn_base *root;
	struct sgn_geom *floor, *wall0, *wall1, *jeep;
	struct sgn_light *l, *spot;
	struct sgn_cam  *cam;
	struct sgn_bb   *bb;

	cache_populate();
	scene_init(S);
	tzarray_p_init(&_nodes); /* objs for cam to look at. */
	sgn_cam_init(cam = malloc(sizeof(*cam)), "cam");
	scene_setcam(S, &cam->base);
	root = scene_getroot(S);

	/* floor */
	sgn_geom_init(floor = malloc(sizeof(*floor)), "floor",
			cache_get("/geom/plane"),
			cache_get("/mat/white"),
			cache_get("/tex/bathroom"));
	sgn_geom_scale(floor,  tzv4_mkp(30, 1, 30));
	sgn_addchild(root, &floor->base);

	sgn_bb_init(bb = malloc(sizeof(*bb)), "flare",
			cache_get("/geom/plane"),
			cache_get("/mat/white"),
			cache_get("/tex/flare"));
	sgn_geom_scale(bb,  tzv4_mkp(3, 3, 3));
	sgn_translate(&bb->base, tzv4_mkp(9, 9, 9));
	sgn_addchild(&floor->base, &bb->base);

	/* wall0 */
	sgn_geom_init(wall0 = malloc(sizeof(*wall0)), "wall0",
			cache_get("/geom/plane"),
			cache_get("/mat/copper"),
			NULL);
	sgn_translate(&wall0->base, tzv4_mkp(0, 15, 15));
	sgn_rotate(&wall0->base, M_PI/2, tzv4_mkp(1, 0, 0));
	sgn_geom_scale(wall0,  tzv4_mkp(30, 1, 30));
	sgn_addchild(&floor->base, &wall0->base);

	/* wall1 */
	sgn_geom_init(wall1 = malloc(sizeof(*wall1)), "wall1",
			cache_get("/geom/plane"),
			cache_get("/mat/copper"),
			NULL);
	sgn_translate(&wall1->base, tzv4_mkp(15, 15, 0));
	sgn_rotate(&wall1->base,-M_PI/2, tzv4_mkp(0, 0, 1));
	sgn_geom_scale(wall1,  tzv4_mkp(30, 1, 30));
	sgn_addchild(&floor->base, &wall1->base);

	/* light */
	sgn_light_init(_light = l = malloc(sizeof(*l)), "global");
	sgn_translate(&l->base, tzv4_mkp(1, 1, 1));
	sgn_addchild(root, &l->base);

	/* spot */
	sgn_light_init(spot = malloc(sizeof(*spot)), "global");
	spot->light.spot_cutoff = 50;
	spot->light.spot_exponent = 5;
	sgn_translate(&spot->base, tzv4_mkv(-1, 5, 0));
	sgn_rotate(&spot->base, M_PI, tzv4_mkp(1, 0, 0));
	sgn_addchild(root, &spot->base);

	/* jeep */
	sgn_addchild(root, (struct sgn_base *)(_jeep = jeep = mkjeep()));

	/* notes to iterate lookat */
	tzarray_p_pushv(&_nodes, &jeep->base);
	tzarray_p_pushv(&_nodes, root);

	sgn_cam_attach( (struct sgn_cam *)cam,
			(struct sgn_base *)tzarray_p_getp(&_nodes)[0]);
}

void sgn_sm_from(struct sgn_geom *to, struct sgn_geom *from) {
	sgn_base_init(&to->base, NULL);
	to->geom = malloc(sizeof(struct geometry));
	to->geom->ibo = from->geom->ibo;
	to->geom->n   = from->geom->n;
	to->geom->type= from->geom->type;

	glGenBuffers(1, &to->geom->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, to->geom->vbo);
	glBufferData(GL_ARRAY_BUFFER,
			(to->geom->n) * sizeof(struct vertex_332),
			NULL,
			GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void sgn_sm_update(struct sgn_geom *to, struct sgn_geom *from, tzv4 _lpos) {
	/* put light in local coordinates */
	tzv4 lpos = tzm4_mulv(&from->base.from, _lpos);
	struct vertex_332 *t_verts, *f_verts;
	int i;

	glBindBuffer(GL_ARRAY_BUFFER, to->geom->vbo);
	t_verts = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, from->geom->vbo);
	f_verts = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

	for (i=0; i<from->geom->n; i++) {
		tzv4 f_pos    = tzv4_mkv3(f_verts[i].pos);
		tzv4 f_normal = tzv4_mkv3(f_verts[i].normal);
		tzv4 l_dir    = tzv4_sub(f_pos, lpos);

		if (tzv4_dot(l_dir, f_normal) <= 0) { /* front-face */
			t_verts[i].pos = f_verts[i].pos;
		} else {
			t_verts[i].pos = tzv4_to3(tzv4_add(f_pos, tzv4_muls(l_dir, 50)));
		}
	}

	/* bind(from) */
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, to->geom->vbo);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main(int argc, char *argv[]) {
	struct sgn_geom sphere, sphere_vol;
	gfx_init();

	/* make it look prety */
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_POLYGON_SMOOTH); 
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#if 0
	glEnable(GL_COLOR_MATERIAL);
#endif

	glfwSetKeyCallback(_window, key_callback);
	glfwSetCursorPosCallback(_window, mouse_callback);
	glfwSetMouseButtonCallback(_window, button_callback);
	glfwSetScrollCallback(_window, scroll_callback);

	mkscene(&S);
	sgn_geom_init(&sphere, "sphere",
			cache_get("/geom/sphere"),
			NULL, NULL);
	sgn_sm_from(&sphere_vol, &sphere);
	_sphere = &sphere;

	sgn_translate(&sphere.base, tzv4_mkp(5, 1, 0));
	sgn_rotate(&sphere.base, M_PI/2, tzv4_mkp(1, 0, 0));
	sgn_addchild(&S.root, &sphere.base);
	sgn_addchild(&sphere.base, &sphere_vol.base);
	key_callback(_window, 0, 0, 0, 0);
	while (!glfwWindowShouldClose(_window)) {
		tzm4 tmp;
		tzm4 *cam = sgn_base_to(S.active_cam);
		scene_update(&S, 1000/60);

		/* grab light pos in global coords */
		sgn_sm_update(&sphere_vol, &sphere, _light->base.to.c._3);
		tzm4_mulm(&tmp, cam, &sphere_vol.base.to);

		/* render objects that will be in shadow. */
		glPushAttrib(     GL_COLOR_BUFFER_BIT
				| GL_DEPTH_BUFFER_BIT
				| GL_ENABLE_BIT
				| GL_POLYGON_BIT
				| GL_STENCIL_BUFFER_BIT);

		/* casted on who? */
		glDepthMask(GL_TRUE);
		scene_draw(&S); /* everyone */
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glEnable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);

		/* setup for stencil ops. */
		glDepthMask(GL_FALSE);
		glEnable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);

		/* front */
		glCullFace(GL_FRONT);
		glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFFL);
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		geometry_draw(sphere_vol.geom, &tmp);
		/* back */
		glCullFace(GL_BACK);
		glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFFL);
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		geometry_draw(sphere_vol.geom, &tmp);

		/* restore to draw with shadows */
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_CULL_FACE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glStencilFunc(GL_NOTEQUAL, 0, 0xFFFFFFFFL);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glEnable(GL_BLEND);
		glColor4f(0, 0, 0, 0.7);
		geometry_draw(sphere_vol.geom, &tmp);
		scene_draw(&S);
		glPopAttrib();

		glDisable(GL_STENCIL_TEST);
		scene_draw(&S);

		glfwSwapBuffers(_window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glfwPollEvents();
	}
	gfx_fini();
	return 0;
}

/* ========================================================================== */

static void gfx_init(void) {
	if (!glfwInit()) {
		assert(0 && "glfw failed to start.");
		exit(EXIT_FAILURE);
	}
	_window = glfwCreateWindow(800, 600, "scene graph", NULL, NULL);
	if (!_window) {
		glfwTerminate();
		assert(0 && "glfw failed to create a window.");
		exit(EXIT_FAILURE);
	}
	glfwSetWindowSizeCallback(_window, reshape);
	glfwMakeContextCurrent(_window);
	glewInit();
}

static void gfx_fini(void) {
	glfwTerminate();
}

static void reshape(GLFWwindow *win, int w, int h) {
	tzm4 P;
	printf("reshape to: (%d, %d)\n", w, h);
	glViewport(0, 0, w, h);
	tzm4_perspective(&P, 90., w/(float)h, 0.1, 100);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(P.f);
	glMatrixMode(GL_MODELVIEW);
}

void sg_core_axes_in(tzm4 *to) {
	glPushMatrix();
	glLoadMatrixf(to->f);
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0);glVertex3f(1, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0);glVertex3f(0, 1, 0);
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0);glVertex3f(0, 0, 1);
	glEnd();
	glPopAttrib();
	glPopMatrix();
}

void sg_core_plane(void) {
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
	glTexCoord2d(1.0,0.0); glVertex2d(1.0,0.0);
	glTexCoord2d(1.0,1.0); glVertex2d(1.0,1.0);
	glTexCoord2d(0.0,1.0); glVertex2d(0.0,1.0);
	glEnd();
	glPopAttrib();
}
