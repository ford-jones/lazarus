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

#include "../include/lazarus_window_manager.h"

Time::Time()
{
	this->frameCount 			= 0;
	this->framesPerSecond 		= 0;

	this->elapsedTime 			= 0.0;
	this->internalSeconds 		= 0.0;
	this->msSinceLastRender 	= 0.0;
	this->timeDelta 			= 0.0;
};

void Time::monitorElapsedUptime()
{
	this->elapsedTime = glfwGetTime();
	if(elapsedTime <= 0.0) globals.setExecutionState(LAZARUS_TIME_ERROR);

	this->msSinceLastRender = (this->elapsedTime - this->internalSeconds);
};

void Time::monitorTimeDelta()
{
	this->monitorFPS();

	this->timeDelta = (1000 / this->framesPerSecond);
	if(timeDelta <= 0.0) globals.setExecutionState(LAZARUS_TIME_ERROR);
};

void Time::monitorFPS()
{
	this->frameCount++;	
	this->monitorElapsedUptime();
	if(this->msSinceLastRender >= 1.0)
	{
		this->framesPerSecond = this->frameCount;
		if(this->framesPerSecond <= 0) globals.setExecutionState(LAZARUS_TIME_ERROR);

		this->frameCount = 0;
		this->internalSeconds += 1.0;
	};
};

/* =======================================
	TODO: 
    - Initialise image
    - Return window / make public
    - Handle resizing
    - Click location (see: glReadPixels())
========================================== */
Events::Events()
{
	keyEventString	    = "";

	keyEventCode 	    = 0;
	keyEventOsCode 		= 0;
	mouseEventCode 	    = LAZARUS_MOUSE_NOCLICK;
	mousePositionX 		= 0;
	mousePositionY 		= 0;
	scrollEventCode 	= 0;

	win 		        = NULL;
};

void Events::eventsInit()
{
	win = glfwGetCurrentContext();

	if(win != NULL)
	{
		glfwSetKeyCallback(win, [](GLFWwindow *win, int key, int scancode, int action, int mods){
			switch(action)
			{
				case GLFW_PRESS :
					LAZARUS_LISTENER_KEYCODE = key;
					LAZARUS_LISTENER_SCANCODE = scancode;
					break;
				case GLFW_RELEASE :
					LAZARUS_LISTENER_KEYCODE = 0;
					LAZARUS_LISTENER_SCANCODE = 0;
					break;
				default:
					break;
			};
			return;
		});

		glfwSetCursorPosCallback(win, [](GLFWwindow *win, double xpos, double ypos){
			LAZARUS_LISTENER_MOUSEX = xpos;
			LAZARUS_LISTENER_MOUSEY = ypos;
			return;
		});

		glfwSetMouseButtonCallback(win, [](GLFWwindow *win, int button, int action, int mods){
			switch(action)
			{
				case GLFW_PRESS :
					LAZARUS_LISTENER_MOUSECODE = button;
					break;
				case GLFW_RELEASE :
					LAZARUS_LISTENER_MOUSECODE = LAZARUS_MOUSE_NOCLICK;
					break;
				default :
					break;
			};
			return;
		});

		glfwSetScrollCallback(win, [](GLFWwindow *win, double xoffset, double yoffset){
			LAZARUS_LISTENER_SCROLLCODE = yoffset;
			return;
		});

	}
	else 
	{
		globals.setExecutionState(LAZARUS_NO_CONTEXT);
	};
};

void Events::monitorEvents()
{	
    glfwPollEvents();
	
    this->updateKeyboardState();
    this->updateMouseState();
};

void Events::updateKeyboardState()
{
	/* ==============================
		TODO: 
		Create cases and unique strings for remaining weird keys:
		- capslock 
		- pgup/down 
		- screenshot 
	================================= */
	
	this->keyEventString = "";
	this->keyEventCode = 0;
	this->keyEventOsCode = 0;
	
	this->keyEventCode = LAZARUS_LISTENER_KEYCODE;
	this->keyEventOsCode = LAZARUS_LISTENER_SCANCODE;
	
	if(keyEventCode > 0)
	{
		switch(keyEventCode)
		{
			case GLFW_KEY_UP :
				this->keyEventString = "up";
				break;
			case GLFW_KEY_DOWN :
				this->keyEventString = "down";
				break;
			case GLFW_KEY_LEFT :
				this->keyEventString = "left";
				break;
			case GLFW_KEY_RIGHT :
				this->keyEventString = "right";
				break;
			case GLFW_KEY_ENTER :
				this->keyEventString = "enter";
				break;
			case GLFW_KEY_SPACE :
				this->keyEventString = "space";
				break;
			case GLFW_KEY_TAB :
				this->keyEventString = "tab";
				break;
			case GLFW_KEY_LEFT_SHIFT :
				this->keyEventString = "shift-l";
				break;
			case GLFW_KEY_RIGHT_SHIFT :
				this->keyEventString = "shift-r";
				break;
			case GLFW_KEY_LEFT_CONTROL :
				this->keyEventString = "ctrl-l";
				break;
			case GLFW_KEY_RIGHT_CONTROL :
				this->keyEventString = "ctrl-r";
				break;
			/* ==================================
				TODO: 
				alt and super keys seem buggy?
				investigate.
			===================================== */
			
			case GLFW_KEY_LEFT_ALT :
				this->keyEventString = "alt-l";
				break;
			case GLFW_KEY_RIGHT_ALT :
				this->keyEventString = "alt-r";
				break;
			case GLFW_KEY_LEFT_SUPER :
				this->keyEventString = "fn-l";
				break;
			case GLFW_KEY_RIGHT_SUPER :
				this->keyEventString = "fn-r";
				break;
			default :
				this->keyEventString = glfwGetKeyName(keyEventCode, keyEventOsCode);
				break;
		};
	};
};

void Events::updateMouseState()
{
	this->mouseEventCode = LAZARUS_MOUSE_NOCLICK;
	this->mousePositionX = 0.0;
	this->mousePositionY = 0.0;
	
	this->mouseEventCode = LAZARUS_LISTENER_MOUSECODE;
	this->mousePositionX = static_cast<int>(LAZARUS_LISTENER_MOUSEX + 0.5);
	this->mousePositionY = static_cast<int>(LAZARUS_LISTENER_MOUSEY + 0.5);		
	
	/* =========================================================
		TODO: 
		Right now scroll can only be either 1 (up) || -1 (down)
		It will do for now, but should probably be changed to some sort of incrementing / decrementing number
		At the very least, it should be reset to 0 when the scrollwheel is not in motion
	============================================================ */
	this->scrollEventCode = static_cast<int>(LAZARUS_LISTENER_SCROLLCODE);
};

WindowManager::WindowManager(const char *title, int width, int height)
{
	std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
	this->errorCode = GLFW_NO_ERROR;
	this->errorMessage = NULL;
	
    this->frame.width = width;
    this->frame.height = height;
    this->frame.title = title;

    this->launchFullscreen = globals.getLaunchInFullscreen();
    this->enableCursor = globals.getCursorHidden();
    this->cullFaces = globals.getBackFaceCulling();
    this->testDepth = globals.getDepthTest();
    this->disableVsync = globals.getVsyncDisabled();
    /* ==================
        Optional
    ===================== */
    this->monitor = NULL;

    this->videoMode = NULL;
    this->cursor = NULL;

    this->isOpen = false;
};

int WindowManager::createWindow()
{
    if(!glfwInit())
    {
        std::cout << "ERROR::WINDOW_MANAGER::GLFW_MISSING" << std::endl;

        globals.setExecutionState(LAZARUS_GLFW_NOINIT);
        
        glfwTerminate();
        return -1;
    };

    /* ==================================================
      MacOS support:
      Can't use latest version of the OpenGL context (4.6).
      Based on what I've read online - Apple deprecated 
      support for OpenGL past version 4.1 in favour of their 
      Metal API which they want to encourage people to use.

      The only way I've been able to get it working is to 
      explicitly request this older version of the GL context
      with GLEW experimental features also turned on.
    ===================================================== */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->checkErrors(__FILE__, __LINE__);

    this->monitor = glfwGetPrimaryMonitor();
    this->videoMode = glfwGetVideoMode(this->monitor);
    
    if((this->frame.width > videoMode->width) || (this->frame.height > videoMode->height))
    {
        globals.setExecutionState(LAZARUS_WIN_EXCEEDS_MAX);
        return -1;
    };

    int targetWidth = 0;
    int targetHeight = 0;

    launchFullscreen
    ? (((targetWidth = videoMode->width) && (targetHeight = videoMode->height)) 
        &&  (
            this->window = glfwCreateWindow(
                videoMode->width, 
                videoMode->height, 
                this->frame.title, 
                this->monitor, 
                NULL
            ))
        )
    : ((targetWidth = this->frame.width) && (targetHeight = this->frame.height) 
        &&  (
            this->window = glfwCreateWindow(
                this->frame.width, 
                this->frame.height, 
                this->frame.title, 
                NULL, 
                NULL
            )
        )
    );

    globals.setDisplaySize(targetWidth, targetHeight);

    glfwSetWindowPos(this->window, floor((videoMode->width - this->frame.width) / 2), floor((videoMode->height - this->frame.height) / 2));

    glfwMakeContextCurrent(this->window);

    /* ========================================================================== 
        Note that alot of the GL ecosystem uses C-style callbacks. The repercussion
        being that pointers such as "this" cannot be used because of the required
        function signature being a static constant. 

        The solution here is to set the glfw user pointer value to "this" (i.e. 
        this class) and then perform a get on the user pointer inside of the actual 
        callback, which; is declared inline as a non-capturing lambda function.
    ============================================================================= */
    glfwSetWindowUserPointer(this->window, this);

    glfwSetWindowCloseCallback(this->window, [](GLFWwindow *win){
        WindowManager *window = (WindowManager *) glfwGetWindowUserPointer(win);
        window->close();
        return;
    });

	this->initialiseGLEW();
    
    return GLFW_NO_ERROR;
};

int WindowManager::setBackgroundColor(float r, float g, float b)
{
	glClearColor(r, g, b, 1.0);
	this->checkErrors(__FILE__, __LINE__);

	this->frame.backgroundColor = glm::vec3(r, g, b);

	return GLFW_NO_ERROR;
};

int WindowManager::loadConfig(GLuint shader)
{	
	if(enableCursor == true)
	{
	    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	};
	
	if(cullFaces == true)
	{
		glEnable            (GL_CULL_FACE);
        glCullFace          (GL_BACK);
	};
	
	if(testDepth == true)
	{
	    glEnable            (GL_DEPTH_TEST);
	};

    if(disableVsync == true)
    {
        glfwSwapInterval(0);
    }
    else
    {
        /* ======================================
            Unless Vsync is disabled, go 
            frame-for-frame between rendering and
            processing.
        ========================================= */
        glfwSwapInterval(1);
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glFrontFace(GL_CCW);

	this->setBackgroundColor(0.0, 0.0, 0.0);

	glUseProgram(shader);	
	this->checkErrors(__FILE__, __LINE__);
	
	return GLFW_NO_ERROR;
};

int WindowManager::open()
{
    glfwSetWindowShouldClose(this->window, GLFW_FALSE);
    this->isOpen = true;

    return GLFW_NO_ERROR;
}

int WindowManager::close()
{
    glfwSetWindowShouldClose(this->window, GLFW_TRUE);
    this->isOpen = false;

    return GLFW_NO_ERROR;
}

int WindowManager::createCursor(int sizeX, int sizeY, int hotX, int hotY, std::string filepath)
{		
    fileReader = std::make_unique<FileReader>();
    this->image = fileReader->readFromImage(filepath);

	this->glfwImage.width = sizeX;
	this->glfwImage.height = sizeY;
	this->glfwImage.pixels = this->image.pixelData;
	
	this->cursor = glfwCreateCursor(&this->glfwImage, hotX, hotY);
	glfwSetCursor(this->window, this->cursor);
	
	this->checkErrors(__FILE__, __LINE__);
	
	return GLFW_NO_ERROR;
};

int WindowManager::snapCursor(float moveX, float moveY)
{
    if(moveX > globals.getDisplayWidth() || moveY > globals.getDisplayHeight())
    {
        globals.setExecutionState(LAZARUS_INVALID_COORDINATE);
    }
    else
    {
        glfwSetCursorPos(this->window, moveX, moveY);
        this->checkErrors(__FILE__, __LINE__);
    };

    return GLFW_NO_ERROR;
};


int WindowManager::presentNextFrame()
{
	glfwSwapBuffers(this->window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    this->checkErrors(__FILE__, __LINE__);

	return GLFW_NO_ERROR;
};

/* =========================================
	TODO:
	Consider using color buffer instead of 
	stencil buffer for this. Currently the 
	max number of entities is 255 due to
	only being able to store 8-bit numbers 
	in the stencil-depth buffer.
============================================ */
int WindowManager::monitorPixelOccupants()
{
	/* ==========================================
		Notifies MeshManager::drawMesh to draw 
		not only VBO contents but also to draw to
		the stencil buffer.
	============================================= */
	if(!globals.getManageStencilBuffer()) globals.setManageStencilBuffer(true);

	/* ==========================================
		Stop tests from last cycle.
	============================================= */
	glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

	/* ==========================================
		Begin the stencil-depth test for the 
		cycle. Fill the depth-buffer with 0's 
		which are only replaced when the test is
		passed. (I.e. The fragment(s) is actually 
		occupying some pixel(s) in screenspace.)
	============================================= */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearStencil(0x00);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	this->checkErrors(__FILE__, __LINE__);
	return GLFW_NO_ERROR;
};

int WindowManager::checkErrors(const char *file, int line)
{
    errorCode = glfwGetError(errorMessage); 
    if(errorCode != GLFW_NO_ERROR)
    {
        std::cerr << RED_TEXT << file << " (" << line << ")" << RESET_TEXT << std::endl;
        std::cerr << RED_TEXT << "ERROR::GLFW::WINDOW " << RESET_TEXT << errorMessage << std::endl;

        globals.setExecutionState(LAZARUS_WINDOW_ERROR);
        
        return errorCode;
    }
    else 
    {
    	return GLFW_NO_ERROR;
    }
};

int WindowManager::initialiseGLEW()
{
    glewExperimental = GL_TRUE;
    glewInit();

    return GLEW_NO_ERROR;
};

WindowManager::~WindowManager() 
{
    glfwDestroyWindow(this->window);

    if(this->cursor != NULL) 
    {
        glfwDestroyCursor(this->cursor);
    }

    glfwTerminate();

    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
};
