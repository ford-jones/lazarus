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
		void translateMeshAsset(MeshManager::Mesh &mesh, _Float32 x, _Float32 y, _Float32 z);
    	void rotateMeshAsset(MeshManager::Mesh &mesh, _Float32 pitch, _Float32 yaw, _Float32 roll);
		void scaleMeshAsset(MeshManager::Mesh &mesh, _Float32 x, _Float32 y, _Float32 z);

		void translateCameraAsset(CameraManager::Camera &camera, _Float32 x, _Float32 y, _Float32 z, _Float32 velocity = 1.0);
		void rotateCameraAsset(CameraManager::Camera &camera, _Float32 pitch, _Float32 yaw, _Float32 roll);
		void orbitCameraAsset(CameraManager::Camera &camera, _Float32 azimuth, _Float32 elevation, _Float32 radius, _Float32 tarX = 0.0, _Float32 tarY = 0.0, _Float32 tarZ = 0.0);

		void translateLightAsset(LightManager::Light &light, _Float32 x, _Float32 y, _Float32 z);

	private:
		bool determineIsSigned(_Float32 x, _Float32 y, _Float32 z);
		_Float32 determineUpVector(_Float32 rotation);
		_Float32 degreesToRadians(_Float32 in, bool enforceLimits = true);

		_Float64 pi;
		_Float32 outRadians;
		_Float32 up;
		
		glm::vec3 rotation;

		glm::vec3 localCoordinates;
		glm::vec4 worldCoordinates;

		GlobalsManager globals;

};

#endif
