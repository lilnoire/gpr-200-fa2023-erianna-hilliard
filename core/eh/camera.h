#pragma once
#include "../eh/transformations.h"
#include "../eh/ehMath/ehMath.h"

namespace eh
{
	struct Camera
	{
		eh::Vec3 position = (0.0f, 0.0f, 5.0f); 
		eh::Vec3 target = (0.0f, 0.0f, 0.0f); 

		float fov = 60.0f;
		bool orthographic = false; 
		float orthoHeight = 6.0f; 
		float nearPlane = 0.1f;
		float farPlane = 100.0f; 
		float aspectRatio = 1.77f;

		inline eh::Mat4 ViewMatrix()const 
		{
			return eh::LookAt(position, target, eh::Vec3(0, 1, 0));
		}

		inline eh::Mat4 ProjectionMatrix()const 
		{
			if (orthographic) 
			{
				return eh::Orthographic(orthoHeight, aspectRatio, nearPlane, farPlane);
			}
			else 
			{
				return eh::Perspective(eh::Radians(fov), aspectRatio, nearPlane, farPlane);
			}
		}

	};

}


