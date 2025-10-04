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

lazarus_result Time::monitorElapsedUptime()
{
	this->elapsedTime = glfwGetTime();
	if(elapsedTime <= 0.0f)
	{
		LOG_ERROR("Time Error: ", __FILE__, __LINE__);
 		return lazarus_result::LAZARUS_TIME_ERROR;
	};

	this->msSinceLastRender = (this->elapsedTime - this->internalSeconds);

	return lazarus_result::LAZARUS_OK;
};

lazarus_result Time::monitorTimeDelta()
{
	this->monitorFPS();

	this->timeDelta = (1000 / this->framesPerSecond);
	if(timeDelta <= 0.0f)
	{
		LOG_ERROR("Time Error: ", __FILE__, __LINE__);
		return lazarus_result::LAZARUS_TIME_ERROR;
	};

	return lazarus_result::LAZARUS_OK;
};

lazarus_result Time::monitorFPS()
{
	this->frameCount++;	
	this->monitorElapsedUptime();

	if(this->msSinceLastRender >= 1.0f)
	{
		this->framesPerSecond = this->frameCount;
		if(this->framesPerSecond <= 0) 
		{
			LOG_ERROR("Time Error: ", __FILE__, __LINE__);
			return lazarus_result::LAZARUS_TIME_ERROR;
		};

		this->frameCount = 0;
		this->internalSeconds += 1.0f;
	};

	return lazarus_result::LAZARUS_OK;
};

Time::~Time()
{
	LOG_DEBUG("Destroying Lazarus::WindowManager::Time");
};

Events::Events()
{
	LOG_DEBUG("Constructing Lazarus::WindowManager::Events");

	keyName	    		= "";
	keyCode 	    	= 0;
	scanCode 			= 0;

	clickState 	    = LAZARUS_MOUSE_NOCLICK;
	mousePositionX 		= 0;
	mousePositionY 		= 0;
	scrollState 		= 0;
	previousScroll		= 0;

	errorCode 			= 0;
	errorMessage 		= NULL;
	win 		        = NULL;
};

lazarus_result Events::eventsInit()
{
	win = glfwGetCurrentContext();

	lazarus_result status = this->checkErrors(__FILE__, __LINE__);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};
	
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
			/* =========================================================
				This one's quite silly. GLFW never clears the value of 
				its yoffset, so once you've scrolled once in either
				direction the scroll state is locked until changed to
				something else. And that something can't be 0 ever again
				so basically locked in infinite up or down state.

				To combat this, ensure we can never checkin the 
				same state twice in a row. Such that at the fastest rate
				of scroll possible, scroll and no-scroll states appear
				one for one.
			============================================================ */

			float previous = LAZARUS_LISTENER_SCROLLCODE;
			
			if(previous == yoffset)
			{
				LAZARUS_LISTENER_SCROLLCODE = 0;
			}		
			else
			{
				LAZARUS_LISTENER_SCROLLCODE = yoffset;	
			}

			return;
		});

		return this->checkErrors(__FILE__, __LINE__);
	}
	else 
	{
        LOG_ERROR("GLFW Error: No OpenGL context.", __FILE__, __LINE__);

		return lazarus_result::LAZARUS_NO_CONTEXT;
	};
};

lazarus_result Events::monitorEvents()
{	
    glfwPollEvents();
	
	/* ===================================
		Don't need to check these, they 
		are always OK as the cb's are 
		void.
	====================================== */

    this->updateKeyboardState();
    this->updateMouseState();

	return this->checkErrors(__FILE__, __LINE__);
};

void Events::updateKeyboardState()
{	
	this->keyName = "";
	this->keyCode = 0;
	this->scanCode = 0;
	
	this->keyCode = LAZARUS_LISTENER_KEYCODE;
	this->scanCode = LAZARUS_LISTENER_SCANCODE;

	/* =========================================
		Check the static global value as it 
		accounts for signedness, which is needed
		to check the name of GLFW_KEY_UNKNOWN
		with the scancode
	============================================ */
	switch(LAZARUS_LISTENER_KEYCODE)
	{
		case GLFW_KEY_UP :
			this->keyName = "up";
			break;
		case GLFW_KEY_DOWN :
			this->keyName = "down";
			break;
		case GLFW_KEY_LEFT :
			this->keyName = "left";
			break;
		case GLFW_KEY_RIGHT :
			this->keyName = "right";
			break;
		case GLFW_KEY_ENTER :
			this->keyName = "enter";
			break;
		case GLFW_KEY_SPACE :
			this->keyName = "space";
			break;
		case GLFW_KEY_TAB :
			this->keyName = "tab";
			break;
		case GLFW_KEY_LEFT_SHIFT :
			this->keyName = "shift-l";
			break;
		case GLFW_KEY_RIGHT_SHIFT :
			this->keyName = "shift-r";
			break;
		case GLFW_KEY_LEFT_CONTROL :
			this->keyName = "ctrl-l";
			break;
		case GLFW_KEY_RIGHT_CONTROL :
			this->keyName = "ctrl-r";
			break;
		/* ==================================
			TODO: 
			alt and super keys seem buggy?
			investigate.
		===================================== */
		
		case GLFW_KEY_LEFT_ALT :
			this->keyName = "alt-l";
			break;
		case GLFW_KEY_RIGHT_ALT :
			this->keyName = "alt-r";
			break;
		case GLFW_KEY_LEFT_SUPER :
			this->keyName = "fn-l";
			break;
		case GLFW_KEY_RIGHT_SUPER :
			this->keyName = "fn-r";
			break;
		case GLFW_KEY_BACKSPACE :
			this->keyName = "backspace";
			break;
		case GLFW_KEY_CAPS_LOCK :
			this->keyName = "capslock";
			break;
		case GLFW_KEY_ESCAPE :
			this->keyName = "esc";
			break;
		case GLFW_KEY_F1 :
			this->keyName = "f1";
			break;
		case GLFW_KEY_F2 :
			this->keyName = "f2";
			break;
		case GLFW_KEY_F3 :
			this->keyName = "f3";
			break;
		case GLFW_KEY_F4 :
			this->keyName = "f4";
			break;
		case GLFW_KEY_F5 :
			this->keyName = "f5";
			break;
		case GLFW_KEY_F6 :
			this->keyName = "f6";
			break;
		case GLFW_KEY_F7 :
			this->keyName = "f7";
			break;
		case GLFW_KEY_F8 :
			this->keyName = "f8";
			break;
		case GLFW_KEY_F9 :
			this->keyName = "f9";
			break;
		case GLFW_KEY_F10 :
			this->keyName = "f10";
			break;
		case GLFW_KEY_F11 :
			this->keyName = "f11";
			break;
		case GLFW_KEY_F12 :
			this->keyName = "f12";
			break;
		case GLFW_KEY_DELETE :
			this->keyName = "delete";
			break;
		case GLFW_KEY_HOME :
			this->keyName = "home";
			break;
		case GLFW_KEY_INSERT :
			this->keyName = "insert";
			break;
		case GLFW_KEY_END :
			this->keyName = "end";
			break;
		case GLFW_KEY_PAGE_UP :
			this->keyName = "pgup";
			break;
		case GLFW_KEY_PAGE_DOWN :
			this->keyName = "pgdn";
			break;
		case GLFW_KEY_UNKNOWN :
			/* ============================================
				Unkown key. Use the scancode (which is 
				specific to the hardware as opposed to the 
				ascii code).
			=============================================== */
			this->keyName = glfwGetKeyName(keyCode, scanCode);
			break;
		default :
			this->keyName = static_cast<char>(keyCode);
			break;
	};

	return;
};

void Events::updateMouseState()
{
	this->clickState 	 = LAZARUS_MOUSE_NOCLICK;
	this->mousePositionX = 0.0f;
	this->mousePositionY = 0.0f;
	
	this->clickState 	 = LAZARUS_LISTENER_MOUSECODE;
	
	/* ===================
		Ceil / round up
	====================== */

	this->mousePositionX = static_cast<int>(LAZARUS_LISTENER_MOUSEX + 0.5f);
	this->mousePositionY = static_cast<int>(LAZARUS_LISTENER_MOUSEY + 0.5f);		
	
	/* ========================================================
		Callbacks are only fired upon receiving an input signal
		from the system. As such, the scroll cb's major 
		shortcoming in eliminating persistent "on" states is
		that its possible outcomes still include an "on" state
		and the input signal could end right there. To fully 
		eliminate the possibility of infinite up / down state
		we need to check the previous state in our process as
		well as the glfw process.
	=========================================================== */

	if(previousScroll == LAZARUS_LISTENER_SCROLLCODE)
	{
		this->scrollState = 0;
	}		
	else
	{
		this->scrollState = LAZARUS_LISTENER_SCROLLCODE;	
	}
	
	this->previousScroll = LAZARUS_LISTENER_SCROLLCODE;

	return;
};

lazarus_result Events::checkErrors(const char *file, int line)
{
    errorCode = glfwGetError(&errorMessage); 
    if(errorCode != GLFW_NO_ERROR)
    {
		std::string message = std::string("Event Error: ").append(errorMessage);
        LOG_ERROR(message.c_str(), file, line);

        return lazarus_result::LAZARUS_EVENT_ERROR;
    }
    else 
    {
    	return lazarus_result::LAZARUS_OK;
    };
};

Events::~Events()
{
	LOG_DEBUG("Destroying Lazarus::WindowManager::Events");
};

WindowManager::WindowManager(const char *title, uint32_t width, uint32_t height)
	: WindowManager::Events(),
	  WindowManager::Time()
{
	LOG_DEBUG("Constructing Lazarus::WindowManager");

	this->errorCode = GLFW_NO_ERROR;
	this->errorMessage = NULL;
	
    this->frame.width = width;
    this->frame.height = height;
    this->frame.title = title;

    this->isFullscreen 	= GlobalsManager::getLaunchInFullscreen();
    this->enableCursor 	= GlobalsManager::getCursorHidden();
    this->cullFaces 	= GlobalsManager::getBackFaceCulling();
    this->testDepth 	= GlobalsManager::getDepthTest();
    this->disableVsync 	= GlobalsManager::getVsyncDisabled();

	this->isOpen = false;

	originalWidth = this->frame.width;
	originalHeight = this->frame.height;

    /* ==================
        Optional
    ===================== */
    this->monitor = NULL;

    this->videoMode = NULL;
    this->cursor = NULL;
};

lazarus_result WindowManager::createWindow()
{
	lazarus_result status = lazarus_result::LAZARUS_OK;

    if(!glfwInit())
    {
        glfwTerminate();
        LOG_ERROR("GLFW Error: GLFW missing.", __FILE__, __LINE__);

        status = lazarus_result::LAZARUS_GLFW_NOINIT;
		return status;
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

    status = this->checkErrors(__FILE__, __LINE__);

    this->monitor = glfwGetPrimaryMonitor();
    this->videoMode = glfwGetVideoMode(this->monitor);
    
    if(
		(static_cast<int32_t>(this->frame.width) > videoMode->width) || 
		(static_cast<int32_t>(this->frame.height) > videoMode->height)
	)
    {
		LOG_ERROR("GLFW Error: ", __FILE__, __LINE__);

        status = lazarus_result::LAZARUS_WIN_EXCEEDS_MAX;
        return status;
    };

    int32_t targetWidth = 0;
    int32_t targetHeight = 0;

    isFullscreen
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

    GlobalsManager::setDisplaySize(targetWidth, targetHeight);

	status = this->centerWindow();
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};

    glfwMakeContextCurrent(this->window);

	status = this->checkErrors(__FILE__, __LINE__);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};

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

		return;	 
	});

	this->initialiseGLEW();
    
    return this->checkErrors(__FILE__, __LINE__);;
};

lazarus_result WindowManager::setBackgroundColor(float r, float g, float b)
{
	glClearColor(r, g, b, 1.0);

	this->frame.backgroundColor = glm::vec3(r, g, b);

	return this->checkErrors(__FILE__, __LINE__);;
};

lazarus_result WindowManager::loadConfig()
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

	lazarus_result status = this->checkErrors(__FILE__, __LINE__);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};
	
	return this->setBackgroundColor(0.0, 0.0, 0.0);
};

lazarus_result WindowManager::toggleFullscreen()
{
	if(!this->isFullscreen)
	{
		/* =============================================
			Track original size so as to restore to
			previous state on next toggle. Note that
			usage of this->frame directly causes a
			bug as the values of this->frame are
			overwritten by WindowManager::resize(...) 
			when the callback is fired after this
			function is called.
		================================================ */

		this->originalWidth = this->frame.width;
		this->originalHeight = this->frame.height;

		glfwSetWindowMonitor(
			this->window, 
			this->monitor, 
			0, 0, 
			this->videoMode->width, 
			this->videoMode->height,
			this->videoMode->refreshRate
		);
		this->isFullscreen = true;
	}
	else
	{
		/* ====================================
			Frame width and height should be
			preserved from last time the screen
			was fullscreen. If it was never
			fullscreen before now, it will use

			the default or the last size 
			specified by the user.
		======================================= */

		this->frame.width = this->originalWidth;
		this->frame.height = this->originalHeight;

		glfwSetWindowMonitor(
			this->window, 
			NULL, 
			0, 0, 
			this->frame.width,
			this->frame.height, 
			0
		);

		lazarus_result status = this->centerWindow();
		if(status != lazarus_result::LAZARUS_OK)
		{
			return status;
		};

		this->isFullscreen = false;
	};

	return this->checkErrors(__FILE__, __LINE__);
};

lazarus_result WindowManager::resize(uint32_t width, uint32_t height)
{
	/* ===========================================
		Due to this being set as the glfw 
		framebuffer resize callback, this is in
		turn also fired following a call to 
		WindowManager::toggleFullscreen(), hence
		why setDisplaySize doesn't need to be 
		called there.
	============================================== */

	this->frame.height = height;
	this->frame.width = width;
	GlobalsManager::setDisplaySize(width, height);

	glViewport(0, 0, this->frame.width, this->frame.height);

	return this->checkErrors(__FILE__, __LINE__);
};

lazarus_result WindowManager::open()
{
    glfwSetWindowShouldClose(this->window, GLFW_FALSE);
    this->isOpen = true;

    return this->checkErrors(__FILE__, __LINE__);;
}

lazarus_result WindowManager::close()
{
    glfwSetWindowShouldClose(this->window, GLFW_TRUE);
    this->isOpen = false;

    return this->checkErrors(__FILE__, __LINE__);;
}

lazarus_result WindowManager::createCursor(uint32_t sizeX, uint32_t sizeY, uint32_t targetX, uint32_t targetY, std::string filepath)
{		
    fileReader = std::make_unique<FileLoader>();
	
    lazarus_result status = fileReader->loadImage(this->image, filepath.c_str());
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};

	this->glfwImage.width = sizeX;
	this->glfwImage.height = sizeY;
	this->glfwImage.pixels = this->image.pixelData;
	
	this->cursor = glfwCreateCursor(&this->glfwImage, targetX, targetY);
	glfwSetCursor(this->window, this->cursor);
	
	return this->checkErrors(__FILE__, __LINE__);
};

lazarus_result WindowManager::snapCursor(float moveX, float moveY)
{
    if(moveX > GlobalsManager::getDisplayWidth() || moveY > GlobalsManager::getDisplayHeight())
    {
		LOG_ERROR("GLFW Error:", __FILE__, __LINE__);
		
        return lazarus_result::LAZARUS_INVALID_COORDINATE;
    }
    else
    {
        glfwSetCursorPos(this->window, moveX, moveY);
        return this->checkErrors(__FILE__, __LINE__);
    };
};


lazarus_result WindowManager::presentNextFrame()
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
lazarus_result WindowManager::monitorPixelOccupants()
{
	/* ==========================================
		Notifies MeshManager::drawMesh to draw 
		not only VBO contents but also to draw to
		the stencil buffer.
	============================================= */
	if(!GlobalsManager::getManageStencilBuffer()) 
	{
		GlobalsManager::setManageStencilBuffer(true);
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

lazarus_result WindowManager::checkErrors(const char *file, int line)
{
    errorCode = glfwGetError(&errorMessage); 
    if(errorCode != GLFW_NO_ERROR)
    {
        std::string message = std::string("Window Error: ").append(errorMessage);
        LOG_ERROR(message.c_str(), file, line);

        return lazarus_result::LAZARUS_WINDOW_ERROR;
    }
    else 
    {
    	return lazarus_result::LAZARUS_OK;
    }
};

lazarus_result WindowManager::centerWindow()
{
	int32_t windowLocationX = floor((videoMode->width - this->frame.width) / 2);
	int32_t windowLocationY = floor((videoMode->height - this->frame.height) / 2);
    glfwSetWindowPos(this->window, windowLocationX, windowLocationY);

	return this->checkErrors(__FILE__, __LINE__);
};

void WindowManager::initialiseGLEW()
{
    glewExperimental = GL_TRUE;
    glewInit();

    return;
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
