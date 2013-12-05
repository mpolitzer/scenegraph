CPATH=-I.\
      -I3rd/tzlib\
      -I3rd/glfw/include/\
      -I3rd/SOIL/src/

SG=scene.c  scene.h\
   sgn_base.c  sgn_base.h\
   sgn_light.c sgn_light.h\
   sgn_geom.c  sgn_geom.h\
   sgn_cam.c   sgn_cam.h\
   sgn_bb.c    sgn_bb.h\
   sgn_proj.c  sgn_proj.h\
   geometry.c  geometry.h\
   material.c  material.h\
   texture.c   texture.h\
   light.c     light.h\
   cache.c     cache.h\
   cam_ctl.c   cam_ctl.h\
   scene_env.c scene_env.h\
   msh.c       msh.h
TZMATH= tzm4_sse.c\
	tzv2_sse.c\
	tzv3_sse.c\
	tzv4_sse.c\
	tzmath.h
SOIL=image_DXT.c     image_DXT.h\
     image_helper.c  image_helper.h\
     stb_image_aug.c stb_image_aug.h\
     SOIL.c          SOIL.h

# .c, .h, .S
RESOURCES=\
	  $(addprefix sg/, ${SG})\
	  $(addprefix 3rd/tzlib/tzmath/, ${TZMATH})\
	  $(addprefix 3rd/SOIL/src/, ${SOIL})\
	  main.c

SRC=$(filter %.c, ${RESOURCES})
HDR=$(filter %.h, ${RESOURCES})
OBJ=${SRC:.c=.o}

CFLAGS=-march=native -O2 -Wall -g $(CPATH)
LDFLAGS=-O2 -lm -L3rd/glfw/build/src/ -lglfw3
PKG=glfw3 glew gl
CFLAGS +=`pkg-config $(PKG) --cflags`
LDFLAGS+=`pkg-config $(PKG) --static --libs`
EXE=main

.PHONY: all clean
all: .depend.mk $(EXE)
zip: Makefile $(SRC) $(HDR)
	zip -r mypackage.zip $^

$(EXE): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $(EXE)
$(OBJ): %.o: %.c
.depend.mk: $(RESOURCES) Makefile
	$(CC) -MM $(CFLAGS) $(SRC) > .depend.mk
clean:
	rm -f $(EXE) $(OBJ)
-include .depend.mk
