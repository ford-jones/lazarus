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
#include "../include/lazarus_event_manager.h"

EventManager::EventManager()
{
	keyString	= "";

	keyCode 	= 0;
	osCode 		= 0;
	mouseCode 	= 0;
	mouseX 		= 0;
	mouseY 		= 0;
	scrollCode 	= 0;

	win 		= NULL;
};

void EventManager::initialise()
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

void EventManager::listen()
{	
    glfwPollEvents();
	
    this->updateKeyboardState();
    this->updateMouseState();
};

void EventManager::updateKeyboardState()
{
	/* ==============================
		TODO: 
		Create cases and unique strings for remaining weird keys:
		- capslock 
		- pgup/down 
		- screenshot 
	================================= */
	
	this->keyString = "";
	this->keyCode = 0;
	this->osCode = 0;
	
	this->keyCode = LAZARUS_LISTENER_KEYCODE;
	this->osCode = LAZARUS_LISTENER_SCANCODE;
	
	if(keyCode > 0)
	{
		switch(keyCode)
		{
			case GLFW_KEY_UP :
				this->keyString = "up";
				break;
			case GLFW_KEY_DOWN :
				this->keyString = "down";
				break;
			case GLFW_KEY_LEFT :
				this->keyString = "left";
				break;
			case GLFW_KEY_RIGHT :
				this->keyString = "right";
				break;
			case GLFW_KEY_ENTER :
				this->keyString = "enter";
				break;
			case GLFW_KEY_SPACE :
				this->keyString = "space";
				break;
			case GLFW_KEY_TAB :
				this->keyString = "tab";
				break;
			case GLFW_KEY_LEFT_SHIFT :
				this->keyString = "shift-l";
				break;
			case GLFW_KEY_RIGHT_SHIFT :
				this->keyString = "shift-r";
				break;
			case GLFW_KEY_LEFT_CONTROL :
				this->keyString = "ctrl-l";
				break;
			case GLFW_KEY_RIGHT_CONTROL :
				this->keyString = "ctrl-r";
				break;
			/* ==================================
				TODO: 
				alt and super keys seem buggy?
				investigate.
			===================================== */
			
			case GLFW_KEY_LEFT_ALT :
				this->keyString = "alt-l";
				break;
			case GLFW_KEY_RIGHT_ALT :
				this->keyString = "alt-r";
				break;
			case GLFW_KEY_LEFT_SUPER :
				this->keyString = "fn-l";
				break;
			case GLFW_KEY_RIGHT_SUPER :
				this->keyString = "fn-r";
				break;
			default :
				this->keyString = glfwGetKeyName(keyCode, osCode);
				break;
		};
	};
};

void EventManager::updateMouseState()
{
	this->mouseCode = LAZARUS_MOUSE_NOCLICK;
	this->mouseX = 0.0;
	this->mouseY = 0.0;
	
	this->mouseCode = LAZARUS_LISTENER_MOUSECODE;
	this->mouseX = static_cast<int>(LAZARUS_LISTENER_MOUSEX + 0.5);
	this->mouseY = static_cast<int>(LAZARUS_LISTENER_MOUSEY + 0.5);		
	
	/* =========================================================
		TODO: 
		Right now scroll can only be either 1 (up) || -1 (down)
		It will do for now, but should probably be changed to some sort of incrementing / decrementing number
		At the very least, it should be reset to 0 when the scrollwheel is not in motion
	============================================================ */
	this->scrollCode = static_cast<int>(LAZARUS_LISTENER_SCROLLCODE);
};
