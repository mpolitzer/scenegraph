#include <sg/scene.h>
#include <tzmath/tzmath.h>
#include <sg/gl/backend.h>

#include <sg/sgn_geom.h>
#include <sg/sgn_light.h>
#include <sg/sgn_cam.h>

static GLFWwindow *_window;
static struct sgn_geom _sphere32x32, _plane32, _cylinder32x32, _cone32x32;

static void geom_init(void);
static void gfx_init(void);
static void gfx_fini(void);
static void reshape(GLFWwindow *win, int w, int h);
static void sg_core_axes(void);

int main(int argc, char *argv[]) {
	struct scene S;
	struct sgn_geom sphere, plane, cone, cylinder;
	struct sgn_base *root = scene_getroot(&S);
	struct sgn_light lamp;
	struct sgn_cam   cam;

	gfx_init();
	geom_init();
	scene_init(&S);
	sgn_cam_init(&cam, "cam");

	glShadeModel(GL_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH); 
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#if 0
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
#endif
	sgn_geom_dup(&sphere,  &_sphere32x32);
	sgn_geom_dup(&plane,   &_plane32);
	sgn_geom_dup(&cylinder,&_cylinder32x32);

	sgn_light_init(&lamp, "lamp");

	sgn_addchild(root, &cylinder.base);
	sgn_addchild(root, &sphere.base);

	sgn_addchild(&plane.base, &cam.base);
	sgn_addchild(&cylinder.base, &lamp.base);
	sgn_addchild(&cylinder.base, &plane.base);

	sgn_translate(&lamp.base, tzv4_mkp(10, 0, 10));
	sgn_translate(&cam.base,  tzv4_mkp(1 , 0, 1 ));
	sgn_translate(&sphere.base, tzv4_mkp( 1.0, 0.0, 0.0));
	sgn_translate(&plane.base,  tzv4_mkp(0.0, 0.0, 1.0));

	scene_setcam(&S, &cam.base);

	while (!glfwWindowShouldClose(_window)) {
		double t0 = glfwGetTime();
		double s = sin(t0), c = cos(t0);

		tzm4_lookat(&sgn_base_I(&cam.base),
				tzv4_mkp(s, 1, c),
				tzv4_mkp(0, 0, 0),
				tzv4_mkp(0, 1, 0));
		scene_draw(&S);

		/* load back original V */
		glLoadMatrixf(sgn_base_to(&S.root).f);
		sg_core_axes();

		glfwSwapBuffers(_window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();
	}
	gfx_fini();
	return 0;
}

/* ========================================================================== */
static void geom_init(void) {
	sgn_geom_mksphere  (&_sphere32x32, "sphere", 32, 32);
	sgn_geom_mkplane   (&_plane32, "plane", 32);
	sgn_geom_mkcylinder(&_cylinder32x32, "cylinder", 32, 32);
	sgn_geom_mkcone    (&_cone32x32, "cone", 32, 32);
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
        glBegin(GL_LINES);
        glColor3f(1, 0, 0); glVertex3f(0, 0, 0);glVertex3f(1, 0, 0);
        glColor3f(0, 1, 0); glVertex3f(0, 0, 0);glVertex3f(0, 1, 0);
        glColor3f(0, 0, 1); glVertex3f(0, 0, 0);glVertex3f(0, 0, 1);
        glEnd();
}
