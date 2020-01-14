#ifndef LIGHT_H
#define LIGHT_H

#include "Globel.h"


class Light {
public:
	glm::vec3 Position;

	glm::vec3 Color;
	glm::vec3 Specular;
	glm::vec3 Direction;
	float Intensity;

	virtual ~Light() {
	}
	Light(
		const glm::vec3 &pos = glm::vec3(0, 0, 0),
		const glm::vec3 &color = glm::vec3(1, 1, 1),
		const glm::vec3 &specular = glm::vec3(1, 1, 1),
		const glm::vec3 &dir = glm::vec3(0, -1, 0),
		const float & i = 1.0f) :
		Position(pos),
		Color(color),
		Specular(specular),
		Direction(dir),
		Intensity(i)
	{}
};
//����� λ�ò���Ҫ
class DirectionLight : public Light {

public:
	DirectionLight(
		const glm::vec3 &dir = glm::normalize(glm::vec3(0, -1, 1)),
		const glm::vec3 &color = glm::vec3(1, 1, 1),
		const glm::vec3 &specular = glm::vec3(1, 1, 1),
		const float & i = 1.0f) : Light(glm::vec3(0, 0, 0),color,specular,dir,i)
	{}
};
//���Դ ������Ҫ
class PointLight : public Light {

public:
	float Constant;
	float Linear;
	float Quadratic;

	PointLight(
		const glm::vec3 &pos = glm::vec3(0, 0, 0),
		const glm::vec3 &color = glm::vec3(1, 1, 1),
		const glm::vec3 &specular = glm::vec3(1, 1, 1),
		const float & i = 1.0f,
		const float & c = 1.0f,
		const float & l = 0.09f,
		const float & q = 0.032f
	) : Light(pos,color,specular,glm::vec3(0,0,0),i),Constant(c), Linear(l), Quadratic(q)
	{}
};
//̽�յ�
class SpotLight : public PointLight {

public:
	float innerCutOff;
	float outterCutOff;

	SpotLight(
		const glm::vec3 &pos = glm::vec3(0, 0, 0),
		const glm::vec3 &dir = glm::vec3(0, 0, -1),
		const glm::vec3 &color = glm::vec3(1, 1, 1),
		const glm::vec3 &specular = glm::vec3(1, 1, 1),
		const float & i = 1.0f,
		const float & c = 1.0f,
		const float & l = 0.09f,
		const float & q = 0.032f,
		const float & icut = glm::cos(glm::radians(12.5f)),
		const float & ocut = glm::cos(glm::radians(17.5))
	) : PointLight(pos,color,specular,i,c,l,q),innerCutOff(icut), outterCutOff(ocut)
	{
		Direction = dir;
	}
};


#endif