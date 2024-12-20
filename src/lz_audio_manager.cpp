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
#include "../include/lz_audio_manager.h"

AudioManager::AudioManager() 
{
    std::cout << GREEN_TEXT << "Calling constructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;

	this->audioOut = {};
	this->reader = nullptr;
	
	this->system = NULL;

	this->audioData.channel = NULL;
	this->audioData.group = NULL;
	this->audioData.sound = NULL;

	this->audioData.prevSourcePosition = {0.0f, 0.0f, 0.0f};
	this->audioData.currentSourcePosition = {0.0f, 0.0f, 0.0f};
	this->audioData.sourceVelocity = {0.0f, 0.0f, 0.0f};

	this->prevListenerPosition = {0.0f, 0.0f, 0.0f};
	this->currentListenerPosition = {0.0f, 0.0f, 0.0f};
	this->listenerVelocity = {0.0f, 0.0f, 0.0f};

	this->listenerForward = {0.0f, 0.0f, 1.0f};
	this->listenerUp = {0.0f, 1.0f, 0.0f};
};

void AudioManager::initialise()
{
	this->result = FMOD::System_Create(&this->system);
	/* ==============================================
		Initialise the system with a right-handed 
		coordinate system, the same as OpenGL's
	================================================= */
	this->result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);

	this->result = system->createChannelGroup("lazarusGroup", &audioData.group);

	this->checkErrors(this->result);
};

AudioManager::Audio AudioManager::createAudio(string filepath, bool is3D, int loopCount)
{
	this->audioOut = {};

	this->reader = std::make_unique<FileReader>();

	srand(time((0)));
	audioOut.id = 1 + (rand() % 2147483647);

	audioOut.sourceLocationX = 0.0f;
	audioOut.sourceLocationY = 0.0f;
	audioOut.sourceLocationZ = 0.0f;

	audioOut.path = reader->relativePathToAbsolute(filepath);
	audioOut.is3D = is3D;
	audioOut.loopCount = loopCount;
	audioOut.isPaused = true;
	audioOut.audioIndex = 0;

	return audioOut;
};

void AudioManager::setPlaybackCursor(AudioManager::Audio &audioIn, int seconds)
{
	this->audioData = this->audioStore[audioIn.audioIndex - 1];

	this->result = this->audioData.channel->setPosition((seconds * 100), FMOD_TIMEUNIT_MS);

	if(result != FMOD_OK)
	{
		globals.setExecutionState(LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR);
	};
};

void AudioManager::loadAudio(AudioManager::Audio &audioIn)
{	
	(audioIn.is3D == true) 
	? this->result = system->createSound(audioIn.path.c_str(), FMOD_3D, NULL, &this->audioData.sound) 
	: this->result = system->createSound(audioIn.path.c_str(), FMOD_DEFAULT, NULL, &this->audioData.sound);
	
	this->checkErrors(this->result);
	
	if(this->audioData.sound != NULL)
	{
		this->result = system->playSound(this->audioData.sound, audioData.group, false, &audioData.channel);
		
		if (audioIn.loopCount != 0)
		{
			audioData.channel->setMode(FMOD_LOOP_NORMAL);
			audioData.channel->setLoopCount(audioIn.loopCount);
		};
		
		audioData.channel->setPaused(true);
	}
	else
	{
		globals.setExecutionState(LAZARUS_FILE_NOT_FOUND);
		std::cout << RED_TEXT << "LAZARUS::ERROR::SOUND_MANAGER" << std::endl;	
		std::cout << "Status: " << LAZARUS_FILE_NOT_FOUND << RESET_TEXT << std::endl;	
	}

	this->checkErrors(this->result);
	
	this->audioStore.push_back(audioData);
	audioIn.audioIndex = this->audioStore.size();

	audioData.channel = NULL;
	audioData.group = NULL;
	audioData.sound = NULL;

	return;
};

void AudioManager::playAudio(AudioManager::Audio &audioIn)
{
	this->audioData = this->audioStore[audioIn.audioIndex - 1];

	if(audioIn.isPaused == true)
	{
		this->result = this->audioData.channel->setPaused(false);
		audioIn.isPaused = false;
	}

	this->checkErrors(this->result);

	return;
};

void AudioManager::pauseAudio(AudioManager::Audio &audioIn)
{
	this->audioData = this->audioStore[audioIn.audioIndex - 1];

	if(audioIn.isPaused == false)
	{
		this->result = this->audioData.channel->setPaused(true);
		audioIn.isPaused = true;
	}

	this->checkErrors(this->result);

	return;
};

void AudioManager::updateSourceLocation(AudioManager::Audio &audioIn, float x, float y, float z)
{
	AudioData targetAudio = this->audioStore[audioIn.audioIndex - 1];

	targetAudio.currentSourcePosition = {x, y, z};

	targetAudio.sourceVelocity = {
		((targetAudio.currentSourcePosition.x - targetAudio.prevSourcePosition.x) * (1000 / 60)),
		((targetAudio.currentSourcePosition.y - targetAudio.prevSourcePosition.y) * (1000 / 60)),
		((targetAudio.currentSourcePosition.z - targetAudio.prevSourcePosition.z) * (1000 / 60))
	};

	this->result = targetAudio.channel->set3DAttributes(&targetAudio.currentSourcePosition, &targetAudio.sourceVelocity);

	this->checkErrors(this->result);

	this->result = system->update();
	targetAudio.prevSourcePosition = targetAudio.currentSourcePosition;

	this->checkErrors(this->result);

	audioIn.sourceLocationX = targetAudio.prevSourcePosition.x;
	audioIn.sourceLocationY = targetAudio.prevSourcePosition.y;
	audioIn.sourceLocationZ = targetAudio.prevSourcePosition.z;

	return;
};

void AudioManager::updateListenerLocation(float x, float y, float z)
{
	this->currentListenerPosition = {x, y, z};

	/* =====================================
		TODO:
		Change (1000 / 60) to FpsCounter::durationTillRendered
	======================================== */

	this->listenerVelocity = {
		((this->currentListenerPosition.x - this->prevListenerPosition.x) * (1000 / 60)),
		((this->currentListenerPosition.y - this->prevListenerPosition.y) * (1000 / 60)),
		((this->currentListenerPosition.z - this->prevListenerPosition.z) * (1000 / 60))
	};

	this->result = system->set3DListenerAttributes(
		0, 
		&this->currentListenerPosition, 
		&this->listenerVelocity,
		&this->listenerForward,
		&this->listenerUp
	);
	
	this->checkErrors(this->result);

	this->result = system->update();
	this->prevListenerPosition = this->currentListenerPosition;

	this->checkErrors(this->result);

	this->listenerLocationX = this->prevListenerPosition.x;
	this->listenerLocationY = this->prevListenerPosition.y;
	this->listenerLocationZ = this->prevListenerPosition.z;
};

void AudioManager::checkErrors(FMOD_RESULT res) 
{
	if(res != FMOD_OK)
	{
		globals.setExecutionState(LAZARUS_AUDIO_ERROR);
		std::cout << RED_TEXT << "LAZARUS::ERROR::SOUND_MANAGER" << std::endl;
		std::cout << "Status: " << LAZARUS_EXECUTION_STATUS << RESET_TEXT << std::endl;
	};
};

AudioManager::~AudioManager()
{
    std::cout << GREEN_TEXT << "Calling destructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
	
	for(unsigned int i = 0; i < this->audioStore.size(); i++)
	{
		AudioManager::AudioData data = this->audioStore[i];

		data.sound->release();
		data.group->release();
	}

	system->release();
};
