TARGET :=

LDFLAGS := -lGLEW -lglfw -lfmod -lfreetype

CXX = g++

CXXFLAGS = -std=c++17 -fPIC -Wall

# OS-specifics
ifeq ($(shell uname),Linux)
	CXX += -lstdc++fs
	CXXFLAGS += -shared
	LDFLAGS += -lGL 
	TARGET += liblazarus.so
else ifeq ($(shell uname),Darwin)
	CXXFLAGS += -dynamiclib
	LDFLAGS += -framework OpenGL
	TARGET += liblazarus.dylib
endif

# Remove whitespace from "lib/ liblazarus.*"
OUT := $(addprefix lib/,$(TARGET))

OBJECTS = src/lz_globals_manager.o src/lz_shader.o src/lz_light.o src/lz_camera.o \
					src/lz_mesh.o src/lz_transforms.o \
					src/lz_mesh_loader.o src/lz_material_loader.o src/lz_texture_loader.o \
					src/lz_window_manager.o src/lz_file_reader.o src/lz_event_manager.o \
					src/lz_fps_counter.o src/lz_image_loader.o src/lz_image_resize.o src/lz_audio_manager.o \
					src/lz_text_manager.o src/lz_font_loader.o src/lz_world_fx.o src/lz_constants.o

run : build
	$(shell mv src/*.o build/)

build : $(OBJECTS)
	$(shell mkdir build && mkdir lib)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(OBJECTS) $(LDFLAGS)

# This is linux specific: 
# MacOS doesn't need to run ldconfig
# Windows uses \ Program Files \ instead of /usr/local/*
install : 
	sudo cp include/* /usr/local/include && sudo cp lib/liblazarus.so /usr/local/lib && sudo ldconfig

uninstall : 
	sudo rm /usr/local/include/lz*.h && sudo rm /usr/local/lib/liblazarus.so

lz_globals_manager.o := include/lz_globals_manager.h
lz_shader.o := include/lz_shader.h
lz_light.o := include/lz_light.h
lz_camera.o := include/lz_camera.h
lz_mesh.o := include/lz_mesh.h
lz_transforms.o := include/lz_transforms.h
lz_mesh_loader.o := include/lz_mesh_loader.h
lz_material_loader.o := include/lz_material_loader.h
lz_file_reader.o := include/lz_file_reader.h
lz_window_manager.o := include/lz_window_manager.h
lz_event_manager.o := include/lz_event_manager.h
lz_fps_counter.o := include/lz_fps_counter.h
lz_audio_manager.o := include/lz_audio_manager.h
lz_texture_loader.o := include/lz_texture.h
lz_image_loader.o := /usr/local/include/stb_image.h
lz_imageResize.o := /usr/local/include/stb_image_resize.h
lz_text_manager.o := include/lz_text_manager.h
lz_font_loader.o := include/lz_font_loader.h
lz_world_fx.o := include/lz_world_fx.h
lz_constants.o := include/lz_constants.h

clean : 
	rm -R lib/ && rm -R build/