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

        void loaderInit();
        int32_t loadTrueTypeFont(std::string filepath, uint32_t charHeight, uint32_t charWidth);
        FileLoader::Image loadCharacter(char character, uint32_t fontIndex);

        virtual ~FontLoader();

    private:
        void createBitmap();
        void flipGlyph();
        void setImageData(uint32_t width, uint32_t height, unsigned char *data);

        std::unique_ptr<FileLoader> fileReader;
        std::string absolutePath;
        
        uint8_t keyCode;
        
        uint32_t glyphIndex;
        
        FT_Matrix transformationMatrix;
        FT_Library lib;
        FT_Face fontFace;
        FT_Error status;
        std::vector<FT_Face> fontStack;
        
        GlobalsManager globals;
        FileLoader::Image image;
};

class TextManager : private FontLoader, private MeshManager
{
    public:
        TextManager(GLuint shader);

        struct Text
        {
            uint32_t layoutIndex;
            glm::vec2 location;
            std::string targetString;
            glm::vec3 color;
        };

        void initialise();
        uint32_t extendFontStack(std::string filepath, uint32_t ptSize = 12);
        Text loadText(std::string targetText, uint32_t fontId, glm::vec2 location, glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f), uint32_t letterSpacing = 1, Text textIn = {});
        void drawText(Text text);
        virtual ~TextManager();
        
        uint32_t fontCount;
        
        private: 
        Text textOut;
        void identifyAlphabetDimensions(uint32_t fontId);
        void setActiveGlyph(char target, uint32_t fontId, uint32_t spacing);
        void setTextColor(glm::vec3 color);
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
        
        GLuint textureId;
        GLuint shaderProgram;
        
        glm::vec3 textColor;
        
        Transform transformer;
        GlobalsManager globals;
        FileLoader::Image glyph;
        
        std::unique_ptr<CameraManager> cameraBuilder;
        
        MeshManager::Mesh quad;
        CameraManager::Camera camera;

        std::vector<MeshManager::Mesh> word;
        std::vector<uint32_t> alphabetHeights;
        std::vector<std::map<uint8_t, FileLoader::Image>> fonts;
        
        std::pair<uint32_t, std::vector<MeshManager::Mesh>> layoutEntry;
        std::map<uint32_t, std::vector<MeshManager::Mesh>> layout;
        std::map<uint8_t, FileLoader::Image> characters;
    
    };
    
    #endif
    