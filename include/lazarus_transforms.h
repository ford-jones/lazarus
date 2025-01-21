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
    #include "lazarus_gl_includes.h"
#endif

#include <memory>

#include "lazarus_mesh.h"
#include "lazarus_camera.h"
#include "lazarus_light.h"
#include "lazarus_globals_manager.h"

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
		void translateMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z);
    	void rotateMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z);
		void scaleMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z);

		void translateCameraAsset(CameraManager::Camera &camera, float x, float y, float z, float velocity = 0.1);
		void rotateCameraAsset(CameraManager::Camera &camera, float x, float y, float z);

		void translateLightAsset(LightManager::Light &light, float x, float y, float z);

	private:
		float determineUpVector(float rotation);
		bool determineIsSigned(float x, float y, float z);
		float up;

		glm::vec3 temp;

		glm::vec3 localCoordinates;
		glm::vec4 worldCoordinates;

		GlobalsManager globals;

};

#endif
