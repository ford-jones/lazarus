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
		void extendTextureStack(int maxWidth, int maxHeight, int textureLayers);
		void loadImageToTextureStack(FileReader::Image imageData, GLuint textureLayer);

		void storeCubeMap(int width, int height);
		void loadCubeMap(std::vector<FileReader::Image> faces);

		void storeBitmapTexture(int maxWidth, int maxHeight);
		void loadBitmapToTexture(FileReader::Image imageData, int xOffset, int yOffset);

		virtual ~TextureLoader();
		
		GLuint bitmapTexture;
		GLuint textureStack;
		GLuint cubeMapTexture;

	private:		
		int countMipLevels(int width, int height);
		void checkErrors(const char *file, int line);

		int mipCount;
		int loopCount;
		int x;
		int y;
		int atlasHeight;
		int atlasWidth;
		
		int xOffset;
		int yOffset;

		GLenum errorCode;

		shared_ptr<FileReader> loader;
		FileReader::Image image;

		GlobalsManager globals;
};

#endif
