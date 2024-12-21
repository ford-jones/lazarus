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

#include "../include/lazarus_text_manager.h"

    /* =======================================================
        TODO:
        - Concatenate any additional texture atlas's produced by
        this function into the existing atlas in memory at a 
        y-offset equal to the textureId.

        - Make draw call param optional. If it isn't present 
        the entire layout should be drawn.
    ========================================================== */

TextManager::TextManager(GLuint shader)
{
    std::cout << GREEN_TEXT << "Calling constructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
    this->shaderProgram = shader;
    this->meshLoader = nullptr;
    this->cameraBuilder = nullptr;
    
    this->textureLoader = nullptr;
    this->fontLoader = nullptr;
    this->word = {};

    this->translation = 0;
    this->span = 0;
    this->targetKey = 0;
    this->targetXL = 0;
    this->targetXR = 0;

    this->textColor = glm::vec3(0.0f, 0.0f, 0.0f);

    this->fontIndex = 0;
    this->layoutIndex = 0;

    this->atlasX = 0;
    this->atlasY = 0;

    this->monitorWidth = 0.0;

    this->uvL = 0.0;
    this->uvR = 0.0;
    this->uvH = 0.0;
};

int TextManager::extendFontStack(std::string filepath, int ptSize)
{
    /* ====================================================
        The name of this function is slightly deceptive.
        This function extends new glyphs into the existing
        alphabet texture atlas along the images' x-axis.
        Essentially adding new columns.

        In concept, it will be more like a stack when a new
        font is added, a new row (on the y-axis) should be 
        created. In memory they will all sit on top each-
        other in one big texture atlas and so in that way
        it will be like a stack - currently it's just a
        single row.
    ======================================================= */
    this->textureLoader = std::make_unique<TextureLoader>();
    this->fontLoader = std::make_unique<FontLoader>();
    
    fontLoader->initialise();

    this->fontIndex = fontLoader->loadFont(filepath, ptSize, 0);

    this->identifyAlphabetDimensions();

    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureAtlas"), 1);
    textureLoader->storeBitmapTexture(atlasX, atlasY);

    this->textureId = textureLoader->bitmapTexture;
    /* ===========================================================================
        The expression (n - 33) AKA (i = 33 && i < 128) occurs in several places 
        and is used to skip control keys as well as calculate the span offset for 
        non-control characters (i.e. keycodes which don't have a unicode UTF-8 
        glyph associated with them. e.g. shift / ctrl).
    ============================================================================== */
    for(int i = 33; i < 128; i++)
    {
        glyph = fontLoader->loadCharacter(static_cast<char>(i), fontIndex);
        textureLoader->loadBitmapToTexture(glyph);

        textures.emplace((i - 33), glyph);
    };

    return fontIndex;
};

int TextManager::loadText(std::string targetText, int posX, int posY, int letterSpacing, float red, float green, float blue, int layoutID)
{
    /* =================================================
        Dereference and re-assign these on each call so
        as to reduce their internal child trackers from
        bloating.
    ==================================================== */
    this->meshLoader = std::make_unique<MeshManager>(this->shaderProgram);
    this->cameraBuilder = std::make_unique<CameraManager>(this->shaderProgram);

    if(word.size() > 0)
    {
        this->word.clear();
    };
    
    this->setTextColor(red, green, blue);

    for(unsigned int i = 0; i < targetText.size(); i++)
    {   
        this->setActiveGlyph(targetText[i], letterSpacing);

        quad = meshLoader->createQuad(static_cast<float>(this->glyph.width), static_cast<float>(this->atlasY), LAZARUS_MESH_ISTEXT, this->uvL, this->uvR, this->uvH);
        
        quad.isGlyph = 1;
        quad.textureId = this->textureId;
        quad.textureLayer = 0;
        quad.textureData = this->glyph;
        quad.textureUnit = GL_TEXTURE1;

        transformer.translateMeshAsset(quad, static_cast<float>(posX + this->translation), static_cast<float>(posY), 0.0f);
        this->translation += (this->glyph.width + letterSpacing);

        this->word.push_back(quad);
    };

    /* =================================================
        The layoutID argument exists so that the user  
        can specify an existing layout object to 
        update / replace instead of creating anew. This 
        is useful for text which needs to be updated 
        inside the render loop - for instance FPS, 
        entity coordinates or any other string which 
        might update on-the-fly.
    ==================================================== */
    if(layoutID != -1)
    {
        layout.erase(layoutID);
        layout.insert_or_assign(layoutID, this->word);
        
        this->translation = 0;
        
        return layoutID;
    }
    else
    {
        this->layoutIndex += 1;

        this->layoutEntry = std::pair<int, std::vector<MeshManager::Mesh>>(this->layoutIndex, this->word);
        layout.insert(this->layoutEntry);

        this->translation = 0;

        return this->layoutIndex;
    };
};

void TextManager::drawText(int layoutIndex)
{
    /* ===============================================
        Unlike other mesh assets (i.e. 3D mesh or 
        sprites), quads which are wrapped with a glyph
        texture are rendered using an orthographic 
        projection matrix which is overlain over the 
        perspective projection matrix. This is done by 
        saving glyph draw calls till last, prior to 
        swapping the front and back buffers. This 
        gives the effect of 2D / HUD text.
    ================================================== */
    this->camera = cameraBuilder->createOrthoCam(globals.getDisplayWidth(), globals.getDisplayHeight());
    
    this->word = layout.at(layoutIndex);

    for(auto i: this->word)
    {
        quad = i;

        if((quad.modelMatrixUniformLocation >= 0) && (camera.projectionLocation >= 0))
        {            
            cameraBuilder->loadCamera(camera);

            meshLoader->loadMesh(quad);
            meshLoader->drawMesh(quad);
            continue;
        }
        else
        {
            globals.setExecutionState(LAZARUS_UNIFORM_NOT_FOUND);
            break;
        };
    };

    return;
};

void TextManager::identifyAlphabetDimensions()
{
    /* =====================================================
        Glyphs will be loaded into the texture atlas in a 
        continuous line. The height of this line is equal to 
        the tallest glyph bitmap and the width is equal to 
        the culmilative width of each glyph's bitmap.
    ======================================================== */
    for(int i = 33; i < 128; i++)
    {
        glyph = fontLoader->loadCharacter(static_cast<char>(i), fontIndex);
        
        atlasX += glyph.width;
        atlasY = std::max(atlasY, glyph.height);
    };

    return;
};

void TextManager::setActiveGlyph(char target, int spacing)
{
    if(target == ' ')
    {
        this->uvL = -1.0f;
        this->uvR = -1.0f;
        this->uvH = -1.0f;

        this->glyph = {pixelData: NULL, height: 0, width: (spacing * 8)};
    }
    else
    {
        this->targetKey = static_cast<int>(target);

        this->lookUpUVs(this->targetKey);
        this->glyph = textures.at((this->targetKey - 33));
    };
};

void TextManager::setTextColor(float r, float g, float b)
{
    this->textColor = glm::vec3(r, g, b);
    glUniform3fv(glGetUniformLocation(this->shaderProgram, "textColor"), 1, &this->textColor[0]);
};

void TextManager::lookUpUVs(int keyCode)
{
    this->span = keyCode - 33;
    this->targetXL = 0;

    /* ===========================================================================
        Calculate L / R / T / B UV coordinates / where the glyph is located in
        the alphabet texture atlas. Divide by primary monitor dimensions to get 
        normalised value.
    ============================================================================== */
    for (int i = 0; i < span; ++i)
    {
        this->glyph = textures.at(i);
        this->targetXL += this->glyph.width;
    }

    this->glyph = textures.at(span);
    this->targetXR = targetXL + this->glyph.width;

    this->monitorWidth = static_cast<float>(atlasX);
    
    this->uvL = static_cast<float>(this->targetXL) / monitorWidth;
    this->uvR = static_cast<float>(this->targetXR) / monitorWidth;
    this->uvH = static_cast<float>(this->fontIndex);

    this->targetXL = 0;
    this->targetXR = 0;
};

TextManager::~TextManager()
{
        std::cout << GREEN_TEXT << "Calling destructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
};