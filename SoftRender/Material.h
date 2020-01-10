#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"

//��׼���ղ��ʰ���
//��������ɫ ���淴����ɫ ���淴��ǿ��
//����һ��
class Material {
public:
	glm::vec4 Color;
	glm::vec4 Specular;
	int Gloss;   // 8 - 256
	Texture * MainTex;
	Shader * shader;

	Texture *Metallic;
	Texture *Normal;
	Texture *Roughness;
	Texture *Ao;
	Texture *Height;

	Material() :
		Color(glm::vec4(1.0, 1.0, 1.0,1.0)),
		Specular(glm::vec4(1.0, 1.0, 1.0,1.0)),
		Gloss(32),
		MainTex(nullptr),
		Metallic(nullptr),
		Normal(nullptr),
		Roughness(nullptr),
		Ao(nullptr),
		Height(nullptr)
	{}
	Material(const glm::vec4 &color,const glm::vec4 & specular,const int & gloss):
		Color(color),
		Specular(specular),
		Gloss(gloss),
		MainTex(nullptr),
		Metallic(nullptr),
		Normal(nullptr),
		Roughness(nullptr),
		Ao(nullptr),
		Height(nullptr)
	{}

	virtual ~Material() = default;

	void SetShader(Shader * s) {
		shader = s;
	}
	void SetTexture(Texture * t) {
		MainTex = t;
	}
	void SetMetallicMap(Texture * m) {
		Metallic = m;
	}
	void SetNormalMap(Texture * n) {
		Normal = n;
	}
	void SetRoughnessMap(Texture * r) {
		Roughness = r;
	}
	void SetAoMap(Texture * ao) {
		Ao = ao;
	}
	void SetHeightMap(Texture *h) {
		Height = h;
	}
};













#endif