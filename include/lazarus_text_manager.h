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
//                                        
/*  LAZARUS ENGINE */

#ifndef LAZARUS_GL_INCLUDES_H
    #include "lazarus_gl_includes.h"
#endif

#ifndef LAZARUS_COMMON_H
	#include "lazarus_common.h"
#endif

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <ft2build.h>

#include "lazarus_shader.h"
#include "lazarus_camera.h"
#include "lazarus_transforms.h"
#include "lazarus_mesh.h"
#include "lazarus_file_loader.h"

#include FT_FREETYPE_H

#ifndef LAZARUS_TEXT_MANAGER_H
#define LAZARUS_TEXT_MANAGER_H

class FontLoader
{
    public:
        FontLoader();

        lazarus_result loaderInit();
        lazarus_result loadTrueTypeFont(uint32_t &index, std::string filepath, uint32_t ptSize);
        lazarus_result loadCharacter(FileLoader::Image &glyph, char character, uint32_t fontIndex);

        virtual ~FontLoader();

    private:
        lazarus_result createBitmap();
        void flipGlyph();
        void setImageData(uint32_t width, uint32_t height, unsigned char *data);

        std::unique_ptr<FileLoader> fileReader;
        std::string absolutePath;
        
        uint8_t keyCode;
        
        uint32_t glyphIndex;
        
        FT_Matrix transformationMatrix;
        FT_Library lib;
        FT_Face fontFace;
        FT_Error result;
        std::vector<FT_Face> fontStack;
        
        FileLoader::Image image;
};

//  TODO:
//  Add functionality for scaling text / adjusting ptsize

class TextManager : private FontLoader, private ModelManager
{
    public:
        TextManager(GLuint shader);

        struct TextConfig
        {
            uint32_t fontIndex = 0;
            uint32_t letterSpacing = 1;
            glm::vec2 location = vec2(0.0f, 0.0f);
            std::string targetString = "";
            glm::vec3 color = vec3(0.0f, 0.0f, 0.0f);            
        };

        struct Text
        {
            uint32_t layoutIndex;
            TextConfig config;
        };

        lazarus_result initialise();
        lazarus_result extendFontStack(uint32_t &fontId, std::string filepath, uint32_t ptSize = 12);
        
        lazarus_result createText(Text &out, TextConfig options);
        lazarus_result loadText(Text textIn);
        lazarus_result drawText(Text textIn);
        
        virtual ~TextManager();

        uint32_t fontCount;
        
        private: 
        Text textOut;
        lazarus_result identifyAlphabetDimensions(uint32_t fontId);
        void setActiveGlyph(char target, uint32_t fontId, uint32_t spacing);
        void lookUpUVs(uint8_t keyCode, uint32_t fontId);
        
        uint8_t targetKey;
        
        uint32_t rowWidth;
        uint32_t rowHeight;
        uint32_t atlasWidth;
        uint32_t atlasHeight;
        
        uint32_t fontIndex;
        uint32_t layoutIndex; 
        
        float translationStride;
        
        float uvL;
        float uvR;
        float uvU;
        float uvD;
        
        GLuint textColorUniformLocation;

        GLuint textureId;
        GLuint shaderProgram;
        
        Transform transformer;
        FileLoader::Image glyph;
        
        std::unique_ptr<CameraManager> cameraBuilder;
        
        ModelManager::Model quad;
        CameraManager::Camera camera;

        std::vector<ModelManager::Model> word;
        std::vector<uint32_t> alphabetHeights;
        std::vector<std::map<uint8_t, FileLoader::Image>> fonts;
        
        std::pair<uint32_t, std::vector<ModelManager::Model>> layoutEntry;
        std::map<uint32_t, std::vector<ModelManager::Model>> layout;
        std::map<uint8_t, FileLoader::Image> characters;
    };
    
#endif