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
#include <set>

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
            KEY_DOWN,
            KEY_HOLD,
            KEY_UP,
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
        /**
         * used by glfw callback handlers to handle pushing incoming user window interactions into the event queue.
         */
        void dispatchEvent(EventType variant, int32_t aValue, int32_t bValue);
        
    private:
        Event event;
        
        /**
         * NOTE:
         * Used for tracking keypresses between frames and promoting them to "KEY_HOLD" 
         * while simultaniously deduplicating key events from the queue. 
         */
        std::set<int32_t> heldKeys;
        std::vector<Event> events;

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
        struct WindowConfig
        {
            uint32_t height = 600;
            uint32_t width = 800;
            const char *title;
            glm::vec3 backgroundColor = glm::vec3(1.0f, 1.0f, 1.0f);

            bool fullscreen = false;
            bool disableCursor = false;
            bool cullFaces = true;
            bool testDepth = true;
            bool disableVsync = false;
            bool wireframeMode = false;
            bool disableResize = false;
        };

        WindowManager();

        lazarus_result create(WindowConfig config);
        lazarus_result setBackgroundColor(float r, float g, float b);
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
        /**
         * Starts up the gl extension wrangler
         */
        void initialiseGLEW();

        /**
         * Moves the window to the center of the monitor.
         * NOTE:
         * This function causes issue with wayland graphical sessions
         */
        lazarus_result centerWindow();

        /**
         * Set up OpenGL state and apply user settings
         * (backface culling, depth-testing, vsync, winding-order etc)
         */
        lazarus_result loadConfig();

        /**
         * Queries GLFW error state
         */
        lazarus_result checkErrors(const char *file, int line);

        std::unique_ptr<FileLoader> fileReader;
        FileLoader::Image image;

        WindowConfig config;
        
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
