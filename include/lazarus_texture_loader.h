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

#include "lazarus_file_reader.h"

using std::string;
using std::shared_ptr;

#ifndef LAZARUS_TEXTURE_LOADER_H
#define LAZARUS_TEXTURE_LOADER_H

class TextureLoader
{
	public:
		TextureLoader();
		void extendTextureStack(uint32_t maxWidth, uint32_t maxHeight, uint32_t textureLayers);
		void loadImageToTextureStack(FileReader::Image imageData, GLuint textureLayer);

		void storeCubeMap(uint32_t width, uint32_t height);
		void loadCubeMap(std::vector<FileReader::Image> faces);

		void storeBitmapTexture(uint32_t maxWidth, uint32_t maxHeight);
		void loadBitmapToTexture(FileReader::Image imageData, uint32_t xOffset, uint32_t yOffset);

		virtual ~TextureLoader();
		
		GLuint bitmapTexture;
		GLuint textureStack;
		GLuint cubeMapTexture;

	private:		
		uint32_t calculateMipLevels(uint32_t width, uint32_t height);
		void checkErrors(const char *file, uint32_t line);

		GLenum errorCode;

		shared_ptr<FileReader> loader;
		FileReader::Image image;

		GlobalsManager globals;
};

#endif
