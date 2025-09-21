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

#ifndef LAZARUS_COMMON_H
	#include "lazarus_common.h"
#endif

#include <iostream>
#include <fmod.h>
#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_codec.h>

#include <string>
#include <memory>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

#include "lazarus_file_loader.h"

using std::unique_ptr;
using std::shared_ptr;
using std::string;
using std::vector;

#ifndef LAZARUS_AUDIO_MANAGER_H
#define LAZARUS_AUDIO_MANAGER_H

class AudioManager
{
	public:
		struct AudioConfig
		{
			std::string filepath = "";
			bool is3D = false;
			int32_t loopCount = 0;
		};
		struct Audio 
		{
			uint32_t id;
			string path;

			glm::vec3 sourceLocation;
			
			bool isPaused;
			
			uint32_t duration;
			uint32_t audioIndex;

			AudioConfig config;
		};


		AudioManager();

		lazarus_result initialise();
		lazarus_result createAudio(Audio &out, AudioConfig options);
		lazarus_result loadAudio(Audio &audioIn);

		lazarus_result setPlaybackCursor(Audio &audioIn, uint32_t seconds);
		lazarus_result playAudio(Audio &audioIn);
		lazarus_result pauseAudio(Audio &audioIn);

		lazarus_result updateSourceLocation(Audio &audioIn, glm::vec3 location);
		lazarus_result updateListenerLocation(glm::vec3 location);

		virtual ~AudioManager();

	private:
		struct AudioData 
		{
			FMOD::Sound *sound;
			FMOD::Channel *channel;
			FMOD::ChannelGroup *group;

			FMOD_VECTOR prevSourcePosition;
			FMOD_VECTOR currentSourcePosition;
			FMOD_VECTOR sourceVelocity;
		};
		lazarus_result validateAudioHandle(AudioData &audioData);
		lazarus_result checkErrors(FMOD_RESULT res, const char *file, uint32_t line);

		uint32_t audioDuration;

		FMOD_RESULT result;
		FMOD::System *system;
		FMOD::ChannelGroup *mixer;
		FMOD_VECTOR prevListenerPosition;
		FMOD_VECTOR currentListenerPosition;
		FMOD_VECTOR listenerVelocity;
		FMOD_VECTOR listenerForward;
		FMOD_VECTOR listenerUp;

		unique_ptr<FileLoader> reader;

		vector<AudioData> audioStore;
		AudioData audioData;

		Audio audioOut;
};

#endif
