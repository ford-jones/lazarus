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
//                ((        ...... ...                                                                   .  . .. .  .  ... .  ..      //                
//              ,/(#%#*                                                                                     .....  ... ......       .#*                 
//                 /((##%#(*                                                                                      .......        ,(#(*,                 
//               (.           .,,,,,                                                                                        .*#%%(                      
//                                                                                                      .***,.   . .,/##%###(/.  ...,,.      
/*  LAZARUS ENGINE */

#include "../include/lazarus_transforms.h"

/* =================================================
	TODO: 
	Create scaling translation
==================================================== */
Transform::Transform()
{
	this->pi = 3.1419;
	this->outRadians = 0.0;
	this->up = 0.0f;
	this->localCoordinates = glm::vec3(0.0, 0.0, 0.0);
	this->worldCoordinates = glm::vec4(localCoordinates, 0.0);

	this->rotation = vec3(0.0, 0.0, 0.0);
};

void Transform::translateMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z)
{
	this->localCoordinates = glm::vec3(x, y, z);
    mesh.modelMatrix = glm::translate(mesh.modelMatrix, this->localCoordinates);

	/* ===========================================================================
		Find worldspace coordinates by multiplying object-space coordinates by the 
		entity's modelview matrix.

		See: https://learnopengl.com/img/getting-started/coordinate_systems.png
	=============================================================================== */
	
	this->worldCoordinates = mesh.modelMatrix * glm::vec4(this->localCoordinates, 1.0);

    mesh.locationX = this->worldCoordinates.x;
    mesh.locationY = this->worldCoordinates.y;
    mesh.locationZ = this->worldCoordinates.z;

	return;
};

void Transform::rotateMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z)
{	
    mesh.modelMatrix = glm::rotate(mesh.modelMatrix, this->degreesToRadians(x), glm::vec3(1.0f, 0.0f, 0.0f));
    mesh.modelMatrix = glm::rotate(mesh.modelMatrix, this->degreesToRadians(y), glm::vec3(0.0f, 1.0f, 0.0f));
	mesh.modelMatrix = glm::rotate(mesh.modelMatrix, this->degreesToRadians(z), glm::vec3(0.0f, 0.0f, 1.0f));
	
    return;
};

void Transform::scaleMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z)
{
	bool positiveSign = this->determineIsSigned(x, y, z);
	if(!positiveSign)
	{
		globals.setExecutionState(LAZARUS_INVALID_DIMENSIONS);	
	}
	else
	{
		mesh.modelMatrix = glm::scale(mesh.modelMatrix, glm::vec3(x, y, z));
	};
};

void Transform::translateCameraAsset(CameraManager::Camera &camera, float x, float y, float z, float velocity)
{
	/* =========================================
		TODO:
		Handle camera roll
		Restore orbit / handle both camera cases
	============================================ */

	
	/* =============================================
	Aggregate inputs to a single value and check
	the sign (-/+). 
	
	If the number is negative (i.e. 0.0f is
	higher than the aggregate value), then apply
	negation to the velocity input. (i.e. apply
	a conversion for speed when moving either
	left or backwards). 
	================================================ */
	// float speed = 0;
	// bool positiveSign = this->determineIsSigned(x, y, z);

	// if(!positiveSign)
	// {
	// 	speed = velocity - (velocity * 2);		
	// }
	// else
	// {
	// 	speed = velocity;
	// };

	if(x != 0.0f)
	{
		/* ============================================
			Note the coordinate system is right-handed.
			If the value of speed is negative, the 
			camera is moving to the left by a multiple
			of the -1 to 1 clamped value of the camera's 
			upward orientation vs direction. If pos,
			we go right.
		=============================================== */
		camera.position += glm::normalize(glm::cross(camera.direction, camera.upVector)) * (x * velocity);
	};

	if(y != 0.0f)
	{
		camera.position.y += (y * velocity);
	};

	if(z != 0.0f)
	{
		camera.position += (z * velocity) * camera.direction;
	}
	// camera.viewMatrix = glm::lookAt(sin(cos(camera.position)), glm::vec3(0.0, 0.0, 0.0), camera.upVector);
	camera.viewMatrix = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
	
	return;
};

void Transform::rotateCameraAsset(CameraManager::Camera &camera, float x, float y, float z)
{	
	this->rotation = vec3(0.0, 0.0, 0.0);

	if((x > 360.0f) || (x < -360.0f))
	{
		globals.setExecutionState(LAZARUS_INVALID_RADIANS);
	}
	else
	{
		this->up = this->determineUpVector(x);
		camera.upVector = glm::vec3(0.0f, this->up, 0.0f);

		float a = this->degreesToRadians(y, false);
		float e = this->degreesToRadians(x);

		this->rotation.x = cos(a) * cos(e);
		this->rotation.y = sin(-e);
		this->rotation.z = sin(a) * cos(e); 

		camera.direction = this->rotation;

		camera.viewMatrix = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
	}
	
	return;
};

void Transform::orbitCameraAsset(CameraManager::Camera &camera, float azimuth, float elevation, float radius, float tarX, float tarY, float tarZ)
{	
	this->rotation = vec3(0.0, 0.0, 0.0);

	if((azimuth > 360.0f) || (azimuth < -360.0f))
	{
		globals.setExecutionState(LAZARUS_INVALID_RADIANS);
	}
	else
	{
		this->up = this->determineUpVector(azimuth);
		camera.upVector = glm::vec3(0.0f, this->up, 0.0f);
		
		float e = this->degreesToRadians(elevation, false);
		float a = this->degreesToRadians(azimuth);
		
		this->rotation.x = cos(e) * cos(a);
		this->rotation.y = sin(a);
		this->rotation.z = sin(e) * cos(a); 
		
		camera.direction = glm::vec3(tarX, tarY, tarZ);
		camera.position = camera.direction + (this->rotation * radius);
		
		camera.viewMatrix = glm::lookAt(camera.position, camera.direction, camera.upVector);
	}
	
	return;
};

void Transform::translateLightAsset(LightManager::Light &light, float x, float y, float z)
{
	light.lightPosition += vec3(x, y, z);
	light.locationX += x;
	light.locationY += y;
	light.locationZ += z;
	
	return;
};

float Transform::determineUpVector(float rotation)
{
	if((rotation >= 90.0f && rotation <= 270.0f) || (rotation <= -90.0f && rotation >= -270.0f))
	{
		return -1.0f;
	}
	else
	{
		return 1.0f;
	};
};

bool Transform::determineIsSigned(float x, float y, float z)
{
	if(std::max(0.0f, (x + y + z)) == 0.0f)
	{
		return false;
	}
	else
	{
		return true;
	};
};

float Transform::degreesToRadians(float in, bool enforceLimits)
{
	//	Optionally check range is valid
	//	This is so that certain illegal / breaking calculations can't be made
	if(enforceLimits && ((in > 360.0f) || (in < -360.0f)))	
	{
		globals.setExecutionState(LAZARUS_INVALID_RADIANS);
	};

	this->outRadians = in * this->pi / 180.0f;

	return this->outRadians;
};