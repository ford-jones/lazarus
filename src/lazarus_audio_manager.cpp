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
#include "../include/lazarus_audio_manager.h"

AudioManager::AudioManager() 
{
    std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__  << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;

	this->audioOut = {};
	this->reader = nullptr;
	
	this->system = NULL;

	this->mixer = NULL;

	this->prevListenerPosition = {0.0f, 0.0f, 0.0f};
	this->currentListenerPosition = {0.0f, 0.0f, 0.0f};
	this->listenerVelocity = {0.0f, 0.0f, 0.0f};

	this->listenerForward = {0.0f, 0.0f, 1.0f};
	this->listenerUp = {0.0f, 1.0f, 0.0f};

	this->audioDuration = 0;
};

void AudioManager::initialise()
{
	this->result = FMOD::System_Create(&this->system);
	/* ==============================================
		Initialise the system with a right-handed 
		coordinate system, the same as OpenGL's
	================================================= */
	this->result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);

	this->result = system->createChannelGroup("lazarusGroup", &this->mixer);

	this->checkErrors(this->result, __FILE__, __LINE__);
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
	AudioData &audioData = this->audioStore[audioIn.audioIndex - 1];

	this->validateAudioHandle(audioData);

	this->result = audioData.channel->setPosition((seconds * 100), FMOD_TIMEUNIT_MS);

	if(result != FMOD_OK)
	{
		globals.setExecutionState(LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR);
	};
};

void AudioManager::loadAudio(AudioManager::Audio &audioIn)
{	
	AudioData data = {} ;
	
	data.group = this->mixer;

	(audioIn.is3D == true) 
	? this->result = system->createSound(audioIn.path.c_str(), FMOD_3D, NULL, &data.sound) 
	: this->result = system->createSound(audioIn.path.c_str(), FMOD_DEFAULT, NULL, &data.sound);
	
	this->checkErrors(this->result, __FILE__, __LINE__);
	
	if(data.sound != NULL)
	{
		this->result = system->playSound(data.sound, data.group, false, &data.channel);
		this->checkErrors(this->result, __FILE__, __LINE__);

		this->result = data.sound->getLength(&audioDuration, FMOD_TIMEUNIT_MS);
		this->checkErrors(this->result, __FILE__, __LINE__);

		audioIn.duration = ceil(audioDuration / 1000);

		if (audioIn.loopCount != 0)
		{
			data.channel->setMode(FMOD_LOOP_NORMAL);
			data.channel->setLoopCount(audioIn.loopCount);
		};
		
		this->result = data.channel->setPaused(true);
		this->checkErrors(this->result, __FILE__, __LINE__);

	}
	else
	{
		globals.setExecutionState(LAZARUS_FILE_NOT_FOUND);
		std::cout << RED_TEXT << "LAZARUS::ERROR::SOUND_MANAGER" << std::endl;	
		std::cout << "Status: " << LAZARUS_FILE_NOT_FOUND << RESET_TEXT << std::endl;	
	}

	this->checkErrors(this->result, __FILE__, __LINE__);
	
	this->audioStore.push_back(data);
	audioIn.audioIndex = this->audioStore.size();

	return;
};

void AudioManager::playAudio(AudioManager::Audio &audioIn)
{
	AudioData &audioData = this->audioStore[audioIn.audioIndex - 1];

	this->validateAudioHandle(audioData);

	if(audioIn.isPaused == true)
	{
		this->result = audioData.channel->setPaused(false);
		audioIn.isPaused = false;
	}

	this->checkErrors(this->result, __FILE__, __LINE__);

	return;
};

void AudioManager::pauseAudio(AudioManager::Audio &audioIn)
{
	AudioData &audioData = this->audioStore[audioIn.audioIndex - 1];

	this->validateAudioHandle(audioData);

	if(audioIn.isPaused == false)
	{
		this->result = audioData.channel->setPaused(true);
		audioIn.isPaused = true;
	}

	return;
};

void AudioManager::updateSourceLocation(AudioManager::Audio &audioIn, float x, float y, float z)
{
	AudioData &audioData = this->audioStore[audioIn.audioIndex - 1];

	this->validateAudioHandle(audioData);

	audioData.currentSourcePosition = {x, y, z};

	audioData.sourceVelocity = {
		((audioData.currentSourcePosition.x - audioData.prevSourcePosition.x) / (1000 / 60)),
		((audioData.currentSourcePosition.y - audioData.prevSourcePosition.y) / (1000 / 60)),
		((audioData.currentSourcePosition.z - audioData.prevSourcePosition.z) / (1000 / 60))
	};

	this->result = audioData.channel->set3DAttributes(&audioData.currentSourcePosition, &audioData.sourceVelocity);

	this->result = system->update();
	this->checkErrors(this->result, __FILE__, __LINE__);

	audioData.prevSourcePosition = audioData.currentSourcePosition;

	audioIn.sourceLocationX = audioData.prevSourcePosition.x;
	audioIn.sourceLocationY = audioData.prevSourcePosition.y;
	audioIn.sourceLocationZ = audioData.prevSourcePosition.z;

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
		((this->currentListenerPosition.x - this->prevListenerPosition.x) / (1000 / 60)),
		((this->currentListenerPosition.y - this->prevListenerPosition.y) / (1000 / 60)),
		((this->currentListenerPosition.z - this->prevListenerPosition.z) / (1000 / 60))
	};

	this->result = system->set3DListenerAttributes(
		0, 
		&this->currentListenerPosition, 
		&this->listenerVelocity,
		&this->listenerForward,
		&this->listenerUp
	);
	
	this->checkErrors(this->result, __FILE__, __LINE__);

	this->result = system->update();
	this->prevListenerPosition = this->currentListenerPosition;

	this->checkErrors(this->result, __FILE__, __LINE__);

	this->listenerLocationX = this->prevListenerPosition.x;
	this->listenerLocationY = this->prevListenerPosition.y;
	this->listenerLocationZ = this->prevListenerPosition.z;
};

void AudioManager::validateAudioHandle(AudioData &audioData)
{
	/* =========================================
		Channel handles become invalid upon it's
		audio playback reaching completion. 
		https://www.fmod.com/docs/2.02/api/white-papers-handle-system.html#core-api-channels

		Perform a channel operation that has
		little overhead so that the result can
		be checked.
	============================================ */
	int index = 0;
	this->result = audioData.channel->getIndex(&index);
	
	if(result == FMOD_ERR_INVALID_HANDLE)
	{
		/* ===============================================
			Ensure that the AudioData object is holding an 
			up to date reference to a valid channel handle 
			by having FMOD reload the sample into one of
			it's free channels (512 max).
		================================================== */
		this->result = system->playSound(audioData.sound, audioData.group, false, &audioData.channel);
		
		if(result != FMOD_OK)
		{
			globals.setExecutionState(LAZARUS_AUDIO_LOAD_ERROR);
		};
	};

	return;
};

void AudioManager::checkErrors(FMOD_RESULT res, const char *file, int line) 
{
	if(res != FMOD_OK)
	{
		std::cerr << RED_TEXT << file << " (" << line << ")" << RESET_TEXT << std::endl;
		std::cout << RED_TEXT << "LAZARUS::ERROR::SOUND_MANAGER " << res << RESET_TEXT << std::endl;

		globals.setExecutionState(LAZARUS_AUDIO_ERROR);
	};

	return;
};

AudioManager::~AudioManager()
{
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
	
	for(unsigned int i = 0; i < this->audioStore.size(); i++)
	{
		AudioManager::AudioData data = this->audioStore[i];

		data.sound->release();
		data.group->release();
	}

	system->release();
};
