#version 410 core

#define MAX_LIGHTS 150

in vec3 fragPosition;
in vec3 diffuseColor;
in vec3 normalCoordinate;
in vec3 textureCoordinate;
in vec3 skyBoxTextureCoordinate;

flat in int isUnderPerspective;

uniform int lightCount;
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightBrightness[MAX_LIGHTS];

uniform vec3 textColor;

uniform int spriteAsset;
uniform int glyphAsset;
uniform int isSkyBox;

uniform float textureLayer;

uniform sampler2D textureAtlas;
uniform sampler2DArray textureArray;
uniform samplerCube textureCube;

out vec4 outFragment;

//  Illuminate the fragment using the lambertian lighting model
vec3 calculateLambertianDeflection (vec4 colorData, vec3 lightPosition, vec3 lightColor) 
{
    vec3 displacement = lightPosition - fragPosition;
    vec3 direction = normalize(displacement);
    float diffusion = max(dot(normalCoordinate, direction), 0.0);
    vec3 color = vec3(colorData.r, colorData.g, colorData.b);
    vec3 illuminatedFrag = (color * lightColor * diffusion);
    
    //  Apply inverse square law to illumination result
    //  Note: Don't apply for directional lights when they are added
    //  This is better maybe: return illuminatedFrag * min(1.0 / pow(dot(displacement, displacement), 0.5), 1.0);
    return illuminatedFrag / (dot(displacement, displacement));
}

vec4 interpretColorData ()
{
    //  rgb with values less than 0 indicate the fragment has no texture and should use diffuse coloring
    //  Otherwise its a texture so pick out the texels from the appropriate sampler

    if((diffuseColor.r >= 0.0) &&
       (diffuseColor.g >= 0.0) && 
       (diffuseColor.b >= 0.0))
    {
        return vec4(diffuseColor, 1.0);
    }
    else 
    {
        if( spriteAsset == 1 )
        {
            vec4 tex = texture(textureArray, vec3(textureCoordinate.xy, textureLayer));
            return tex;
        } 
        else if( glyphAsset == 1)
        {
            vec4 tex = texture(textureAtlas, textureCoordinate.xy);
            
            vec4 sampled = vec4(1.0, 1.0, 1.0, tex.r);
            vec4 text = vec4(textColor, 1.0) * sampled;

            if(text.a < 0.1)
            {
                discard;
            }

            return text;           
        }
        else if( isSkyBox == 1 )
        {
            vec4 tex = texture(textureCube, skyBoxTextureCoordinate);
            return tex;
        }
        else
        {
            vec4 tex = texture(textureArray, vec3(textureCoordinate.xy, textureLayer));

            if(tex.a < 0.1)
            {
                discard;
            }

            return tex;
        } 

    }
}

void main ()
{
    vec4 fragColor = interpretColorData();
            
    //  When the fragment is part of a skybox or is observed by an orthographic camera, use color as-is.
    if(isSkyBox == 1 || isUnderPerspective == 0)
    {
        outFragment = fragColor;
    }
    else
    {
        vec3 illuminationResult = 0.1 * fragColor.rgb;

        //  Calculate the fragment's diffuse lighting for each light in the scene.
        for(int i = 0; i < lightCount; i++)
        {
            illuminationResult += (calculateLambertianDeflection(fragColor, lightPositions[i], lightColors[i]) * lightBrightness[i]);
        };

        outFragment = vec4(illuminationResult, 1.0);
    }

    return;
}