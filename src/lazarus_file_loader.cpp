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

FileLoader::FileLoader()
{
    std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
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

    this->textData = NULL;

};

string FileLoader::relativePathToAbsolute(string filename) 
{
    this->filenameString      =   std::filesystem::absolute(filename).string();                                              //  Find the absolute path from root (/) to the mesh asset and convert to std::string

    return this->filenameString;                                         //  Return the absolute path to the asset, exit the thread
};

const char *FileLoader::loadText(string filepath) 
{
    if(std::filesystem::exists(filepath))
    {
        this->absolutePath = this->relativePathToAbsolute(filepath);
        ifstream fileStream(absolutePath);

        if(fileStream.is_open())
        {
            this->stringstream << fileStream.rdbuf();

            this->contents = stringstream.str();
            this->textData = this->contents.c_str();

            return textData;
        } 
        else 
        {
            std::cout << RED_TEXT << "fileStream is not open" << RESET_TEXT << std::endl;
            globals.setExecutionState(LAZARUS_FILESTREAM_CLOSED);

            this->textData = std::to_string(LAZARUS_FILESTREAM_CLOSED).c_str();

            return this->textData;
        };
    }
    else 
    {
        std::cout << RED_TEXT << "File doesn't exist" << RESET_TEXT << std::endl;
        globals.setExecutionState(LAZARUS_FILE_NOT_FOUND);

        this->textData = std::to_string(LAZARUS_FILE_NOT_FOUND).c_str();

        return this->textData;
    };
};

FileLoader::Image FileLoader::loadImage(const char *filename, const unsigned char *raw, uint32_t size)
{
    this->imageData = {};
    this->outResize = {};
	
    /* ====================================================
        Images should be flipped on load due to the fact that 
        most file formats store the (x: 0.0, y: 0.0) coordinate
        at the top left (first pixel of first row), while 
        OpenGL's texture coordinate system stores it as the
        inverse - i.e. bottom left (first pixel, last row).
    ======================================================= */
    stbi_set_flip_vertically_on_load(true);

    /* =============================================================
        In the case that the file has already been opened and read 
        elsewhere in the program, but has not yet been decoded.
    ================================================================ */
    this->imageData = (raw == NULL)
    ? stbi_load(filename, &imageWidth, &imageHeight, &channelCount, 0)
    : stbi_load_from_memory(const_cast<stbi_uc*>(raw), size, &imageWidth, &imageHeight, &channelCount, 0);

    if(imageData != NULL) 
    {
        this->enforceResize = globals.getEnforceImageSanity();
        this->maxWidth = globals.getMaxImageWidth();
        this->maxHeight = globals.getMaxImageHeight();

        if(enforceResize == true)
        {
            if(this->maxWidth <= 0 || this->maxHeight <= 0)
            {
                std::cerr << RED_TEXT << "LAZARUS::ERROR::FileLoader::IMAGE_LOADER " << "Width and height must both have values higher than zero." << RESET_TEXT << std::endl;    
                globals.setExecutionState(LAZARUS_IMAGE_RESIZE_FAILURE);

                outImage.pixelData = NULL;
                outImage.height = 0;
                outImage.width = 0;

                return outImage;
            }

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
                outImage.pixelData = outResize;
                outImage.height = this->maxWidth;
                outImage.width = this->maxHeight;
            }
            else 
            {
                outImage.pixelData = this->imageData;
                outImage.height = imageHeight;
                outImage.width = imageWidth;

                std::cerr << RED_TEXT << "LAZARUS::ERROR::FileLoader::IMAGE_LOADER " << LAZARUS_IMAGE_RESIZE_FAILURE << RESET_TEXT << std::endl;    
                globals.setExecutionState(LAZARUS_IMAGE_RESIZE_FAILURE);
            }

        }
        else
        {
            outImage.pixelData = this->imageData;
            outImage.height = imageHeight;
            outImage.width = imageWidth;
        }
    }
	else
	{
        outImage.pixelData = NULL;
        outImage.height = 0;
        outImage.width = 0;

		std::cerr << RED_TEXT << "LAZARUS::ERROR::FileLoader::IMAGE_LOADER " << stbi_failure_reason() << RESET_TEXT << std::endl;
        globals.setExecutionState(LAZARUS_IMAGE_LOAD_FAILURE);
	};
	
	return outImage;
};

FileLoader::~FileLoader()
{
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
	stbi_image_free(this->imageData);
    free(outResize);
};