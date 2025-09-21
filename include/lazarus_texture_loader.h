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
		lazarus_result extendTextureStack(uint32_t maxWidth, uint32_t maxHeight, uint32_t textureLayers);
		lazarus_result loadImageToTextureStack(FileLoader::Image imageData, GLuint textureLayer);
		
		lazarus_result storeCubeMap(uint32_t width, uint32_t height);
		lazarus_result loadCubeMap(std::vector<FileLoader::Image> faces);
		
		lazarus_result storeBitmapTexture(uint32_t maxWidth, uint32_t maxHeight);
		lazarus_result loadBitmapToTexture(FileLoader::Image imageData, uint32_t xOffset, uint32_t yOffset);
		
		virtual ~TextureLoader();

		GLuint textureId;		

	private:		
		uint32_t calculateMipLevels(uint32_t width, uint32_t height);
		lazarus_result checkErrors(const char *file, uint32_t line);
		lazarus_result clearErrors();
		
		GLenum errorCode;

		shared_ptr<FileLoader> loader;
		FileLoader::Image image;
		StorageType storageType;
};

#endif
