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
=================================================================== */

#include "../include/lazarus_texture_loader.h"

TextureLoader::TextureLoader(TextureLoader::StorageType storageVariant)
{
	LOG_DEBUG("Constructing Lazarus::TextureLoader");

	this->loader = nullptr;	

	this->image.pixelData = NULL;
	this->image.height = 0;
	this->image.width = 0;

	this->bitmapTexture = 0;
	this->textureStack = 0;

	this->errorCode = 0;

	
	switch (storageVariant)
	{
		case TextureLoader::StorageType::ARRAY:
			glGenTextures(1, &this->textureStack);
			glBindTexture(GL_TEXTURE_2D_ARRAY, this->textureStack);
			break;
		
		case TextureLoader::StorageType::ATLAS:
			glGenTextures(1, &this->bitmapTexture);
			glBindTexture(GL_TEXTURE_2D, this->bitmapTexture);			
			break;

		case TextureLoader::StorageType::CUBEMAP:
			glGenTextures(1, &this->cubeMapTexture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMapTexture);
			break;
	
	default:
		break;
	}
};

void TextureLoader::extendTextureStack(uint32_t maxWidth, uint32_t maxHeight, uint32_t textureLayers)
{
	this->clearErrors();

	glActiveTexture(GL_TEXTURE2);
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

	this->checkErrors(__FILE__, __LINE__);

	return;
};

void TextureLoader::loadImageToTextureStack(FileLoader::Image imageData, GLuint textureLayer)
{	
	this->clearErrors();

	this->image.width = imageData.width;
	this->image.height = imageData.height;
	this->image.pixelData = imageData.pixelData;

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->textureStack);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if(this->image.pixelData != NULL)
	{	
		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY, 									//	target
			0, 														// 	mipmap level (leave as 0 if openGL is generating the mipmaps)
			0, 0, 													// 	xy offset into the layer
			(textureLayer - 1), 									// 	layer depth to set this texture, zero-indexed
			this->image.width, 										//	actual texture width
			this->image.height,										//	actual texture height
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

	this->checkErrors(__FILE__, __LINE__);

	return;
};

void TextureLoader::storeCubeMap(uint32_t width, uint32_t height)
{
	this->clearErrors();

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

	uint32_t mipCount = this->calculateMipLevels(width, height);
	glTexStorage2D(
		GL_TEXTURE_CUBE_MAP, 
		mipCount, 
		GL_RGBA8, 
		width, 
		height
	);

	this->checkErrors(__FILE__, __LINE__);

	return;
};

void TextureLoader::loadCubeMap(std::vector<FileLoader::Image> faces)
{	
	this->clearErrors();

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubeMapTexture);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if(faces.size() > 6)
	{
		LOG_ERROR("Texture Error:", __FILE__, __LINE__);
		globals.setExecutionState(StatusCode::LAZARUS_INVALID_CUBEMAP);
	}
	else
	{
		for(uint8_t i = 0; i < 6; i++)
		{
			this->clearErrors();

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

			this->checkErrors(__FILE__, __LINE__);
		};

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		this->checkErrors(__FILE__, __LINE__);
	};

	return;
};

void TextureLoader::storeBitmapTexture(uint32_t maxWidth, uint32_t maxHeight)
{
	this->clearErrors();
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
		0 is passed into the pixel parameter for the meantime. 

		Note the use of GL_R8. The glyphs are monocolour bitmaps and so are loaded into a 
		single-channel, which is later swizzled into the alpha value of a RGBA 4-channel color 
		on the GPU side. The swizzle can and probably should be done here to make it clearer.
	=========================================================================================== */

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_R8, 
		maxWidth, 
		maxHeight, 
		0, 
		GL_RED, 
		GL_UNSIGNED_BYTE, 
		0
	);

	this->checkErrors(__FILE__, __LINE__);

	return;
};

void TextureLoader::loadBitmapToTexture(FileLoader::Image imageData, uint32_t xOffset, uint32_t yOffset)
{
	this->image.width = imageData.width;
	this->image.height = imageData.height;
	this->image.pixelData = imageData.pixelData;

	this->clearErrors();
	
	/* ================================================================
		Load the glyph's rendered bitmap into the previously allocated
		texture at an offset equal to the current width of the texture
		atlas and the culmilative height of previous alphabet sets.
	=================================================================== */
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->bitmapTexture);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexSubImage2D(
		GL_TEXTURE_2D, 
		0, 
		xOffset, 
		yOffset, 
		this->image.width, 
		this->image.height, 
		GL_RED, 
		GL_UNSIGNED_BYTE, 
		(void *)this->image.pixelData
	);
	this->checkErrors(__FILE__, __LINE__);

	this->clearErrors();

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	this->checkErrors(__FILE__, __LINE__);

	return;
};

uint32_t TextureLoader::calculateMipLevels(uint32_t width, uint32_t height)
{
	uint32_t loopCount = 0;

	uint32_t mipWidth = width;
	uint32_t mipHeight = height;

	loopCount += 1;

	/* ===============================================
		Repeatedly halve the image's dimensions until
		it is 1x1 px in size to calculate the number
		of mips.
	================================================== */
	while( 1 )
	{
		loopCount += 1;

		if(mipWidth != 1)
		{
			uint32_t xResult = static_cast<uint32_t>(floor(mipWidth / 2));
			mipWidth = xResult;
		}

		if (mipHeight != 1)
		{
			uint32_t yResult = static_cast<uint32_t>(floor(mipHeight / 2));
			mipHeight = yResult;
		}

		if ( (mipWidth == 1) && (mipHeight == 1) )
		{
			break;
		}
	}

	return loopCount;
};

void TextureLoader::checkErrors(const char *file, uint32_t line)
{
    this->errorCode = glGetError();
    
    if(this->errorCode != GL_NO_ERROR)
    {
        std::string message = std::string("OpenGL Error: ").append(std::to_string(this->errorCode));
        LOG_ERROR(message.c_str(), file, line);

		globals.setExecutionState(StatusCode::LAZARUS_OPENGL_ERROR);
    } 

	return;
};

void TextureLoader::clearErrors()
{
	this->errorCode = glGetError();
	
	while(this->errorCode != GL_NO_ERROR)
	{
		this->errorCode = glGetError();
	};
};

TextureLoader::~TextureLoader()
{
	LOG_DEBUG("Destroying Lazarus::TextureLoader");

	glDeleteTextures(1, &textureStack);
	glDeleteTextures(1, &bitmapTexture);
	glDeleteTextures(1, &cubeMapTexture);
};
