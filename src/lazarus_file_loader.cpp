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

#include "../include/lazarus_file_loader.h"

/* =============================================
    Note: These definitions must be compiled as
    source / baked-in for stb libraries to work.
    I.e. they must be in scope of 1 source file
    and so can't be linked all over the place.
================================================ */

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
	#include <stb_image.h>
#endif

#ifndef STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
	#include <stb_image_resize.h>
#endif

FileLoader::FileLoader()
{
    LOG_DEBUG("Constructing Lazarus::FileLoader");

	this->imageData = 0;
    this->outResize = 0;
    this->resizeStatus = 0;
    
    this->enforceResize = false;
    this->maxWidth = 0;
    this->maxHeight = 0;

    this->outImage = {};
	this->imageHeight = 0;
	this->imageHeight = 0;
	this->channelCount = 0;	
};

lazarus_result FileLoader::relativePathToAbsolute(std::string filename, std::string &out) 
{
    this->path.clear();
    this->path = std::filesystem::absolute(filename);

    bool fileMounted = std::filesystem::exists(this->path);
    if(!fileMounted)
    {
        LOG_ERROR("Filesystem Error: ", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_FILE_NOT_FOUND;
    }
    out.append(this->path.string());

    return lazarus_result::LAZARUS_OK;
};

lazarus_result FileLoader::loadText(std::string filepath, std::string &out) 
{
    this->path.clear();
    if(std::filesystem::exists(filepath))
    {
        std::string absolutePath = "";
        lazarus_result status = this->relativePathToAbsolute(filepath, absolutePath);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };

        ifstream fileStream(absolutePath);

        if(fileStream.is_open())
        {
            this->stringstream << fileStream.rdbuf();
            out.append(stringstream.str());

            return lazarus_result::LAZARUS_OK;
        } 
        else 
        {
            LOG_ERROR("Filesystem Error:", __FILE__, __LINE__);

            return lazarus_result::LAZARUS_FILESTREAM_CLOSED;
        };
    }
    else 
    {
        LOG_ERROR("Filesystem Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_FILE_NOT_FOUND;
    };
};

lazarus_result FileLoader::loadImage(FileLoader::Image &out, const char *filename, const unsigned char *raw, uint32_t size, bool flipVert)
{
    this->imageData = {};
    this->outResize = {};
    
    if(raw == NULL && filename)
    {   
        /* ====================================================
            Images should be flipped on load due to the fact that 
            most file formats store the (x: 0.0, y: 0.0) coordinate
            at the top left (first pixel of first row), while 
            OpenGL's texture coordinate system stores it as the
            inverse - i.e. bottom left (first pixel, last row).

            It seems the exception to this rule are glb files
            (i.e. load_from_memory).
        ======================================================= */
        stbi_set_flip_vertically_on_load(flipVert);
        this->imageData = stbi_load(filename, &imageWidth, &imageHeight, &channelCount, 0);
    }
    else
    {
        stbi_set_flip_vertically_on_load(flipVert);
        /* =============================================================
            If the file has already been opened and read elsewhere in 
            the program, but has not yet been decoded.
        ================================================================ */
        this->imageData = stbi_load_from_memory(const_cast<stbi_uc*>(raw), size, &imageWidth, &imageHeight, &channelCount, 0);
    }

    if(imageData != NULL) 
    {
        this->enforceResize = GlobalsManager::getEnforceImageSanity();
        this->maxWidth = GlobalsManager::getMaxImageWidth();
        this->maxHeight = GlobalsManager::getMaxImageHeight();

        if(enforceResize == true && this->maxWidth > 0 && this->maxHeight > 0)
        {   
            /* ================================================= 
                Evil solution (the correct way):
            
                The return value of stbir_resize_uint8, unlike 
                stbi_load is reserved for error codes. This means 
                that to pass data into lazarus' tightly-packed byte 
                array (unsigned char *) it has to do so as a side-
                effect. To do so the memory has to be allocated 
                manually so that we can pass stbir a pointer to the 
                actual byte array.
            
                See: https://stackoverflow.com/a/65873156/23636614
            ==================================================== */
            outResize = (unsigned char *) malloc(this->maxWidth * this->maxHeight * channelCount);

            resizeStatus = stbir_resize_uint8(this->imageData, imageWidth, imageHeight, 0, outResize, this->maxWidth, this->maxHeight, 0, channelCount);

            if(resizeStatus == 1)
            {
                /* ===============================================
                    Resize success, send back result.
                ================================================== */

                out.pixelData = outResize;
                out.height = this->maxWidth;
                out.width = this->maxHeight;

                return lazarus_result::LAZARUS_OK;
            }
            else 
            {
                /* ================================================
                    Resize failed so send back the image as it was
                    when loaded.
                =================================================== */

                out.pixelData = this->imageData;
                out.height = imageHeight;
                out.width = imageWidth;

                LOG_ERROR("Filesystem Error:", __FILE__, __LINE__);

                return lazarus_result::LAZARUS_IMAGE_RESIZE_FAILURE;
            }

        }
        else
        {
            /* ==========================================
                Send back the data as-is, user doesn't
                want to resize.
            ============================================= */

            out.pixelData = this->imageData;
            out.height = imageHeight;
            out.width = imageWidth;

            return lazarus_result::LAZARUS_OK;
        }
    }
	else
	{
        /* ===============
            Load failed.
        ================== */

        out.pixelData = NULL;
        out.height = 0;
        out.width = 0;

        std::string message = std::string("Filesystem Error: ").append(stbi_failure_reason());
		LOG_ERROR(message.c_str(), __FILE__, __LINE__);

        return lazarus_result::LAZARUS_IMAGE_LOAD_FAILURE;
	};
};

FileLoader::~FileLoader()
{
    LOG_DEBUG("Destroying Lazarus::FileLoader");

	stbi_image_free(this->imageData);
    free(outResize);
};