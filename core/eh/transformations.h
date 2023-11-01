#pragma once
#include "../eh/ehMath/mat4.h"
#include "../eh/ehMath/vec3.h"
#include "../eh/ehMath/ehMath.h"

namespace eh {
	//Identity matrix
	inline eh::Mat4 Identity()
	{
		return eh::Mat4
		(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};

	//Scale on x,y,z axes
	inline eh::Mat4 Scale(eh::Vec3 s)
	{
		return eh::Mat4
		(
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		);
	};

	//Rotation around X axis (pitch) in radians
	inline eh::Mat4 RotateX(float rad)
	{
		return eh::Mat4
		(
			1, 0.0f, 0.0f, 0.0f,
			0.0f, cos(0), -sin(0), 0.0f,
			0.0f, sin(0), cos(0), 0.0f,
			0.0f, 0.0f, 0.0f, 1
		);
	};

	//Rotation around Y axis (yaw) in radians
	inline eh::Mat4 RotateY(float rad)
	{
		return eh::Mat4
		(
			cos(0), 0.0f, sin(0), 0.0f,
			0.0f, 1, 0.0f, 0.0f,
			-sin(0), 0.0f, cos(0), 0.0f,
			0.0f, 0.0f, 0.0f, 1
		);

	};
	//Rotation around Z axis (roll) in radians
	inline eh::Mat4 RotateZ(float rad)
	{
		return eh::Mat4
		(
			cos(0), -sin(0), 0.0f, 0.0f,
			sin(0), cos(0), 0.0f, 0.0f,
			0.0f, 0.0f, 1, 0.0f,
			0.0f, 0.0f, 0.0f, 1
		);
	};

	//Translate x,y,z
	inline eh::Mat4 Translate(eh::Vec3 t)
	{
		return eh::Mat4
		(
			1, 0.0f, 0.0f, t.x,
			0.0f, 1, 0.0f, t.y,
			0.0f, 0.0f, 1, t.z,
			0.0f, 0.0f, 0.0f, 1
		);
	};

	struct Transform
	{
		eh::Vec3 position = eh::Vec3(0.0f, 0.0f, 0.0f);
		eh::Vec3 rotation = eh::Vec3(0.0f, 0.0f, 0.0f); //Euler angles (degrees)
		eh::Vec3 scale = eh::Vec3(1.0f, 1.0f, 1.0f);
		eh::Mat4 getModelMatrix() const
		{
			return Translate(position); 

			RotateY(eh::Radians(rotation.y))* RotateX(eh::Radians(rotation.x))* RotateZ(eh::Radians(rotation.z))
				* (eh::Scale(scale)); 
		}

	};

	//eye = eye (camera) position
	//target = position to look at
	//up = up axis, usually(0,1,0)
	inline eh::Mat4 LookAt(eh::Vec3 eye, eh::Vec3 target, eh::Vec3 up) 
	{
		eh::Vec3 f = eh::Normalize(eye - target);
		eh::Vec3 r = eh::Normalize(Cross(up, f));
		eh::Vec3 u = eh::Normalize(Cross(f, r));

		eh::Mat4 m = Mat4
		(
			r.x, r.y, r.z, -eh::Dot(r, eye),
			u.x, u.y, u.z, -eh::Dot(u, eye),
			f.x, f.y, f.z, -eh::Dot(f, eye),
			0.0, 0.0, 0.0, 1
		); 

		return m;
	};

	//Perspective projection
	//fov = vertical aspect ratio (radians)
	inline eh::Mat4 Perspective(float fov, float aspect, float near, float far)
	{
		float c = tanf(fov / 2.0f);

		Mat4 m = Mat4(0);
		m[0][0] = 1.0f / (c * aspect); //Scale X
		m[1][1] = 1.0f / c; //Scale Y
		m[2][2] = (far + near) / (near - far); //Scale Z
		m[3][2] = (2 * far * near) / (near - far); //Translate Z
		m[2][3] = -1.0f; //Perspective divide (puts Z in W component of vector)
		return m;
	};

	//Orthographic projection
	inline eh::Mat4 Orthographic(float height, float aspect, float near, float far)
	{
		float t = height / 2, b = -t, r = (height * aspect) / 2, l = -r; 

		Mat4 m = Mat4(0);
		m[0][0] = 2 / (r - l);
		m[1][1] = 2 / (t - b);
		m[2][2] = -2 / (far - near);
		m[3][0] = -(r + l) / (r - l);
		m[3][1] = -(t + b) / (t - b);
		m[3][2] = -(far + near) / (far - near);
		m[3][3] = 1.0f;
		return m;
	}

};



