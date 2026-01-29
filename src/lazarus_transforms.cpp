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

Transform::Transform()
{
	LOG_DEBUG("Constructing Lazarus::Transform");

	/* ===========================================
		Use low precision (and incorrect) pi value
		so that things like camera rotation dont 
		break at the extremities.
	============================================== */

	this->pi = 3.1419;
	this->accumulatePitch = 0.0f;
	this->accumulateRoll = 0.0f;
	this->accumulateYaw = 0.0f;
	this->outRadians = 0.0;
	this->up = 0.0f;
	this->localCoordinates = glm::vec3(0.0, 0.0, 0.0);
	this->worldCoordinates = glm::vec4(localCoordinates, 0.0);

	this->rotation = vec3(0.0, 0.0, 0.0);
};

lazarus_result Transform::translateModel(ModelManager::Model &model, float x, float y, float z, uint32_t instanceID)
{
	ModelManager::Model::Instance &instance = model.instances.at(instanceID);
	glm::mat4 &instanceMatrix = instance.modelMatrix;

	this->localCoordinates = glm::vec3(x, y, z);

    instanceMatrix = glm::translate(instanceMatrix, this->localCoordinates);

	/* ===========================================================================
		Find worldspace coordinates by multiplying object-space coordinates by the 
		entity's modelview matrix.

		See: https://learnopengl.com/img/getting-started/coordinate_systems.png
	=============================================================================== */
	
	this->worldCoordinates = instanceMatrix * glm::vec4(this->localCoordinates, 1.0);

    instance.position.x = this->worldCoordinates.x;
    instance.position.y = this->worldCoordinates.y;
    instance.position.z = this->worldCoordinates.z;

	return lazarus_result::LAZARUS_OK;
};

lazarus_result Transform::rotateModel(ModelManager::Model &model, float pitch, float yaw, float roll, uint32_t instanceID)
{
	/* ===================================================
		Extract the current z axis rotation values from
		row 3 of the matrice and then truncate the last 
		element. This can be treated as the mesh asset's 
		forward / direction vector.
	====================================================== */
	ModelManager::Model::Instance &instance = model.instances.at(instanceID);
	glm::mat4 &instanceMatrix = instance.modelMatrix;

	instance.direction = instanceMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); 

    instanceMatrix = glm::rotate(instanceMatrix, this->degreesToRadians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    instanceMatrix = glm::rotate(instanceMatrix, this->degreesToRadians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	instanceMatrix = glm::rotate(instanceMatrix, this->degreesToRadians(roll), glm::vec3(0.0f, 0.0f, 1.0f));
	
    return lazarus_result::LAZARUS_OK;
};

lazarus_result Transform::scaleModel(ModelManager::Model &model, float x, float y, float z, uint32_t instanceID)
{
	float sum = (x + y + z);
	float max = std::max(0.0f, sum);

	if(max <= 0.0f)
	{
        LOG_ERROR("Transform Error", __FILE__, __LINE__);

		return lazarus_result::LAZARUS_INVALID_DIMENSIONS;	
	}
	else
	{
		ModelManager::Model::Instance &instance = model.instances.at(instanceID);
		glm::mat4 &instanceMatrix = instance.modelMatrix;

		instance.scale = glm::vec3(x, y, z);

		instanceMatrix = glm::scale(instanceMatrix, instance.scale);

		return lazarus_result::LAZARUS_OK;
	};
};

lazarus_result Transform::translateCamera(CameraManager::Camera &camera, float x, float y, float z, float velocity)
{
	/* =========================================
		TODO:
		Handle camera roll
	============================================ */

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

	camera.viewMatrix = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
	
	return lazarus_result::LAZARUS_OK;
};

lazarus_result Transform::rotateCamera(CameraManager::Camera &camera, float pitch, float yaw, float roll)
{	
	/* ============================================
		Ensures consistent behaviour between
		camera rotation vs translation, as the cam
		translation naturally accumulates.
	=============================================== */
	this->accumulatePitch += pitch;
	this->accumulateYaw += yaw;
	this->accumulateRoll += roll;
	
	if(	((pitch > 360.0f) || (pitch < -360.0f)) 					|| 
		((accumulatePitch > 360.0f) || (accumulatePitch < -360.0f))	)
	{
		this->accumulatePitch = 0;
        LOG_ERROR("Transform Error", __FILE__, __LINE__);
		
		return lazarus_result::LAZARUS_INVALID_RADIANS;
	}
	else
	{
		this->rotation = vec3(0.0, 0.0, 0.0);
		
		this->up = this->determineUpVector(accumulatePitch);
		camera.upVector = glm::vec3(0.0f, this->up, 0.0f);

		float p = this->degreesToRadians(accumulatePitch);
		float y = this->degreesToRadians(accumulateYaw, false);

		this->rotation.x = cos(y) * cos(p);
		this->rotation.y = sin(-p);
		this->rotation.z = sin(y) * cos(p); 

		camera.direction = this->rotation;

		camera.viewMatrix = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
	}
	
	return lazarus_result::LAZARUS_OK;
};

lazarus_result Transform::orbitCamera(CameraManager::Camera &camera, float azimuth, float elevation, float radius, float tarX, float tarY, float tarZ)
{	
	this->rotation = vec3(0.0, 0.0, 0.0);

	if((azimuth > 360.0f) || (azimuth < -360.0f))
	{
        LOG_ERROR("Transform Error", __FILE__, __LINE__);

		return lazarus_result::LAZARUS_INVALID_RADIANS;
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

		return lazarus_result::LAZARUS_OK;
	}
};

lazarus_result Transform::translateLight(LightManager::Light &light, float x, float y, float z)
{
	light.config.position += vec3(x, y, z);
	
	return lazarus_result::LAZARUS_OK;
};

float Transform::determineUpVector(float rotation)
{
	/* =========================================================
		When in the range of 90 - 270 degrees the orientation of
		"up" should be +y. (i.e. effectively the same way we see 
		the world. Consider LOS from looking at your feet, up to 
		the sky).  
	============================================================ */

	if(
		(rotation >= 90.0f && rotation <= 270.0f) || 
		(rotation <= -90.0f && rotation >= -270.0f))
	{
		return -1.0f;
	}
	else
	{
		return 1.0f;
	};
};

float Transform::degreesToRadians(float in, bool enforceLimits)
{
	/* =======================================================================
		Optionally check range is valid
		This is so that certain illegal / breaking calculations can't be made
	========================================================================== */
	if(
		enforceLimits && 
		((in > 360.0f) || (in < -360.0f)))	
	{
        LOG_ERROR("Transform Error", __FILE__, __LINE__);

		return lazarus_result::LAZARUS_INVALID_RADIANS;
	};

	this->outRadians = in * this->pi / 180.0f;

	return this->outRadians;
};

Transform::~Transform()
{
	LOG_DEBUG("Destroying Lazarus::Transform");
};