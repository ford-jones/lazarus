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
        lazarus_result monitorElapsedUptime();
        lazarus_result monitorTimeDelta();
		lazarus_result monitorFPS();
		
		float framesPerSecond;
		float timeDelta;
		float elapsedTime;

        virtual ~Time();
		
	private:
		float msSinceLastRender;
		float internalSeconds;
		float frameCount;		
};

class EventManager
{
    public:
        enum EventType
        {
            KEY_PRESS,
            CLICK,
            MOUSE_MOVE,
            SCROLL
        };

        struct Event
        {
            EventType type;
            
            int32_t key             = 0;
		    int32_t keyVariant      = 0;
		    int32_t click           = 0;
		    int32_t scroll          = 0;
		    int32_t mousePositionX  = 0;
		    int32_t mousePositionY  = 0;
        };

        EventManager();

    	lazarus_result eventsInit();
        lazarus_result monitorEvents();
        lazarus_result convertKeyName(int32_t key, int32_t scan, std::string &out);

        void getLatestKey(int32_t &outCode, int32_t &outScan);
        void getLatestMouseMove(int32_t &outX, int32_t &outY);
        void getLatestClick(int32_t &out);
        void getLatestScroll(int32_t &out);
        
        std::vector<Event> eventQueue;
            
        virtual ~EventManager();
		
    protected:
        void dispatchEvent(EventType variant, int32_t aValue, int32_t bValue);
        
    private:
        Event event;

        int32_t latestKeyState;
		int32_t latestScanState;
		int32_t latestClickState;
		int32_t latestMouseXState;
		int32_t latestMouseYState;
        int32_t latestScrollState;
        
        std::string keyName;

        lazarus_result checkErrors(const char *file, int line);

        int32_t errorCode;
        const char* errorMessage;
        
        GLFWwindow *win;
    };

class WindowManager : public EventManager, public Time
{
    public:
        WindowManager(const char *title, uint32_t width = 800, uint32_t height = 600);

        lazarus_result createWindow();
        lazarus_result setBackgroundColor(float r, float g, float b);
		lazarus_result loadConfig();
        lazarus_result toggleFullscreen();
        lazarus_result resize(uint32_t width, uint32_t height);
        lazarus_result open();
        lazarus_result close();

		lazarus_result createCursor(uint32_t sizeX, uint32_t sizeY, uint32_t targetX, uint32_t targetY, std::string filepath);
        lazarus_result snapCursor(float moveX, float moveY);

        lazarus_result presentNextFrame();
        lazarus_result monitorPixelOccupants();

        bool isOpen;

        virtual ~WindowManager();
        
	private:
        void initialiseGLEW();
        lazarus_result centerWindow();
        lazarus_result checkErrors(const char *file, int line);

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
};

#endif
