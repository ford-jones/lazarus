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
	LOG_DEBUG("Constructing Lazarus::WindowManager::Time");

	this->frameCount 			= 0;
	this->framesPerSecond 		= 0;

	this->elapsedTime 			= 0.0f;
	this->internalSeconds 		= 0.0f;
	this->msSinceLastRender 	= 0.0f;
	this->timeDelta 			= 0.0f;
};

void Time::monitorElapsedUptime()
{
	this->elapsedTime = glfwGetTime();
	if(elapsedTime <= 0.0f)
	{
		LOG_ERROR("Time Error: ", __FILE__, __LINE__);
 		globals.setExecutionState(StatusCode::LAZARUS_TIME_ERROR);
	};

	this->msSinceLastRender = (this->elapsedTime - this->internalSeconds);

	return;
};

void Time::monitorTimeDelta()
{
	this->monitorFPS();

	this->timeDelta = (1000 / this->framesPerSecond);
	if(timeDelta <= 0.0f)
	{
		LOG_ERROR("Time Error: ", __FILE__, __LINE__);
		globals.setExecutionState(StatusCode::LAZARUS_TIME_ERROR);
	};

	return;
};

void Time::monitorFPS()
{
	this->frameCount++;	
	this->monitorElapsedUptime();

	if(this->msSinceLastRender >= 1.0f)
	{
		this->framesPerSecond = this->frameCount;
		if(this->framesPerSecond <= 0) 
		{
			LOG_ERROR("Time Error: ", __FILE__, __LINE__);
			globals.setExecutionState(StatusCode::LAZARUS_TIME_ERROR);
		};

		this->frameCount = 0;
		this->internalSeconds += 1.0f;
	};

	return;
};

Time::~Time()
{
	LOG_DEBUG("Destroying Lazarus::WindowManager::Time");
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
	LOG_DEBUG("Constructing Lazarus::WindowManager::Events");

	keyEventString	    = "";

	keyEventCode 	    = 0;
	keyEventOsCode 		= 0;
	mouseEventCode 	    = LAZARUS_MOUSE_NOCLICK;
	mousePositionX 		= 0;
	mousePositionY 		= 0;
	scrollEventCode 	= 0;

	errorCode 			= 0;
	errorMessage 		= NULL;
	win 		        = NULL;
};

void Events::eventsInit()
{
	win = glfwGetCurrentContext();

	this->checkErrors(__FILE__, __LINE__);
	
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

		this->checkErrors(__FILE__, __LINE__);
	}
	else 
	{
        LOG_ERROR("GLFW Error: No OpenGL context.", __FILE__, __LINE__);

		globals.setExecutionState(StatusCode::LAZARUS_NO_CONTEXT);

		return;
	};
};

void Events::monitorEvents()
{	
    glfwPollEvents();
	
    this->updateKeyboardState();
    this->updateMouseState();

	this->checkErrors(__FILE__, __LINE__);
	
	return;
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

	return;
};

void Events::updateMouseState()
{
	this->mouseEventCode = LAZARUS_MOUSE_NOCLICK;
	this->mousePositionX = 0.0f;
	this->mousePositionY = 0.0f;
	
	this->mouseEventCode = LAZARUS_LISTENER_MOUSECODE;
	this->mousePositionX = static_cast<int>(LAZARUS_LISTENER_MOUSEX + 0.5f);
	this->mousePositionY = static_cast<int>(LAZARUS_LISTENER_MOUSEY + 0.5f);		
	
	/* =========================================================
		TODO: 
		Right now scroll can only be either 1 (up) || -1 (down)
		It will do for now, but should probably be changed to some sort of incrementing / decrementing number
		At the very least, it should be reset to 0 when the scrollwheel is not in motion
	============================================================ */
	this->scrollEventCode = static_cast<int>(LAZARUS_LISTENER_SCROLLCODE);

	return;
};

int32_t Events::checkErrors(const char *file, int line)
{
    errorCode = glfwGetError(&errorMessage); 
    if(errorCode != GLFW_NO_ERROR)
    {
		std::string message = std::string("Event Error: ").append(errorMessage);
        LOG_ERROR(message.c_str(), file, line);

        globals.setExecutionState(StatusCode::LAZARUS_EVENT_ERROR);
        
        return errorCode;
    }
    else 
    {
    	return GLFW_NO_ERROR;
    }
};

Events::~Events()
{
	LOG_DEBUG("Destroying Lazarus::WindowManager::Events");
};

WindowManager::WindowManager(const char *title, uint32_t width, uint32_t height)
{
	LOG_DEBUG("Constructing Lazarus::WindowManager");

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

int32_t WindowManager::createWindow()
{
    if(!glfwInit())
    {
        LOG_ERROR("GLFW Error: GLFW missing.", __FILE__, __LINE__);

        globals.setExecutionState(StatusCode::LAZARUS_GLFW_NOINIT);
        
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
    
    if(
		(static_cast<int32_t>(this->frame.width) > videoMode->width) || 
		(static_cast<int32_t>(this->frame.height) > videoMode->height)
	)
    {
		LOG_ERROR("GLFW Error: ", __FILE__, __LINE__);

        globals.setExecutionState(StatusCode::LAZARUS_WIN_EXCEEDS_MAX);
        return -1;
    };

    int32_t targetWidth = 0;
    int32_t targetHeight = 0;

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

	int32_t windowLocationX = floor((videoMode->width - this->frame.width) / 2);
	int32_t windowLocationY = floor((videoMode->height - this->frame.height) / 2);
    glfwSetWindowPos(this->window, windowLocationX, windowLocationY);

    glfwMakeContextCurrent(this->window);

	this->checkErrors(__FILE__, __LINE__);
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

	glfwSetFramebufferSizeCallback(this->window, [](GLFWwindow *win, int width, int height){
		WindowManager *window = (WindowManager *) glfwGetWindowUserPointer(win);
		window->resize(width, height);
		//	TODO:
		//	Redraw on resize
		//	Create new member function which can be retrieved from user pointer
		return;	 
	});

	this->initialiseGLEW();
    
    return this->checkErrors(__FILE__, __LINE__);;
};

int32_t WindowManager::setBackgroundColor(float r, float g, float b)
{
	glClearColor(r, g, b, 1.0);

	this->frame.backgroundColor = glm::vec3(r, g, b);

	return this->checkErrors(__FILE__, __LINE__);;
};

int32_t WindowManager::loadConfig()
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
	this->checkErrors(__FILE__, __LINE__);

	this->setBackgroundColor(0.0, 0.0, 0.0);
	
	return this->checkErrors(__FILE__, __LINE__);;
};

int32_t WindowManager::resize(uint32_t width, uint32_t height)
{
	if((width < this->videoMode->width) && (height < this->videoMode->height))
	{
		this->frame.height = height;
		this->frame.width = width;
		globals.setDisplaySize(width, height);
		
		glViewport(0, 0, this->frame.width, this->frame.height);
	}
	else
	{
		LOG_ERROR("Window Error: ", __FILE__, __LINE__);
		globals.setExecutionState(LAZARUS_WIN_EXCEEDS_MAX);
	}

	return this->checkErrors(__FILE__, __LINE__);
};

int32_t WindowManager::open()
{
    glfwSetWindowShouldClose(this->window, GLFW_FALSE);
    this->isOpen = true;

    return this->checkErrors(__FILE__, __LINE__);;
}

int32_t WindowManager::close()
{
    glfwSetWindowShouldClose(this->window, GLFW_TRUE);
    this->isOpen = false;

    return this->checkErrors(__FILE__, __LINE__);;
}

int32_t WindowManager::createCursor(uint32_t sizeX, uint32_t sizeY, uint32_t targetX, uint32_t targetY, std::string filepath)
{		
    fileReader = std::make_unique<FileLoader>();
    this->image = fileReader->loadImage(filepath.c_str());

	this->glfwImage.width = sizeX;
	this->glfwImage.height = sizeY;
	this->glfwImage.pixels = this->image.pixelData;
	
	this->cursor = glfwCreateCursor(&this->glfwImage, targetX, targetY);
	glfwSetCursor(this->window, this->cursor);
	
	return this->checkErrors(__FILE__, __LINE__);
};

int32_t WindowManager::snapCursor(float moveX, float moveY)
{
    if(moveX > globals.getDisplayWidth() || moveY > globals.getDisplayHeight())
    {
		LOG_ERROR("GLFW Error:", __FILE__, __LINE__);
		
        globals.setExecutionState(StatusCode::LAZARUS_INVALID_COORDINATE);
		return -1;
    }
    else
    {
        glfwSetCursorPos(this->window, moveX, moveY);
        return this->checkErrors(__FILE__, __LINE__);
    };
};


int32_t WindowManager::presentNextFrame()
{
	glfwSwapBuffers(this->window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    return this->checkErrors(__FILE__, __LINE__);
};

/* =========================================
	TODO:
	Consider using color buffer instead of 
	stencil buffer for this. Currently the 
	max number of entities is 255 due to
	only being able to store 8-bit numbers 
	in the stencil-depth buffer.
============================================ */
int32_t WindowManager::monitorPixelOccupants()
{
	/* ==========================================
		Notifies MeshManager::drawMesh to draw 
		not only VBO contents but also to draw to
		the stencil buffer.
	============================================= */
	if(!globals.getManageStencilBuffer()) 
	{
		globals.setManageStencilBuffer(true);
	};

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

	return this->checkErrors(__FILE__, __LINE__);
};

int32_t WindowManager::checkErrors(const char *file, int line)
{
    errorCode = glfwGetError(&errorMessage); 
    if(errorCode != GLFW_NO_ERROR)
    {
        std::string message = std::string("Window Error: ").append(errorMessage);
        LOG_ERROR(message.c_str(), file, line);

        globals.setExecutionState(StatusCode::LAZARUS_WINDOW_ERROR);
        
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
	LOG_DEBUG("Destroying Lazarus::WindowManager");

    glfwDestroyWindow(this->window);

    if(this->cursor != NULL) 
    {
        glfwDestroyCursor(this->cursor);
    }

    glfwTerminate();
};
