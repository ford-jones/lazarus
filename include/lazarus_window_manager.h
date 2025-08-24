//              .,                                                                                                          .(*      ,*,                
//                ((.     (.    ,                                                                          */*  ,%##%%%%%%%%%%%%%%%#(*           .      
//         .//**/#%%%%%%%%%%%%%%#*   .,                                                             ,**   .(%%%%%%#(******,***,/#%%%%%%%%###(/,         
//             #%%%#*.#%/***,,..,*(%(.    ,,                                                     *     /#%##/*****,,,,,,,,,.,...,,#%,  .#%#.            
//    .,     *%&#/   %#**,,*..,....,.*#,     ..                                               *     ,%#%#/*,,*,*,,,,,.,.,,.,...,...((     /#(//*/**.    
//           (%#    *#*...,.,,..........*/      ,                                          .      *#%(#(**,,,,,,,,..,..,..,,........(.     *#(          
//           *#     *(......,.............(#      ,                                       .     ,((, ##,,,.....,.................. ./       **  .,.     
//            *     ./........ ...........*#*,                                          ,      ,(,  ./*,,,..,,.................  .. *                   
//                   /, ........    ... ../(  *.                                              ,*     /,...,,.,,.....   ............**                   
//                    *... .............  /    ,                                             *,      ,*,,............  ,....     ...                    
//                     *.   ..... .... ..*                                                  .*        *...................   .  ...                     
//               *       ... ......... ,.                                                   ,          ... ..........  ...     ..       ,               
//                ((        .,.,.. ...                                                                   .  . .. .  .  ... .  ..      //                
//              ,/(#%#*                                                                                     .....  ... ......       .#*                 
//                 /((##%#(*                                                                                      .......        ,(#(*,                 
//               (.           .,,,,,                                                                                        .*#%%(                      
//                                                                                                      .***,.   . .,/##%###(/.  ...,,.      
/*  LAZARUS ENGINE */
#ifndef LAZARUS_GL_INCLUDES_H
    #include "lazarus_gl_includes.h"
#endif

#ifndef LAZARUS_COMMON_H
	#include "lazarus_common.h"
#endif

#include <iostream>
#include <string>
#include <memory>

#include "lazarus_file_loader.h"

#ifndef LAZARUS_WINDOW_MANAGER_H
#define LAZARUS_WINDOW_MANAGER_H

class Time
{
	public:
		Time();
        void monitorElapsedUptime();
        void monitorTimeDelta();
		void monitorFPS();
		
		float framesPerSecond;
		float timeDelta;
		float elapsedTime;

        virtual ~Time();
		
	private:
        GlobalsManager globals;

		float msSinceLastRender;
		float internalSeconds;
		float frameCount;		
};

class Events
{
    public:
    	//	TODO:
    	//	Create a constructor / destructor
        //  Create a public getter for retreiving the members
        //  Make members private

        Events();

    	void eventsInit();
        void monitorEvents();

        std::string keyEventString;
        uint32_t keyEventCode;
		uint32_t keyEventOsCode;

		uint32_t mouseEventCode;
		uint32_t mousePositionX;
		uint32_t mousePositionY;
		
		int8_t scrollEventCode;

        virtual ~Events();
		
    private:
        int32_t checkErrors(const char *file, int line);
        void updateKeyboardState();
        void updateMouseState();

        int32_t errorCode;
        const char* errorMessage;

        GLFWwindow *win;

		GlobalsManager globals;
};

class WindowManager : public Events, public Time
{
    public:
        WindowManager(const char *title, uint32_t width = 800, uint32_t height = 600);

        int32_t createWindow();
        int32_t setBackgroundColor(float r, float g, float b);
		int32_t loadConfig();
        int32_t toggleFullscreen();
        int32_t resize(uint32_t width, uint32_t height);
        int32_t open();
        int32_t close();

		int32_t createCursor(uint32_t sizeX, uint32_t sizeY, uint32_t targetX, uint32_t targetY, std::string filepath);
        int32_t snapCursor(float moveX, float moveY);

        int32_t presentNextFrame();
        int32_t monitorPixelOccupants();

        bool isOpen;

        virtual ~WindowManager();
        
	private:
        int32_t centerWindow();
		int32_t initialiseGLEW();
        int32_t checkErrors(const char *file, int line);

        //  Dont know why I made this private
        struct Window
        {
            uint32_t height;
            uint32_t width;
            const char *title;
            glm::vec3 backgroundColor;
        };

        std::unique_ptr<FileLoader> fileReader;
        FileLoader::Image image;

        Window frame;

        bool isFullscreen;
        bool enableCursor;
        bool cullFaces;
        bool testDepth;
        bool disableVsync;

        int32_t originalWidth;
        int32_t originalHeight;

        int32_t errorCode;
        const char* errorMessage;
        
        const GLFWvidmode *videoMode;
        GLFWmonitor *monitor;
        GLFWwindow *window;
        GLFWcursor *cursor;
        GLFWimage glfwImage;

        GlobalsManager globals;
};

#endif
