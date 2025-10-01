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

FontLoader::FontLoader()
{
    LOG_DEBUG("Constructing Lazarus::FontLoader");

    this->fileReader = nullptr;

    this->lib = NULL;
    this->fontFace = NULL;
    this->result = 0;

    this->fontStack = {};

    this->keyCode = 0;
    
    this->setImageData(0, 0, NULL);
};

lazarus_result FontLoader::loaderInit()
{
    result = FT_Init_FreeType(&lib);

    if(result != FT_Err_Ok)
    {
        std::string message = std::string("FontLoader Error: ").append(std::to_string(this->result));
        LOG_ERROR(message.c_str(), __FILE__, __LINE__);

        return lazarus_result::LAZARUS_FT_INIT_FAILURE;
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result FontLoader::loadTrueTypeFont(uint32_t &index, std::string filepath, uint32_t ptSize)
{
    this->absolutePath.clear();
    fileReader = std::make_unique<FileLoader>();

    lazarus_result status = fileReader->relativePathToAbsolute(filepath, this->absolutePath);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    result = FT_New_Face(lib, this->absolutePath.c_str(), 0, &fontFace);

    if(result != FT_Err_Ok)
    {
        std::string message = std::string("FontLoader Error: ").append(std::to_string(this->result));
        LOG_ERROR(message.c_str(), __FILE__, __LINE__);

        return lazarus_result::LAZARUS_FILE_UNREADABLE;
    } 
    else 
    {
        FT_Set_Pixel_Sizes(fontFace, 0, ptSize);

        fontStack.push_back(fontFace);

        index = fontStack.size();
        return lazarus_result::LAZARUS_OK;
    }
};

lazarus_result FontLoader::loadCharacter(FileLoader::Image &glyph, char character, uint32_t fontIndex)
{
    this->fontFace = fontStack[fontIndex - 1];
    this->keyCode = static_cast<uint8_t>(character);

    /* ====================================================================
        The glyph needs to be rotated on load, prior to being rendered to a
        bitmap. When rendered; each glyph will then be aligned on the 
        bottom of it's bounding box rather than the default behaviour which 
        aligns glyphs along the top.

        This means the texture will be upside-down when it's loaded into
        VRAM / passed into OpenGL. As a result, the quad the texture is 
        mapped to will also have to be rotated.

        If you need to see what I mean, disable the following line and use
        renderDoc to inspect the texture once it's on the GPU.
    ======================================================================= */
    this->flipGlyph();

    this->glyphIndex = FT_Get_Char_Index(fontFace, keyCode);
    result = FT_Load_Glyph(fontFace, glyphIndex, FT_LOAD_DEFAULT);

    if(result != FT_Err_Ok)
    {
        std::string message = std::string("FontLoader Error: ").append(std::to_string(this->result));
        LOG_ERROR(message.c_str(), __FILE__, __LINE__);

        this->setImageData(0, 0, NULL);
        return lazarus_result::LAZARUS_FT_LOAD_FAILURE;
    }
    else
    {
        lazarus_result status = this->createBitmap();
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
    }

    glyph = this->image;
    return lazarus_result::LAZARUS_OK;
};

lazarus_result FontLoader::createBitmap()
{
    result = FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL);

    if(result != FT_Err_Ok)
    {
        std::string message = std::string("FontLoader Error: ").append(std::to_string(this->result));
        LOG_ERROR(message.c_str(), __FILE__, __LINE__);
        
        this->setImageData(0, 0, NULL);
        
        return lazarus_result::LAZARUS_FT_RENDER_FAILURE;
    }
    else
    {
        this->setImageData(
            fontFace->glyph->bitmap.width, 
            fontFace->glyph->bitmap.rows, 
            fontFace->glyph->bitmap.buffer
        );

        return lazarus_result::LAZARUS_OK;
    };
};

void FontLoader::setImageData(uint32_t width, uint32_t height, unsigned char *data)
{
    this->image.width = width;
    this->image.height = height;
    this->image.pixelData = data;

    return;
};

void FontLoader::flipGlyph()
{
    FT_Vector pixelStore;

    pixelStore.x = 1 * 64;
    pixelStore.y = 1 * 64;

    /* =========================================
        Construct a transformation matrix used to
        flip the glyph vertically. This is to 
        accomidate OpenGL's cartesian coordinate
        system (0.0 being bottom left instead of top 
        left).
    ============================================ */
    transformationMatrix.xx = (FT_Fixed)( cos( 0.0f ) * 0x10000L );
    transformationMatrix.xy = (FT_Fixed)(-sin( 0.0f ) * 0x10000L );
    transformationMatrix.yx = (FT_Fixed)( sin( 0.0f ) * 0x10000L );
    transformationMatrix.yy = (FT_Fixed)( cos( 180.0f ) * 0x10000L );

    FT_Set_Transform( fontFace, &transformationMatrix, &pixelStore);

    /* ==========================================
        Advance the pen / cursor / locator to the 
        end of the active glyph's coordinates to 
        be properly located for next glyph load.
    ============================================= */
    pixelStore.x += fontFace->glyph->advance.x;
    pixelStore.y += fontFace->glyph->advance.y;

    return;
};

FontLoader::~FontLoader()
{
    LOG_DEBUG("Destroying Lazarus::FontLoader");

    for(size_t i = 0; i < fontStack.size(); i++)
    {
        this->fontFace = fontStack[i];
        FT_Done_Face(this->fontFace);
    };
    FT_Done_FreeType(this->lib);
};

TextManager::TextManager(GLuint shader) 
    : TextManager::FontLoader(),
      TextManager::MeshManager(shader, TextureLoader::StorageType::ATLAS)
{
    LOG_DEBUG("Constructing Lazarus::TextManager");

    this->shaderProgram = shader;
    this->textColorUniformLocation = glGetUniformLocation(this->shaderProgram, "textColor");
    this->cameraBuilder = std::make_unique<CameraManager>(this->shaderProgram);
    
    this->textOut = {};
    this->word = {};
    this->characters = {};
    this->fonts = {};
    this->alphabetHeights = {};

    this->translationStride = 0;

    this->targetKey = 0;
    this->fontIndex = 0;
    this->layoutIndex = 0;

    this->rowWidth = 0;
    this->rowHeight = 0;

    this->atlasWidth = 0;
    this->atlasHeight = 0;

    this->uvL = 0.0;
    this->uvR = 0.0;
    this->uvU = 0.0;
    this->uvD = 0.0;
};

lazarus_result TextManager::initialise()
{
    this->camera = {};
    CameraManager::CameraConfig config = {};

    config.aspectRatioX = GlobalsManager::getDisplayWidth();
    config.aspectRatioY = GlobalsManager::getDisplayHeight();

    lazarus_result status = FontLoader::loaderInit();
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

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

    return cameraBuilder->createOrthoCam(camera, config);
};

lazarus_result TextManager::extendFontStack(uint32_t &fontId, std::string filepath, uint32_t ptSize)
{
    fonts.clear();
    alphabetHeights.clear();
    
    lazarus_result status = FontLoader::loadTrueTypeFont(this->fontIndex, filepath, ptSize);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    
    /* ===========================================================================
        The expression (n - 33) AKA (i = 33 && i < 128) occurs in several places 
        and is used to skip control keys as well as calculate the offset for 
        non-control characters (i.e. keycodes which don't have a unicode UTF-8 
        glyph associated with them. e.g. shift / ctrl).
    ============================================================================== */

    rowWidth = 0;
    rowHeight = 0;
    atlasWidth = 0;
    atlasHeight = 0;

    uint32_t yOffset = 0;
    uint32_t xOffset = 0;
    
    /* ============================================
        Provision storage for known aphabets.
        
        I.e.
        texture atlas maximum width and height 
        equal to the width of each char in the widest
        font and the sum of the heights of each font's
        tallest char.
    =============================================== */
    for(size_t n = 0; n < this->fontIndex; n++)
    {   
        rowHeight = 0;
        yOffset = this->atlasHeight;
        alphabetHeights.push_back(yOffset);

        status = this->identifyAlphabetDimensions(n + 1);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };

        if(rowWidth > atlasWidth)
        {
            this->atlasWidth = rowWidth;
        }

        this->atlasHeight += rowHeight;
        alphabetHeights.push_back(this->atlasHeight);
        
        status = MeshManager::TextureLoader::storeBitmapTexture(atlasWidth, atlasHeight);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
    }
    
    /* =============================================
        Load each character of each font to it's
        respective location within the texture atlas.
    ================================================ */
    for(size_t n = 0; n < this->fontIndex; n++)
    {
        characters.clear();

        yOffset = alphabetHeights[n * 2];
        xOffset = 0;
        
        /* ==========================================
            +1 for zero-exclusive keycode indexing
        ============================================= */
        
        for(uint32_t i = 33; i < INT8_MAX + 1; i++)
        {
            status = FontLoader::loadCharacter(this->glyph, static_cast<char>(i), (n + 1));
            if(status != lazarus_result::LAZARUS_OK)
            {
                return status;
            };

            status = MeshManager::TextureLoader::loadBitmapToTexture(this->glyph, xOffset, yOffset);
            if(status != lazarus_result::LAZARUS_OK)
            {
                return status;
            };

            xOffset += this->glyph.width;
    
            characters.emplace((i - 33), this->glyph);
        };
        fonts.push_back(characters);
    };

    fontId = fonts.size() - 1;
    return lazarus_result::LAZARUS_OK;
};

lazarus_result TextManager::createText(TextManager::Text &out, TextManager::TextConfig options)
{
    this->layoutIndex += 1;

    textOut.layoutIndex = this->layoutIndex;
    textOut.config = options;
    
    this->layoutEntry = std::pair<int, std::vector<MeshManager::Mesh>>(this->layoutIndex, {});
    this->layout.insert(this->layoutEntry);

    lazarus_result status = this->loadText(textOut);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    out = this->textOut;
    return lazarus_result::LAZARUS_OK;
};

lazarus_result TextManager::loadText(TextManager::Text textIn)
{
    lazarus_result status = lazarus_result::LAZARUS_OK;

    TextManager::TextConfig &settings = textIn.config;
    
    /* =================================================
        Creation of these tiles takes up space in the
        manager's dataStore and will grow indefinitely
        if not flushed out. 

        Also helps ensure the location of the tiles at
        the time of draw. I.e. index 0 .. n
    ==================================================== */
    
    MeshManager::clearMeshStorage();

    if(word.size() > 0)
    {
        this->word.clear();
    };

    this->translationStride = 0;
    
    /* ==========================================
        Generate new tiles which are used as 
        texture surfaces for a letter. 
        
        Letters are identified by their UV 
        coordinates from within the glyph atlas.
    ============================================= */

    for(size_t i = 0; i < textIn.config.targetString.size(); i++)
    {   
        this->setActiveGlyph(settings.targetString[i], settings.fontIndex, settings.letterSpacing);

        MeshManager::QuadConfig quadSettings = {};
        quadSettings.width = this->glyph.width;
        quadSettings.height = this->glyph.height;
        quadSettings.texturePath = "";
        quadSettings.uvXL = this->uvL;
        quadSettings.uvXR = this->uvR;
        quadSettings.uvYU = this->uvU;
        quadSettings.uvYD = this->uvD;

        status = MeshManager::createQuad(this->quad, quadSettings);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
        /* =============================================
            Translate the tile to it's location within
            the word, relative to the other letters.
        ================================================ */
        status = transformer.translateMeshAsset(
            quad, 
            static_cast<float>((settings.location.x + (this->glyph.width / 2.0f)) + this->translationStride), 
            static_cast<float>((settings.location.y + (this->rowHeight / 2.0f))), 
            0.0f
        );
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };

        this->translationStride += (this->glyph.width + settings.letterSpacing);

        this->word.push_back(quad);
    };

    /* ============================================
        Cleanup the layout at the locaiton of an
        entry and assume it's position with the new
        set of tiles.
    =============================================== */

    layout.erase(textIn.layoutIndex);
    layout.insert_or_assign(textIn.layoutIndex, this->word);
        
    this->translationStride = 0;

    //  TODO:
    //  Check opengl errors 
    
    glUniform3fv(this->textColorUniformLocation, 1, &textIn.config.color[0]);

    return status;
};

lazarus_result TextManager::drawText(TextManager::Text textIn)
{    
    this->word = layout.at(textIn.layoutIndex);

    //  TODO:
    //  Check errors

    for(auto i: this->word)
    {
        quad = i;
        
        cameraBuilder->loadCamera(camera);
        MeshManager::loadMesh(quad);
        MeshManager::drawMesh(quad);
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result TextManager::identifyAlphabetDimensions(uint32_t fontId)
{
    /* =====================================================
        Glyphs will be loaded into the texture atlas in a 
        continuous line. The height of this line is equal to 
        the tallest glyph bitmap and the width is equal to 
        the culmilative width of each glyph's bitmap.
    ======================================================== */
    this->rowWidth = 0;
    this->rowHeight = 0;

    for(uint8_t i = 33; i < INT8_MAX + 1; i++)
    {
        lazarus_result status = FontLoader::loadCharacter(this->glyph, static_cast<char>(i), fontId);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
        
        rowWidth += glyph.width;
        if(glyph.height > rowHeight)
        {
            rowHeight = glyph.height;
        };
    };

    return lazarus_result::LAZARUS_OK;
};

void TextManager::setActiveGlyph(char target, uint32_t fontId, uint32_t spacing)
{
    /* =====================================================
        If the active glyph is a space; set the UV coordinates
        to be outside of the texture atlas. That way the 
        sampled area of the atlas will contain nothing.
    ======================================================== */

    if(target == ' ')
    {
        this->uvL = -1.0f;
        this->uvR = -1.0f;
        this->uvU = -1.0f;
        this->uvD = -1.0f;

        this->glyph.pixelData = NULL;
        this->glyph.height = 0;
        this->glyph.width = (spacing * 8);
    } 
    else 
    {
        this->targetKey = static_cast<int>(target);

        this->lookUpUVs(this->targetKey, fontId);
        this->characters = this->fonts.at(fontId);
        this->glyph = this->characters.at((this->targetKey - 33));
    };

    return;
};

void TextManager::lookUpUVs(uint8_t keyCode, uint32_t fontId)
{
    this->characters = this->fonts.at(fontId);
    uint8_t range = keyCode - 33;
    uint32_t targetXL = 0;
    uint32_t targetXR = 0;

    /* ===========================================================================
        Calculate L / R / T / B UV coordinates / where the glyph is located in
        the alphabet texture atlas. Divide by primary monitor dimensions to get 
        normalised value.
    ============================================================================== */
    for(uint8_t i = 0; i < range; ++i)
    {
        this->glyph = characters.at(i);
        targetXL += this->glyph.width;
    };

    this->glyph = characters.at(range);
    targetXR = targetXL + this->glyph.width;

    float uvRangeX = static_cast<float>(this->atlasWidth);
    float uvRangeY = static_cast<float>(this->atlasHeight);
    
    /* ===================================================
        Normalise the values of the pixel locations within
        the dimensions of the texture atlas.
    ====================================================== */
    this->uvL = static_cast<float>(targetXL) / uvRangeX;
    this->uvR = static_cast<float>(targetXR) / uvRangeX;
    this->uvU = static_cast<float>(this->alphabetHeights[(fontId * 2) + 1]) / uvRangeY;
    this->uvD = static_cast<float>(this->alphabetHeights[(fontId * 2)]) / uvRangeY;

    return;
};

TextManager::~TextManager()
{
    LOG_DEBUG("Destroying Lazarus::TextManager");
};