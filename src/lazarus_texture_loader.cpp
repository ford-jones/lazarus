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

/* ================================================================
	TODO:
		- Support for texture-per-face loading
		- Support for multiple textured meshes (layering)
		- Ideally have each of these functions use immutable storage
		- Create and bind textures where needed. This gets ugly fast 
		  as a scene starts to grow.
=================================================================== */

#include "../include/lazarus_texture_loader.h"

TextureLoader::TextureLoader()
{
	std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;

	this->loader = nullptr;	

	this->image.pixelData = NULL;
	this->image.height = 0;
	this->image.width = 0;

	this->bitmapTexture = 0;
	this->textureStack = 0;

	this->x = 0;
	this->y = 0;
	this->loopCount = 0;
	this->mipCount = 0;
	this->errorCode = 0;
	
	this->offset = 0;

	glGenTextures(1, &this->textureStack);
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->textureStack);

	glGenTextures(1, &this->bitmapTexture);
	glBindTexture(GL_TEXTURE_2D, this->bitmapTexture);

	glGenTextures(1, &this->cubeMapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMapTexture);
};

void TextureLoader::extendTextureStack(int maxWidth, int maxHeight, int textureLayers)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->textureStack);

	/* =========================================================================
		Allocate / Reallocate memory to store texture data. Doesn't actually 
		upload the texture yet, that's done later. Just (re)provisions room for 
		the incoming load.
	============================================================================ */
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY, 											//	target
		0, 																//	mip level (0 because opengl is generating the mipmap)
		GL_RGBA8, 														//	internal format
		maxWidth, maxHeight,									 		//	width, height
		textureLayers, 													//	layers
		0, 																//	border, deprecated - always 0
		GL_RGBA, 														//	external format
		GL_UNSIGNED_BYTE, 												//	channel type
		NULL															//	pixel data, NULL because the texture will be subImage'd in later
	);

	const char* signature = __FILE__;
	int line = __LINE__;

	std::string file = signature;
	this->checkErrors(std::string("File: ").append(file.append(" Line : (" + std::to_string(line) + ")")).c_str());

	return;
};

void TextureLoader::loadImageToTextureStack(FileReader::Image imageData, GLuint textureLayer)
{	
	this->image.width = imageData.width;
	this->image.height = imageData.height;
	this->image.pixelData = imageData.pixelData;

	if(this->image.pixelData != NULL)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		
		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY, 									//	target
			0, 														// 	mipmap level (leave as 0 if openGL is generating the mipmaps)
			0, 0, 													// 	xy offset into the layer
			(textureLayer - 1), 									// 	layer depth to set this texture, zero-indexed
			this->image.width, this->image.height,					//	actual texture width / height
			1, 														// 	number of layers being passed each time this is called
			GL_RGBA, 												//	texel data format
			GL_UNSIGNED_BYTE, 										//	texel data type
			((const void *)(this->image.pixelData)) 				// 	texel data
		);
	
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	}

	const char* signature = __FILE__;
	int line = __LINE__;

	std::string file = signature;
	this->checkErrors(std::string("File: ").append(file.append(" Line : (" + std::to_string(line) + ")")).c_str());
};

void TextureLoader::storeCubeMap(int width, int height)
{
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMapTexture);
	
	/* ===========================================================
		Calculate the depth of the mip map (levels) for the given
		width / height params, which are the dimensions for the 
		images that make up each face of the cubemap. Despite only
		handing over the dimensions for one face, OpenGL will 
		multiply this number internally by 6 to allocate storage 
		each mip of each face.
	============================================================== */

	this->mipCount = this->countMipLevels(width, height);
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, this->mipCount, GL_RGBA8, width, height);

	const char* signature = __FILE__;
	int line = __LINE__;

	std::string file = signature;
	this->checkErrors(std::string("File: ").append(file.append(" Line : (" + std::to_string(line) + ")")).c_str());
};

void TextureLoader::loadCubeMap(std::vector<FileReader::Image> faces)
{	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMapTexture);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if(faces.size() > 6)
	{
		globals.setExecutionState(LAZARUS_INVALID_CUBEMAP);
	}
	else
	{
		for(unsigned int i = 0; i < 6; i++)
		{
			/* ===================================================
				For each face; buffer the images pixel data to 
				the respective faces target binding. These targets 
				are intrinsically related to / a part of the 
				GL_TEXTURE_CUBE_MAP texture name which is currently 
				bound to the active texture slot.
			====================================================== */
			GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

			glTexSubImage2D(
				target,
				0,
				0, 0,
				faces[0].width, faces[0].height,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				(const void *)faces[i].pixelData
			);

			const char* signature = __FILE__;
			int line = __LINE__;

			std::string file = signature;
			this->checkErrors(std::string("File: ").append(file.append(" Line : (" + std::to_string(line) + ")")).c_str());
		};

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		const char* signature = __FILE__;
		int line = __LINE__;

		std::string file = signature;
		this->checkErrors(std::string("File: ").append(file.append(" Line : (" + std::to_string(line) + ")")).c_str());
	};
};

void TextureLoader::storeBitmapTexture(int maxWidth, int maxHeight)
{
	/* ===========================================
		Hardcoded because this function is used 
		specifically for glyph loading only. If 
		that ends up changing then it should be
		made dynamic. See other similar 
		glActiveTexture calls.
	============================================== */
	glActiveTexture(GL_TEXTURE1);
	
	glBindTexture(GL_TEXTURE_2D, this->bitmapTexture);

	/* ========================================================================================
		Allocate space for the texture atlas. The texture atlas hasn't been constructed yet so
		0 is passed in for the meantime. 

		Note the use of GL_R8. The glyphs are monocolour bitmaps and so are loaded into a 
		single-channel, which is later swizzled into the alpha value of a RGBA 4-channel color 
		on the GPU side. The swizzle can and probably should be done here to make it clearer.
	=========================================================================================== */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, maxWidth, maxHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	const char* signature = __FILE__;
	int line = __LINE__;

	std::string file = signature;
	this->checkErrors(std::string("File: ").append(file.append(" Line : (" + std::to_string(line) + ")")).c_str());

	this->offset = 0;
};

void TextureLoader::loadBitmapToTexture(FileReader::Image imageData)
{
	this->image.width = imageData.width;
	this->image.height = imageData.height;
	this->image.pixelData = imageData.pixelData;

	/* ================================================================
		Load the glyph's rendered bitmap into the previously allocated
		texture at an offset equal to the current width of the texture
		atlas.
	=================================================================== */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexSubImage2D(
		GL_TEXTURE_2D, 
		0, 
		this->offset, 
		0, 
		this->image.width, 
		this->image.height, 
		GL_RED, 
		GL_UNSIGNED_BYTE, 
		(void *)this->image.pixelData
	);

	offset += imageData.width;

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	
};

int TextureLoader::countMipLevels(int width, int height)
{
	this->loopCount = 0;

	this->x = width;
	this->y = height;

	this->loopCount += 1;

	while( 1 )
	{
		this->loopCount += 1;

		if(this->x != 1)
		{
			int xResult = floor(this->x / 2);
			this->x = xResult;
		}

		if (this->y != 1)
		{
			int yResult = floor(this->y / 2);
			this->y = yResult;
		}

		if ( (this->x == 1) && (this->y == 1) )
		{
			break;
		}
	}

	return this->loopCount;
};

void TextureLoader::checkErrors(const char *invoker)
{
    this->errorCode = glGetError();
    
    if(this->errorCode != 0)
    {
        std::cerr << RED_TEXT << "ERROR::GL_ERROR::CODE " << RESET_TEXT << this->errorCode << std::endl;
        std::cerr << RED_TEXT << "INVOKED BY: " << RESET_TEXT << invoker << std::endl;

		globals.setExecutionState(LAZARUS_OPENGL_ERROR);
    } 

	return;
};

TextureLoader::~TextureLoader()
{
	std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;

	glDeleteTextures(1, &textureStack);
	glDeleteTextures(1, &bitmapTexture);
};
