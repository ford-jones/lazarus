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
	this->up = 0.0f;
	this->localCoordinates = glm::vec3(0.0, 0.0, 0.0);
	this->worldCoordinates = glm::vec4(localCoordinates, 0.0);

	this->temp = vec3(0.0, 0.0, 0.0);
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
    mesh.modelMatrix = glm::rotate(mesh.modelMatrix, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
    mesh.modelMatrix = glm::rotate(mesh.modelMatrix, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
	mesh.modelMatrix = glm::rotate(mesh.modelMatrix, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
	
    return;
};

void Transform::scaleMeshAsset(MeshManager::Mesh &mesh, float x, float y, float z)
{
	mesh.modelMatrix = glm::scale(mesh.modelMatrix, glm::vec3(x, y, z));
};

void Transform::translateCameraAsset(CameraManager::Camera &camera, float x, float y, float z, float velocity)
{
	/* =========================================
		TODO:
		Handle camera roll
		Restore orbit / handle both camera cases
	============================================ */

	float speed = 0;

	/* =============================================
		Aggregate inputs to a single value and check
		the sign (-/+). 

		If the number is negative (i.e. 0.0f is
		higher than the aggregate value), then apply
		negation to the velocity input. (i.e. apply
		a conversion for speed when moving either
		left or backwards). 
	================================================ */
	float positiveSign = std::max(0.0f, (x + y + z));

	if(positiveSign == 0.0f)
	{
		speed = velocity - (velocity * 2);		
	}
	else
	{
		speed = velocity;
	};

	if(x != 0.0)
	{
		/* ============================================
			Note the coordinate system is right-handed.
			If the value of speed is negative, the 
			camera is moving to the left by a multiple
			of the -1 to 1 clamped value of the camera's 
			upward orientation vs direction. If pos,
			we go right.
		=============================================== */
		camera.position += (glm::normalize(glm::cross(camera.direction, camera.upVector)) * speed);
	};

	if(y != 0.0)
	{
		/* =============================================
			Using "velocity" here instead of "speed"
			because the variable here should always be 
			positively signed. Otherwise in the case of 
			(-y * -speed) the result will be positive.
			The matching sign is only relevant when 
			mulitplying against a vector.

			10.0 & 5.0 Multipliers because y movement appears
			considerably slower than z or x.
		================================================ */
		camera.position.y += ((y * 10.0f) * (velocity * 5.0f));
	};

	if(z != 0.0)
	{
		camera.position += (speed * camera.direction);
	}

	camera.viewMatrix = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
	
	return;
};

void Transform::rotateCameraAsset(CameraManager::Camera &camera, float x, float y, float z)
{	
	temp = vec3(0.0, 0.0, 0.0);

	if((x > 360.0f) || (x < -360.0f))
	{
		globals.setExecutionState(LAZARUS_INVALID_RADIANS);
	}
	else
	{
		this->up = this->determineUpVector(x);
		camera.upVector = glm::vec3(0.0f, this->up, 0.0f);

		temp.x = cos(glm::radians(y)) * cos(glm::radians(x));
		temp.y = sin(glm::radians(-x));
		temp.z = sin(glm::radians(y)) * cos(glm::radians(x)); 

		camera.direction = temp;

		camera.viewMatrix = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
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