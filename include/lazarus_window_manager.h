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

#include "lazarus_file_reader.h"

#ifndef LAZARUS_WINDOW_MANAGER_H
#define LAZARUS_WINDOW_MANAGER_H

class FpsCounter
{
	public:
		FpsCounter();
        void monitorElapsedUptime();
        void monitorTimeDelta();
		void monitorFPS();
		
		float framesPerSecond;
		float timeDelta;
		float elapsedTime;
		
	private:
        GlobalsManager globals;

		float msSinceLastRender;
		float internalSeconds;
		int frameCount;		
};

class EventManager
{
    public:
    	//	TODO:
    	//	Create a constructor / destructor

        EventManager();

    	void eventsInit();
        void monitorEvents();

        string keyEventString;
        int keyEventCode;
		int keyEventOsCode;

		int mouseEventCode;
		int mousePositionX;
		int mousePositionY;
		
		int scrollEventCode;
		
    private:
        void updateKeyboardState();
        void updateMouseState();

        GLFWwindow *win;

		GlobalsManager globals;
};

class WindowManager : public EventManager, public FpsCounter
{
    public:
        WindowManager(const char *title, int width = 800, int height = 600);

        int createWindow();
		int loadConfig(GLuint shader);

        int open();
        int close();

		int createCursor(int sizeX, int sizeY, int hotX, int hotY, std::string filepath);
        int snapCursor(float moveX, float moveY);

        int presentNextFrame();

        bool isOpen;

        virtual ~WindowManager();
        
	private:
		int initialiseGLEW();
        int checkErrors(const char *file, int line);

        //  Dont know why I made this private
        struct Window
        {
            int height, width;
            const char *title;
        };

        std::unique_ptr<FileReader> fileReader;
        FileReader::Image image;

        Window frame;

        bool launchFullscreen;
        bool enableCursor;
        bool cullFaces;
        bool testDepth;
        bool disableVsync;

        int errorCode;
        const char** errorMessage;
        
        const GLFWvidmode *videoMode;
        GLFWmonitor *monitor;
        GLFWwindow *window;
        GLFWcursor *cursor;
        GLFWimage glfwImage;

        GlobalsManager globals;
};

#endif
