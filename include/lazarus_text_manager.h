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
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include <ft2build.h>
#include <math.h>

#include "lazarus_shader.h"
#include "lazarus_camera.h"
#include "lazarus_texture_loader.h"
#include "lazarus_transforms.h"
#include "lazarus_mesh.h"

#include FT_FREETYPE_H

#ifndef LAZARUS_TEXT_MANAGER_H
#define LAZARUS_TEXT_MANAGER_H

class FontLoader
{
    public:
        FontLoader();

        void loaderInit();
        int loadTrueTypeFont(std::string filepath, int charHeight, int charWidth);
        FileReader::Image loadCharacter(char character, int fontIndex);

        virtual ~FontLoader();

    private:
        void createBitmap();
        void flipGlyph();
        void setImageData(int width, int height, unsigned char *data);

        GlobalsManager globals;
        FileReader::Image image;
        std::vector<FT_Face> fontStack;

        int keyCode;

        FT_Matrix transformationMatrix;
        FT_Library lib;
        FT_Face fontFace;

        unsigned int glyphIndex;

        FT_Error status;

        std::unique_ptr<FileReader> fileReader;
        std::string absolutePath;
};

class TextManager : public FontLoader
{
    public:
        TextManager(GLuint shader);
        int extendFontStack(std::string filepath, int ptSize = 12);
        int loadText(std::string targetText, int posX, int posY, int letterSpacing = 1, float red = 0.0f, float green = 0.0f, float blue = 0.0f, int layoutID = -1);
        void drawText(int layoutIndex = 0);
        virtual ~TextManager();

    private: 
        void identifyAlphabetDimensions();
        void setActiveGlyph(char target, int spacing);
        void setTextColor(float r, float g, float b);
        void lookUpUVs(int keyCode);

        int translationStride;
        int targetKey;
        int targetXL;
        int targetXR;
        int span;

        int atlasX;
        int atlasY;

        float monitorWidth;

        float uvL;
        float uvR;
        float uvH;

        unsigned int fontIndex;
        unsigned int layoutIndex;

        GLuint textureId;
        GLuint shaderProgram;

        glm::vec3 textColor;

        Transform transformer;
        GlobalsManager globals;
        FileReader::Image glyph;

        std::unique_ptr<MeshManager> meshLoader;
        std::unique_ptr<TextureLoader> textureLoader;
        // std::unique_ptr<FontLoader> fontLoader;
        std::unique_ptr<CameraManager> cameraBuilder;

        MeshManager::Mesh quad;
        CameraManager::Camera camera;

        std::vector<MeshManager::Mesh> word;
        
        std::map<int, FileReader::Image> textures;
        std::map<int, std::vector<MeshManager::Mesh>> layout;
        std::pair<int, std::vector<MeshManager::Mesh>> layoutEntry;
};

#endif
