#include <stdlib.h>

#include <sg/scene.h>
#include <tzmath/tzmath.h>
#include <sg/gl/backend.h>

#include <sg/cache.h>
#include <sg/sgn_geom.h>
#include <sg/sgn_light.h>
#include <sg/sgn_cam.h>

#include <sg/ds/tzarray_p.h>
#include <sg/ds/tzarray_p.inl>

static struct scene S;
static GLFWwindow *_window;
struct tzarray_p_t _nodes;

static void gfx_init(void);
static void gfx_fini(void);
static void reshape(GLFWwindow *win, int w, int h);
static void sg_core_axes(void);

static void cache_populate(void) {
	cache_init();
#define NEW_G malloc(sizeof(struct geometry))
	cache_put("/geom/sphere",   geometry_mksphere  (NEW_G, 32, 32));
	cache_put("/geom/plane",    geometry_mkplane   (NEW_G, 32));
	cache_put("/geom/cylinder", geometry_mkcylinder(NEW_G, 32, 32));
	cache_put("/geom/cone",     geometry_mkcone    (NEW_G, 32, 32));
#undef NEW_G
#define NEW_M malloc(sizeof(struct material))
	cache_put("/mat/default", material_default(NEW_M));
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
}

static void key_callback(GLFWwindow* window,
		int key, int scancode,
		int action, int mods) {
	static float phi=1, theta=M_PI/2, r=2.0;
	static int focus=0;
	float sp, st, cp, ct;

	bool action_mask = (action == GLFW_PRESS || action == GLFW_REPEAT);

	if (key == GLFW_KEY_ESCAPE && action_mask)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_UP && action_mask)
		phi += 0.03;
	if (key == GLFW_KEY_DOWN && action_mask)
		phi -= 0.03;
	if (key == GLFW_KEY_LEFT && action_mask)
		theta += 0.03;
	if (key == GLFW_KEY_RIGHT && action_mask)
		theta -= 0.03;
	if (key == GLFW_KEY_Z && action_mask)
		r += 0.3;
	if (key == GLFW_KEY_A && action_mask)
		r -= 0.3;
	if (key == GLFW_KEY_R && action_mask) {
		tzm4_mkrows(&sgn_base_I(S.active_cam),
				tzv4_mk( 1, 0, 0, 0),
				tzv4_mk( 0, 0, 1, 0),
				tzv4_mk( 0,-1, 0, 0),
				tzv4_mk( 0, 0, 0, 1));
	} else {
		if (phi > M_PI) phi = M_PI-0.0001;
		if (phi <    0) phi =      0.0001;
		if (r   < 0.01) r   =      0.0001;
		printf("%f, %f, %f, %d\n", phi, theta, r, focus);

		sp = sin(phi),   cp = cos(phi);
		st = sin(theta), ct = cos(theta);
		tzm4_lookat(&sgn_base_I(S.active_cam),
				tzv4_mkp(r*ct*sp, r*cp, r*sp*st),
				tzv4_mkp(      0,    0,       0),
				tzv4_mkp(      0,    1,       0));
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		focus = (focus+1) % tzarray_p_n(&_nodes);
	}
	sgn_cam_attach( (struct sgn_cam *)S.active_cam,
			(struct sgn_base *)tzarray_p_getp(&_nodes)[focus]);
}

int main(int argc, char *argv[]) {
	struct sgn_geom sphere, plane, cone, cylinder;
	struct sgn_base *root = scene_getroot(&S);
	struct sgn_light lamp;
	struct sgn_light spot;
	struct sgn_cam   cam;

	gfx_init();
	cache_populate();
	scene_init(&S);
	sgn_cam_init(&cam, "cam");
	tzarray_p_init(&_nodes);
	tzarray_p_pushv(&_nodes, &cylinder);
	tzarray_p_pushv(&_nodes, &sphere);
	tzarray_p_pushv(&_nodes, &plane);
	tzarray_p_pushv(&_nodes, &cone);
	tzarray_p_pushv(&_nodes, &lamp);
	tzarray_p_pushv(&_nodes, &spot);

	glEnable (GL_BLEND);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH); 
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
#if 0
	glEnable(GL_COLOR_MATERIAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
#if 0
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
#endif
	sgn_geom_init(&sphere,   "sphere",
			cache_get("/geom/sphere"),
			cache_get("/mat/default"));
	sgn_geom_init(&plane,    "plane",
			cache_get("/geom/plane"),
			cache_get("/mat/default"));
	sgn_geom_init(&cylinder, "cylinder",
			cache_get("/geom/cylinder"),
			cache_get("/mat/default"));
	sgn_geom_init(&cone,     "cone",
			cache_get("/geom/cone"),
			cache_get("/mat/default"));

	sgn_light_init(&lamp, "lamp");
	sgn_light_init(&spot, "spot");

	sgn_addchild(root, &cylinder.base);
	sgn_addchild(root, &sphere.base);

	/* set lookat object */
	sgn_cam_attach(&cam, &plane.base);
	sgn_addchild(&cone.base, &spot.base);

	sgn_addchild(&cylinder.base, &lamp.base);
	sgn_addchild(&cylinder.base, &plane.base);
	sgn_addchild(&plane.base, &cone.base);

	sgn_translate(&cone.base, tzv4_mkp(0, 2, 0));
	sgn_rotate(&cone.base, M_PI, tzv4_mkp(1, 0, 0));

	spot.light.spot_cutoff = 20;

	sgn_translate(&lamp.base, tzv4_mkp(1, -2, 0));
	sgn_translate(&plane.base, tzv4_mkp(0, 1, 0));
	sgn_translate(&sphere.base, tzv4_mkp( 1.0, 0.0, 0.0));
	sgn_translate(&cone.base, tzv4_mkp(0.5, 1, 0));

	scene_setcam(&S, &cam.base);
	glfwSetKeyCallback(_window, key_callback);

	key_callback(_window, 0, 0, 0, 0);
	while (!glfwWindowShouldClose(_window)) {
		double t0 = glfwGetTime();
		double s = sin(t0), c = cos(t0);
#if 0
		tzm4_lookat(&sgn_base_I(&cam.base),
				tzv4_mkp(3*s, 3, 3*c),
				tzv4_mkp(0, 0, 0),
				tzv4_mkp(0, 1, 0));
#endif
		sgn_rotate(&cone.base, 0.001, tzv4_mkp(1*s, 0, 1*c));
		tzm4_inverse(&sgn_base_I(&cone.base), &sgn_base_T(&cone.base));
#if 0
		sgn_rotate(&sphere.base, 0.1, tzv4_mkp(1, 0, 0));
#endif
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
