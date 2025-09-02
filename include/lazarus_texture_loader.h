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
#ifndef LAZARUS_GL_INCLUDES_H
    #include "lazarus_gl_includes.h"
#endif

#ifndef LAZARUS_COMMON_H
	#include "lazarus_common.h"
#endif

#include <iostream>
#include <string>
#include <memory>
#include <cmath>

#include "lazarus_file_loader.h"

using std::string;
using std::shared_ptr;

#ifndef LAZARUS_TEXTURE_LOADER_H
#define LAZARUS_TEXTURE_LOADER_H

class TextureLoader
{
	
	protected:
		enum StorageType
		{
			CUBEMAP	= 1,
			ATLAS	= 2,
			ARRAY	= 3
		};
		struct TextureData
		{
			int32_t unitId;

            GLuint samplerId;
			bool discardAlphaZero;
		};
		TextureLoader(StorageType storageVariant);
		void extendTextureStack(uint32_t maxWidth, uint32_t maxHeight, uint32_t textureLayers);
		void loadImageToTextureStack(FileLoader::Image imageData, GLuint textureLayer);
		
		void storeCubeMap(uint32_t width, uint32_t height);
		void loadCubeMap(std::vector<FileLoader::Image> faces);
		
		void storeBitmapTexture(uint32_t maxWidth, uint32_t maxHeight);
		void loadBitmapToTexture(FileLoader::Image imageData, uint32_t xOffset, uint32_t yOffset);
		
		virtual ~TextureLoader();

		GLuint textureId;		

	private:		
		uint32_t calculateMipLevels(uint32_t width, uint32_t height);
		void checkErrors(const char *file, uint32_t line);
		void clearErrors();
		
		GLenum errorCode;

		shared_ptr<FileLoader> loader;
		FileLoader::Image image;
		StorageType storageType;

		GlobalsManager globals;
};

#endif
