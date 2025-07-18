TARGET :=

LDFLAGS := -lGLEW -lglfw -lfmod -lfreetype

CXX = g++

CXXFLAGS = -std=c++17 -fPIC -Wall

BUILDSTEPS := 

# OS-specifics
ifeq ($(shell uname),Linux)
	CXX += -lstdc++fs
	CXXFLAGS += -shared
	LDFLAGS += -lGL 
	TARGET += liblazarus.so

	BUILDSTEPS += && sudo ldconfig
else ifeq ($(shell uname),Darwin)
	CXXFLAGS += -dynamiclib
	LDFLAGS += -framework OpenGL
	TARGET += liblazarus.dylib
endif

# Remove whitespace from "lib/ liblazarus.*"
OUT := $(addprefix lib/,$(TARGET))

OBJECTS = src/lazarus_common.o src/lazarus_shader.o src/lazarus_light.o src/lazarus_camera.o \
					src/lazarus_mesh.o src/lazarus_transforms.o src/lazarus_texture_loader.o \
					src/lazarus_window_manager.o src/lazarus_file_reader.o \
					src/lazarus_image_loader.o src/lazarus_image_resize.o src/lazarus_audio_manager.o \
					src/lazarus_text_manager.o src/lazarus_world_fx.o

lazarus_common.o := include/lazarus_common.h
lazarus_shader.o := include/lazarus_shader.h
lazarus_light.o := include/lazarus_light.h
lazarus_camera.o := include/lazarus_camera.h
lazarus_mesh.o := include/lazarus_mesh.h
lazarus_transforms.o := include/lazarus_transforms.h
lazarus_file_reader.o := include/lazarus_file_reader.h
lazarus_window_manager.o := include/lazarus_window_manager.h
lazarus_audio_manager.o := include/lazarus_audio_manager.h
lazarus_texture_loader.o := include/lazarus_texture.h
lazarus_image_loader.o := /usr/local/include/stb_image.h
lazarus_imageResize.o := /usr/local/include/stb_image_resize.h
lazarus_text_manager.o := include/lazarus_text_manager.h
lazarus_world_fx.o := include/lazarus_world_fx.h

run : build
	$(shell mv src/*.o build/)

debug : CXXFLAGS += -g
debug : run

optimise : CXXFLAGS += -O3
optimise : run

build : $(OBJECTS)
	$(shell mkdir build && mkdir lib)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(OBJECTS) $(LDFLAGS)

clean : 
	@echo "Destroying latest build files." && rm -R lib/ && rm -R build/

install : 
	@echo "installing..." && sudo cp include/* /usr/local/include && sudo cp $(OUT) /usr/local/lib $(BUILDSTEPS)

uninstall : 
	@echo "uninstalling..." && sudo rm /usr/local/include/lazarus_*.h && sudo rm $(addprefix /usr/local/,$(OUT))