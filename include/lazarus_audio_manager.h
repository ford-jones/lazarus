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
			int id;
			string path;

			float sourceLocationX;
			float sourceLocationY;
			float sourceLocationZ;

			int duration;

			bool is3D;
			bool isPaused;

			int loopCount;

			unsigned int audioIndex;
		};

		AudioManager();

		void initialise();
		Audio createAudio(string filepath, bool is3D = false, int loopCount = 0);
		void loadAudio(Audio &audioIn);

		void setPlaybackCursor(Audio &audioIn, int seconds);
		void playAudio(Audio &audioIn);
		void pauseAudio(Audio &audioIn);

		void updateSourceLocation(Audio &audioIn, float x, float y, float z);
		void updateListenerLocation(float x, float y, float z);

		virtual ~AudioManager();

		float listenerLocationX;
		float listenerLocationY;
		float listenerLocationZ;

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
		void checkErrors(FMOD_RESULT res, const char *file, int line);

		unsigned int audioDuration;

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
