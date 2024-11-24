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
#ifndef LAZARUS_GL_INCLUDES_H
    #include "lz_gl_includes.h"
#endif

#include <memory>

#include "lz_mesh.h"
#include "lz_camera.h"
#include "lz_light.h"
#include "lz_globals_manager.h"

using glm::vec3;
using std::shared_ptr;

#ifndef LAZARUS_TRANSFORMS_H
#define LAZARUS_TRANSFORMS_H

/* =====================================================
	TODO:
	1. Scaling transforms
	2. Light rotation (For use with directional lights)
======================================================== */

class Transform
{	
    public:
		Transform();
		void translateMeshAsset(Mesh::TriangulatedMesh &mesh, float x, float y, float z);
    	void rotateMeshAsset(Mesh::TriangulatedMesh &mesh, float x, float y, float z);

		void translateCameraAsset(Camera::FixedCamera &camera, float x, float y, float z);
		void rotateCameraAsset(Camera::FixedCamera &camera, float x, float y, float z);

		void translateLightAsset(Light::AmbientLight &light, float x, float y, float z);

	private:
		float determineUpVector(float rotation);
		float up;

		glm::vec3 temp;

		glm::vec3 localCoordinates;
		glm::vec4 worldCoordinates;

		GlobalsManager globals;

};

#endif