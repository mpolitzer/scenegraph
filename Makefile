CPATH=-I. -I3rd/tzlib -I3rd/glfw/include/
#GL=shader.c shader.h util.c opengl.h
#DS=oco_string.c oco_string.h
#SRLA=srv4_sse.c srm4_sse.c
#GEOM=sphere.c sphere.h vertex.h
#RESOURCES=$(addprefix gfx/, ${GL})\
#	  $(addprefix cds/, ${DS})\
#	  $(addprefix la/, ${SRLA})\
#	  $(addprefix geom/, ${GEOM})\
#	  main.c

SG=scene.c  scene.h\
   sgn_base.c  sgn_base.h\
   sgn_light.c sgn_light.h\
   sgn_geom.c  sgn_geom.h\
   sgn_cam.c   sgn_cam.h
TZMATH= tzm4_sse.c tzv2_sse.c tzv3_sse.c tzv4_sse.c tzmath.h
# .c, .h, .S
RESOURCES=\
	  $(addprefix sg/, ${SG})\
	  $(addprefix 3rd/tzlib/tzmath/, ${TZMATH})\
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
