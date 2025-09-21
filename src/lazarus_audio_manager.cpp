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
	LOG_DEBUG("Constructing Lazarus::AudioManager");

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

lazarus_result AudioManager::initialise()
{
	this->reader = std::make_unique<FileLoader>();

	this->result = FMOD::System_Create(&this->system);
	/* ==============================================
		Initialise the system with a right-handed 
		coordinate system, the same as OpenGL's
	================================================= */
	this->result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);
	this->result = system->createChannelGroup("lazarusGroup", &this->mixer);

	return this->checkErrors(this->result, __FILE__, __LINE__);
};

lazarus_result AudioManager::createAudio(AudioManager::Audio &out, AudioManager::AudioConfig options)
{
	lazarus_result status = reader->relativePathToAbsolute(options.filepath, out.path);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};

	out.id = audioStore.size();
	out.sourceLocation = {0.0f, 0.0f, 0.0f};
	out.config = options;
	out.isPaused = true;
	out.audioIndex = 0;

	return lazarus_result::LAZARUS_OK;
};

lazarus_result AudioManager::setPlaybackCursor(AudioManager::Audio &audioIn, uint32_t seconds)
{
	AudioData &audioData = this->audioStore[audioIn.audioIndex - 1];

	lazarus_result status = this->validateAudioHandle(audioData);
	if(status != lazarus_result::LAZARUS_OK)
	{		
		return status;
	};

	this->result = audioData.channel->setPosition((seconds * 100), FMOD_TIMEUNIT_MS);
	if(result != FMOD_OK)
	{
		LOG_ERROR("Sound Error:", __FILE__, __LINE__);
		return lazarus_result::LAZARUS_AUDIO_PLAYBACK_POSITION_ERROR;
	};

	return LAZARUS_OK;
};

lazarus_result AudioManager::loadAudio(AudioManager::Audio &audioIn)
{	
	AudioData data = {} ;
	
	data.group = this->mixer;

	(audioIn.config.is3D == true) 
	? this->result = system->createSound(audioIn.path.c_str(), FMOD_3D, NULL, &data.sound) 
	: this->result = system->createSound(audioIn.path.c_str(), FMOD_DEFAULT, NULL, &data.sound);
	
	lazarus_result status = this->checkErrors(this->result, __FILE__, __LINE__);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};
	
	if(data.sound != NULL)
	{
		this->result = system->playSound(
			data.sound, 
			data.group, 
			false, 
			&data.channel
		);
		this->checkErrors(this->result, __FILE__, __LINE__);

		this->result = data.sound->getLength(&audioDuration, FMOD_TIMEUNIT_MS);

		lazarus_result status = this->checkErrors(this->result, __FILE__, __LINE__);
		if(status != lazarus_result::LAZARUS_OK)
		{
			return status;
		};

		audioIn.duration = ceil(audioDuration / 1000);

		if (audioIn.config.loopCount != 0)
		{
			data.channel->setMode(FMOD_LOOP_NORMAL);
			data.channel->setLoopCount(audioIn.config.loopCount);
		};
		
		this->result = data.channel->setPaused(true);

		status = this->checkErrors(this->result, __FILE__, __LINE__);
		if(status != lazarus_result::LAZARUS_OK)
		{
			return status;
		};
	}
	else
	{
		LOG_ERROR("Sound Error:", __FILE__, __LINE__);

		return lazarus_result::LAZARUS_FILE_NOT_FOUND;
	}
	
	this->audioStore.push_back(data);
	audioIn.audioIndex = this->audioStore.size();

	return lazarus_result::LAZARUS_OK;
};

lazarus_result AudioManager::playAudio(AudioManager::Audio &audioIn)
{
	AudioData &audioData = this->audioStore[audioIn.audioIndex - 1];

	lazarus_result status = this->validateAudioHandle(audioData);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};

	if(audioIn.isPaused == true)
	{
		this->result = audioData.channel->setPaused(false);
		audioIn.isPaused = false;
	}

	return this->checkErrors(this->result, __FILE__, __LINE__);
};

lazarus_result AudioManager::pauseAudio(AudioManager::Audio &audioIn)
{
	AudioData &audioData = this->audioStore[audioIn.audioIndex - 1];

	lazarus_result status = this->validateAudioHandle(audioData);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};

	if(audioIn.isPaused == false)
	{
		this->result = audioData.channel->setPaused(true);
		audioIn.isPaused = true;
	}

	return this->checkErrors(this->result, __FILE__, __LINE__);
};

lazarus_result AudioManager::updateSourceLocation(AudioManager::Audio &audioIn, glm::vec3 location)
{
	AudioData &audioData = this->audioStore[audioIn.audioIndex - 1];

	lazarus_result status = this->validateAudioHandle(audioData);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};

	audioData.currentSourcePosition = {location.x, location.y, location.z};

	audioData.sourceVelocity = {
		((audioData.currentSourcePosition.x - audioData.prevSourcePosition.x) / (1000 / 60)),
		((audioData.currentSourcePosition.y - audioData.prevSourcePosition.y) / (1000 / 60)),
		((audioData.currentSourcePosition.z - audioData.prevSourcePosition.z) / (1000 / 60))
	};

	this->result = audioData.channel->set3DAttributes(&audioData.currentSourcePosition, &audioData.sourceVelocity);

	this->result = system->update();
	status = checkErrors(this->result, __FILE__, __LINE__);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};

	audioData.prevSourcePosition = audioData.currentSourcePosition;

	audioIn.sourceLocation = {
		audioData.prevSourcePosition.x,
		audioData.prevSourcePosition.y,
		audioData.prevSourcePosition.z
	};

	return LAZARUS_OK;
};

lazarus_result AudioManager::updateListenerLocation(glm::vec3 location)
{
	this->currentListenerPosition = {location.x, location.y, location.z};

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
	
	lazarus_result status = this->checkErrors(this->result, __FILE__, __LINE__);
	if(status != lazarus_result::LAZARUS_OK)
	{
		return status;
	};

	this->result = system->update();
	this->prevListenerPosition = this->currentListenerPosition;

	return this->checkErrors(this->result, __FILE__, __LINE__);
};

lazarus_result AudioManager::validateAudioHandle(AudioData &audioData)
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
		this->result = system->playSound(
			audioData.sound, 
			audioData.group, 
			false, 
			&audioData.channel
		);
		
		if(result != FMOD_OK)
		{
			LOG_ERROR("Sound Error:", __FILE__, __LINE__);

			return lazarus_result::LAZARUS_AUDIO_LOAD_ERROR;
		};
	};

	return lazarus_result::LAZARUS_OK;
};

lazarus_result AudioManager::checkErrors(FMOD_RESULT res, const char *file, uint32_t line) 
{
	if(res != FMOD_OK)
	{
		std::string message = std::string("Sound Error: ").append(std::to_string(res));
		LOG_ERROR(message.c_str(), file, line);

		return lazarus_result::LAZARUS_AUDIO_ERROR;
	};

	return lazarus_result::LAZARUS_OK;
};

AudioManager::~AudioManager()
{
    LOG_DEBUG("Destroying Lazarus::AudioManager");
	
	for(size_t i = 0; i < this->audioStore.size(); i++)
	{
		AudioManager::AudioData data = this->audioStore[i];

		data.sound->release();
		data.group->release();
	}

	system->release();
};
