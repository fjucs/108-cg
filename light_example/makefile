CXX=g++
FLAG=-std=c++14
CFLAGS = -lopengl32 -lglu32 -lfreeglut
INCLUDE=-I../3rd/imgui/ -I../3rd/freeglut/include
LIBS=-L../3rd/freeglut/lib

BUILD_DIR=./build/

EXE = light
SOURCES = main.cpp
# SOURCES += imgui_impl_glut.cpp imgui_impl_opengl2.cpp
# SOURCES += ../3rd/imgui/imgui.cpp ../3rd/imgui/imgui_demo.cpp ../3rd/imgui/imgui_draw.cpp ../3rd/imgui/imgui_widgets.cpp

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

%.o:%.cpp
	$(CXX) $(FLAG) $(INCLUDE) $(LIBS) $(CFLAGS) -c -o $@ $<

%.o:../3rd/imgui/%.cpp
	$(CXX) $(FLAG) $(INCLUDE) $(LIBS) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(FLAG) -o $@ $^ $(INCLUDE) $(LIBS) $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(EXE) $(OBJS)
