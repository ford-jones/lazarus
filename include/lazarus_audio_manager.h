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
#include <time.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

#include "lazarus_file_reader.h"

using std::unique_ptr;
using std::shared_ptr;
using std::string;
using std::vector;

#ifndef LAZARUS_AUDIO_MANAGER_H
#define LAZARUS_AUDIO_MANAGER_H

class AudioManager
{
	public:
		struct Audio 
		{
			uint32_t id;
			string path;

			glm::vec3 sourceLocation;
			
			bool is3D;
			bool isPaused;
			
			int32_t loopCount;
			
			uint32_t duration;
			uint32_t audioIndex;
		};

		AudioManager();

		void initialise();
		Audio createAudio(string filepath, bool is3D = false, int32_t loopCount = 0);
		void loadAudio(Audio &audioIn);

		void setPlaybackCursor(Audio &audioIn, uint32_t seconds);
		void playAudio(Audio &audioIn);
		void pauseAudio(Audio &audioIn);

		void updateSourceLocation(Audio &audioIn, _Float32 x, _Float32 y, _Float32 z);
		void updateListenerLocation(_Float32 x, _Float32 y, _Float32 z);

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
		void validateAudioHandle(AudioData &audioData);
		void checkErrors(FMOD_RESULT res, const char *file, uint32_t line);

		uint32_t audioDuration;

		FMOD_RESULT result;
		FMOD::System *system;
		FMOD::ChannelGroup *mixer;
		FMOD_VECTOR prevListenerPosition;
		FMOD_VECTOR currentListenerPosition;
		FMOD_VECTOR listenerVelocity;
		FMOD_VECTOR listenerForward;
		FMOD_VECTOR listenerUp;

		unique_ptr<FileReader> reader;

		vector<AudioData> audioStore;
		AudioData audioData;

		Audio audioOut;

		GlobalsManager globals;
};

#endif
