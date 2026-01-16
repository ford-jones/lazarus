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

#ifndef LAZARUS_COMMON_H
	#include "lazarus_common.h"
#endif

#include <memory>
#include <algorithm>

#include "lazarus_mesh.h"
#include "lazarus_camera.h"
#include "lazarus_light.h"

using glm::vec3;
using std::shared_ptr;

#ifndef LAZARUS_TRANSFORMS_H
#define LAZARUS_TRANSFORMS_H

/* =====================================================
	TODO:
	1. Light rotation (For use with directional lights)
======================================================== */

class Transform
{	
    public:
		Transform();
		lazarus_result translateMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z, uint32_t matrixId = 0);
    	lazarus_result rotateMeshAsset(MeshManager::Mesh &mesh, float pitch, float yaw, float roll, uint32_t matrixId = 0);
		lazarus_result scaleMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z, uint32_t matrixId = 0);

		lazarus_result translateCameraAsset(CameraManager::Camera &camera, float x, float y, float z, float velocity = 1.0);
		lazarus_result rotateCameraAsset(CameraManager::Camera &camera, float pitch, float yaw, float roll);
		lazarus_result orbitCameraAsset(CameraManager::Camera &camera, float azimuth, float elevation, float radius, float tarX = 0.0, float tarY = 0.0, float tarZ = 0.0);

		lazarus_result translateLightAsset(LightManager::Light &light, float x, float y, float z);

		virtual ~Transform();
		
	private:
		float determineUpVector(float rotation);
		float degreesToRadians(float in, bool enforceLimits = true);

		double pi;
		float outRadians;
		float up;

		float accumulatePitch;
		float accumulateYaw;
		float accumulateRoll;
		
		glm::vec3 rotation;

		glm::vec3 localCoordinates;
		glm::vec4 worldCoordinates;
};

#endif
