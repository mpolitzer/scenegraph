#include <stdlib.h>

#include <sg/scene.h>
#include <tzmath/tzmath.h>
#include <sg/gl/backend.h>

#include <sg/cache.h>
#include <sg/geometry.h>
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

static struct scene S;
static struct cam_ctl _ctl = { .r = 1, .phi = 1, .theta = M_PI/2 };
static GLFWwindow *_window;
struct tzarray_p_t _nodes;
struct sgn_base *_node;
struct sgn_base *_lbase, *_ldome, *_ball;

static void gfx_init(void);
static void gfx_fini(void);
static void reshape(GLFWwindow *win, int w, int h);
static void sg_core_axes(void);
void sg_core_plane(void);

void mouse_callback(GLFWwindow *window, double x, double y);

static void cache_populate(void) {
	cache_init();
#define NEW_G malloc(sizeof(struct geometry))
	cache_put("/geom/sphere",   geometry_mksphere  (NEW_G, 64, 64));
	cache_put("/geom/plane",    geometry_mkplane   (NEW_G, 128));
	cache_put("/geom/cylinder", geometry_mkcylinder(NEW_G, 64, 64));
	cache_put("/geom/cone",     geometry_mkcone    (NEW_G, 64, 64));
#undef NEW_G
#define NEW_M malloc(sizeof(struct material))
	cache_put("/mat/default", material_default(NEW_M));
	cache_put("/mat/white", material_init(NEW_M,
				tzv4_mkp(0.60, 0.60, 0.60),
				tzv4_mkp(0.60, 0.60, 0.60),
				tzv4_mkp(0.60, 0.60, 0.60),
				tzv4_mkp(0.00, 0.00, 0.00),
				25.6));
	cache_put("/mat/copper", material_init(NEW_M,
				tzv4_mkp(0.21, 0.13, 0.05),
				tzv4_mkp(0.71, 0.43, 0.18),
				tzv4_mkp(0.39, 0.27, 0.17),
				tzv4_mkp(0.00, 0.00, 0.00),
				25.6));
	cache_put("/mat/bplastic", material_init(NEW_M,
				tzv4_mkp(0.00, 0.00, 0.00),
				tzv4_mkp(0.00, 0.00, 0.00),
				tzv4_mkp(0.50, 0.50, 0.50),
				tzv4_mkp(0.00, 0.00, 0.00),
				32.0));
#undef NEW_M
#define NEW_T malloc(sizeof(struct texture))
	cache_put("/tex/grass", texture_init(NEW_T,
				GL_TEXTURE_2D,
				"ground_grass_1024_tile.jpg"));
	cache_put("/tex/earth", texture_init(NEW_T,
				GL_TEXTURE_2D,
				"earth.png"));
	cache_put("/tex/city", texture_init(NEW_T,
				GL_TEXTURE_2D,
				"city1_1.png"));
	cache_put("/tex/cloud", texture_init(NEW_T,
				GL_TEXTURE_2D,
				"cloud.png"));
	cache_put("/tex/flare", texture_init(NEW_T,
				GL_TEXTURE_2D,
				"flare.png"));
#undef NEW_T
}

static void key_callback(GLFWwindow* window,
		int key, int scancode,
		int action, int mods) {
	static int focus=0;
	static bool fog = false;

	bool action_mask = (action == GLFW_PRESS || action == GLFW_REPEAT);

	if (key == GLFW_KEY_ESCAPE && action_mask)
		glfwSetWindowShouldClose(window, GL_TRUE);
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
	if (key == GLFW_KEY_UP && action_mask) {
		sgn_rotate(_ldome, 0.1, tzv4_mkp(1, 0, 0));
	}
	if (key == GLFW_KEY_DOWN && action_mask) {
		sgn_rotate(_ldome, 0.1, tzv4_mkp(-1, 0, 0));
	}
	if (key == GLFW_KEY_RIGHT && action_mask) {
		sgn_rotate(_lbase, 0.1, tzv4_mkp(0, 1, 0));
	}
	if (key == GLFW_KEY_LEFT && action_mask) {
		sgn_rotate(_lbase, 0.1, tzv4_mkp(0, -1, 0));
	}
	if (key == GLFW_KEY_R && action_mask) {
		tzm4_mkrows(&sgn_base_I(S.active_cam),
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

struct sgn_geom *mktable(void) {
	struct sgn_geom *ttop, *leg;
	/* table top */
	sgn_geom_init(ttop = malloc(sizeof(*ttop)), "floor",
			cache_get("/geom/plane"),
			cache_get("/mat/copper"),
			NULL);
	sgn_translate(&ttop->base, tzv4_mkp(0, 1, 0));
	/* leg0 */
	sgn_geom_init(leg = malloc(sizeof(*leg)), "leg0",
			cache_get("/geom/cylinder"),
			cache_get("/mat/copper"),
			NULL);
	sgn_geom_scale(leg,  tzv4_mkp(0.1, 1, 0.1));
	sgn_translate(&leg->base, tzv4_mkp(0.45, -0.5, 0.45));
	sgn_addchild(&ttop->base, &leg->base);

	/* leg1 */
	sgn_geom_init(leg = malloc(sizeof(*leg)), "leg1",
			cache_get("/geom/cylinder"),
			cache_get("/mat/copper"),
			NULL);
	tzm4_scale(&sgn_geom_localT(leg),  tzv4_mkp(0.1, 1, 0.1));
	sgn_translate(&leg->base, tzv4_mkp(-0.45, -0.5, 0.45));
	sgn_addchild(&ttop->base, &leg->base);

	/* leg2 */
	sgn_geom_init(leg = malloc(sizeof(*leg)), "leg2",
			cache_get("/geom/cylinder"),
			cache_get("/mat/copper"),
			NULL);
	tzm4_scale(&sgn_geom_localT(leg),  tzv4_mkp(0.1, 1, 0.1));
	sgn_translate(&leg->base, tzv4_mkp(-0.45, -0.5, -0.45));
	sgn_addchild(&ttop->base, &leg->base);

	/* leg3 */
	sgn_geom_init(leg = malloc(sizeof(*leg)), "leg3",
			cache_get("/geom/cylinder"),
			cache_get("/mat/copper"),
			NULL);
	tzm4_scale(&sgn_geom_localT(leg),  tzv4_mkp(0.1, 1, 0.1));
	sgn_translate(&leg->base, tzv4_mkp( 0.45, -0.5, -0.45));
	sgn_addchild(&ttop->base, &leg->base);

	return ttop;
}

struct sgn_geom *mkabajour(void) {
	struct sgn_geom *lbase, *lbasej, *lh0, *lj0, *lh1, *lj1, *ldome;
	struct sgn_light *lamp;
	/* abajour */
	sgn_geom_init(lbase = malloc(sizeof(*lbase)), "base",
			cache_get("/geom/cylinder"),
			cache_get("/mat/bplastic"),
			NULL);
	sgn_geom_scale(lbase, tzv4_mkp(0.1, 0.1, 0.1));
	sgn_translate(&lbase->base, tzv4_mkp(0, 0.05, 0));

	sgn_geom_init(lbasej = malloc(sizeof(*lbasej)), "basej",
			cache_get("/geom/sphere"),
			cache_get("/mat/bplastic"),
			NULL);
	sgn_geom_scale(lbasej, tzv4_mkp(0.1, 0.1, 0.1));
	sgn_translate(&lbasej->base, tzv4_mkp(0.0, 0.05, 0));
	sgn_addchild(&lbase->base, &lbasej->base);

	sgn_geom_init(lh0 = malloc(sizeof(*lh0)), "lh0",
			cache_get("/geom/cylinder"),
			cache_get("/mat/bplastic"),
			NULL);
	sgn_geom_scale(lh0, tzv4_mkp(0.06, 0.1, 0.06));
	sgn_geom_translate(lh0, tzv4_mkp(0.0, 0.5, 0));
	sgn_rotate(&lh0->base, M_PI/6, tzv4_mkp(1, 0, 0));
	sgn_addchild(&lbasej->base, &lh0->base);

	sgn_geom_init(lj0 = malloc(sizeof(*lbasej)), "lj0",
			cache_get("/geom/sphere"),
			cache_get("/mat/bplastic"),
			NULL);
	sgn_geom_scale(lj0, tzv4_mkp(0.1, 0.1, 0.1));
	sgn_translate(&lj0->base, tzv4_mkp(0.0, 0.1, 0));
	sgn_addchild(&lh0->base, &lj0->base);


	sgn_geom_init(lh1 = malloc(sizeof(*lh1)), "lh1",
			cache_get("/geom/cylinder"),
			cache_get("/mat/bplastic"),
			NULL);
	sgn_geom_scale(lh1, tzv4_mkp(0.06, 0.1, 0.06));
	sgn_geom_translate(lh1, tzv4_mkp(0.0, 0.5, 0));
	sgn_rotate(&lh1->base, M_PI/6, tzv4_mkp(1, 0, 0));
	sgn_addchild(&lj0->base, &lh1->base);

	sgn_geom_init(lj1 = malloc(sizeof(*lj1)), "lj1",
			cache_get("/geom/sphere"),
			cache_get("/mat/bplastic"),
			NULL);
	sgn_geom_scale(lj1, tzv4_mkp(0.1, 0.1, 0.1));
	sgn_translate(&lj1->base, tzv4_mkp(0.0, 0.1, 0));
	sgn_rotate(&lj1->base, M_PI/3, tzv4_mkp(1, 0, 0));
	sgn_addchild(&lh1->base, &lj1->base);

	sgn_geom_init(ldome = malloc(sizeof(*ldome)), "ldome",
			cache_get("/geom/cone"),
			cache_get("/mat/bplastic"),
			NULL);
	sgn_geom_scale(ldome, tzv4_mkp(0.1, 0.1, 0.1));
	sgn_translate(&ldome->base, tzv4_mkp(0.0, 0.05, 0));
	sgn_rotate(&ldome->base, M_PI/30, tzv4_mkp(1, 0, 0));
	sgn_addchild(&lj1->base, &ldome->base);

	tzarray_p_pushv(&_nodes, &ldome->base);
	_lbase = &lbase->base;
	_ldome = &lj1->base;

	sgn_light_init(lamp = malloc(sizeof(*lamp)), "llamp");
	lamp->light.diffuse = tzv4_mkp(0.8, 0.8, 0.8);
	lamp->light.spot_cutoff = 30;
	lamp->light.spot_exponent = 20;
	sgn_addchild(&ldome->base, &lamp->base);

	return lbase;
}

void mkscene(struct scene *S) {
	struct sgn_base *root;
	struct sgn_geom *ball, *ball1, *ttop, *floor, *wall0, *wall1, *abajour;
	struct sgn_geom *bb;
	struct sgn_light *l, *spot;
	struct sgn_cam  *cam;
	struct sgn_base *clouds;

	cache_populate();
	scene_init(S);
	tzarray_p_init(&_nodes);
	sgn_cam_init(cam = malloc(sizeof(*cam)), "cam");
	scene_setcam(S, &cam->base);
	root = scene_getroot(S);

	/* floor */
	sgn_geom_init(floor = malloc(sizeof(*floor)), "floor",
			cache_get("/geom/plane"),
			cache_get("/mat/copper"),
			NULL);
	tzm4_scale(&sgn_geom_localT(floor),  tzv4_mkp(30, 1, 30));
	sgn_addchild(root, &floor->base);

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

	/* table */
	sgn_addchild(&floor->base, &(ttop = mktable())->base);

	/* abajour */
	sgn_addchild(&ttop->base, &(abajour = mkabajour())->base);
	sgn_translate(&abajour->base, tzv4_mkp(0.35, 0, 0.35));

	/* ball */
	sgn_geom_init(ball = malloc(sizeof(*ball)), "ball",
			cache_get("/geom/sphere"),
			cache_get("/mat/white"),
			cache_get("/tex/earth"));
	sgn_geom_scale(ball, tzv4_mkp(0.1, 0.1, 0.1));
	sgn_translate(&ball->base, tzv4_mkp(0, 0.05, 0));
	sgn_addchild(&ttop->base, &ball->base);
	_ball = &ball->base;

	/* ball1 */
	sgn_geom_init(ball1 = malloc(sizeof(*ball1)), "ball",
			cache_get("/geom/sphere"),
			cache_get("/mat/copper"),
			NULL);
	sgn_geom_scale(ball1, tzv4_mkp(0.03, 0.03, 0.03));
	sgn_translate(&ball1->base, tzv4_mkp(0.2, 0.0, 0));
	sgn_addchild(&ball->base, &ball1->base);

	/* light */
	sgn_light_init(l = malloc(sizeof(*l)), "global");
	sgn_translate(&l->base, tzv4_mkp(1.5, 0.5, 0));
	sgn_addchild(&ttop->base, &l->base);

	/* spot */
	sgn_light_init(spot = malloc(sizeof(*spot)), "global");
	spot->light.spot_cutoff = 30;
	spot->light.spot_exponent = 30;
	sgn_translate(&spot->base, tzv4_mkv(-1, 5, 0));
	sgn_rotate(&spot->base, M_PI, tzv4_mkp(1, 0, 0));
	sgn_addchild(&ttop->base, &spot->base);

	/* group of clouds */
	sgn_base_init(clouds = malloc(sizeof(*clouds)), NULL);
	sgn_translate(clouds, tzv4_mkp( 4, 4, 0));
	sgn_addchild(&ttop->base, clouds);

	/* cloud0 */
	sgn_bb_init(bb = malloc(sizeof(*bb)), "bb",
			cache_get("/geom/plane"),
			cache_get("/mat/white"),
			cache_get("/tex/cloud"));
	sgn_translate(&bb->base, tzv4_mkp(-.5, 0, 0));
	sgn_geom_scale(bb, tzv4_mkp(1, 1, 3));
	sgn_addchild(clouds, &bb->base);

	/* cloud1 */
	sgn_bb_init(bb = malloc(sizeof(*bb)), "bb",
			cache_get("/geom/plane"),
			cache_get("/mat/white"),
			cache_get("/tex/cloud"));
	sgn_translate(&bb->base, tzv4_mkp(.5, 0.1, 0));
	sgn_geom_scale(bb, tzv4_mkp(1, 1, 3));
	sgn_addchild(clouds, &bb->base);

	/* notes to iterate lookat */
	tzarray_p_pushv(&_nodes, &bb->base);
	tzarray_p_pushv(&_nodes, &ball->base);
	tzarray_p_pushv(&_nodes, &l->base);
	tzarray_p_pushv(&_nodes, &floor->base);

	sgn_cam_attach( (struct sgn_cam *)cam,
			(struct sgn_base *)tzarray_p_getp(&_nodes)[0]);
}

void one_up_hack(void) {
#if 0
	tzm4 tmp, P, V;
	tzm4 *veI = &sgn_base_to(S.active_cam);
	tzm4 *vp  = &sgn_base_to(_ldome);

	struct geometry *geom = cache_get("/geom/plane");
	struct texture *tex   = cache_get("/tex/city");
	struct material *mat  = cache_get("/mat/white");

	tzm4_perspective(&P, 90., 800/(float)600, 0.1, 100);

	tzm4_mkiden(&tmp);
	glMatrixMode(GL_TEXTURE);
	tzm4_lookat(&V, tzv4_mkp(1, 1, 1),
			tzv4_mkp(0, 0, 0),
			tzv4_mkp(0, 1, 0));
	tzm4_mulm(&tmp, &tmp, veI);
	tzm4_mulm(&tmp, &V, &tmp);
	tzm4_mulm(&tmp, &P, &tmp);
	glLoadMatrixf(tmp.f);

	glMatrixMode(GL_MODELVIEW);
	texture_load(tex);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

#endif
}

int main(int argc, char *argv[]) {

	gfx_init();

	/* make it look prety */
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#if 0
	glEnable(GL_COLOR_MATERIAL);
#endif
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
#if 0
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
#else
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
#endif
	glfwSetKeyCallback(_window, key_callback);
	glfwSetCursorPosCallback(_window, mouse_callback);
	glfwSetMouseButtonCallback(_window, button_callback);
	glfwSetScrollCallback(_window, scroll_callback);

	mkscene(&S);
	key_callback(_window, 0, 0, 0, 0);
	while (!glfwWindowShouldClose(_window)) {
		one_up_hack();
		sgn_rotate(_ball, 0.01, tzv4_mkp(0, 1, 0));
		scene_draw(&S);
		glfwSwapBuffers(_window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

void sg_core_axes(void) {
	glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3f(1, 0, 0); glVertex3f(0, 0, 0);glVertex3f(1, 0, 0);
        glColor3f(0, 1, 0); glVertex3f(0, 0, 0);glVertex3f(0, 1, 0);
        glColor3f(0, 0, 1); glVertex3f(0, 0, 0);glVertex3f(0, 0, 1);
        glEnd();
	glEnable(GL_LIGHTING);
}

void sg_core_axes_in(tzm4 *to) {
	glPushMatrix();
	glLoadMatrixf(to->f);
	glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3f(1, 0, 0); glVertex3f(0, 0, 0);glVertex3f(1, 0, 0);
        glColor3f(0, 1, 0); glVertex3f(0, 0, 0);glVertex3f(0, 1, 0);
        glColor3f(0, 0, 1); glVertex3f(0, 0, 0);glVertex3f(0, 0, 1);
        glEnd();
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void sg_core_plane(void) {
	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glTexCoord2d(0.0,0.0); glVertex2d(0.0,0.0);
	glTexCoord2d(1.0,0.0); glVertex2d(1.0,0.0);
	glTexCoord2d(1.0,1.0); glVertex2d(1.0,1.0);
	glTexCoord2d(0.0,1.0); glVertex2d(0.0,1.0);
	glEnd();
	glEnable(GL_LIGHTING);
}
