
#ifndef MATH_H
#define MATH_H


#include"Globel.h"


// glm �ľ������о������Դ�ŵ�ʱ��Ҫת��
// �о������任��  v * M  ���о����� M * v

//�ӿھ���  ox oy�����½ǵ�����  ��-1,1��NDC �任�� ��Ļ���� [0,0],[w,h]
// Vp = [  w/2 ,   0  ,  0  , ox+w/2 ,
//			0  ,  h/2 ,  0  , oy+h/2 ,
//          0  ,   0  ,  1  ,   0    ,
//          0  ,   0  ,  0  ,   1   ]
glm::mat4 GetViewPortMatrix(int ox, int oy, int width, int height) {

	glm::mat4 result = glm::mat4(1.0f);
	result[0][0] = width / 2.0f;
	result[3][0] = ox + (width / 2.0f);
	result[1][1] = height / 2.0f;
	result[3][1] = oy + (height / 2.0f);
	return result;
}

//�۲����
// V = R*T
// T = [  1 , 0 , 0 , -eyex          R = [  Right , 0 
//        0 , 1 , 0 , -eyey                   UP  , 0
//        0 , 0 , 1 , -eyez               - Front , 0
//        0 , 0 , 0 ,   1   ]                 0   , 1 ]
//V =  [  Right  ,  - Right��eye
//          UP   ,  - UP��eye
//       -Front  ,   Front��eye
//         0     ,       1        ]
// ����ռ�һ��������ϵ��������ĻΪ+Z����������ӽǿ���ȥ�Ļ�+Z�ǳ�����Ļ���
// Ϊ���ó��򱣳ֲ��䣬��Front����ȡ��
 
glm::mat4 GetViewMatrix(glm::vec3 pos,glm::vec3 front,glm::vec3 right,glm::vec3 up) {
	glm::mat4 result = glm::mat4(1.0f);
	result[0][0] = right.x;
	result[1][0] = right.y;
	result[2][0] = right.z;
	result[3][0] = -glm::dot(right, pos);
	result[0][1] = up.x;
	result[1][1] = up.y;
	result[2][1] = up.z;
	result[3][1] = -glm::dot(up, pos);
	result[0][2] = -front.x;
	result[1][2] = -front.y;
	result[2][2] = -front.z;
	result[3][2] = glm::dot(front, pos);
	return result;
}

//͸��ͶӰ ���� fov(����) aspect near far
//M = [   1/aspect*tan(fov/2),       0      ,         0      ,       0
//               0  ,         1/tan(fov/2)  ,         0      ,       0 
//               0  ,                0      ,  - (f+n)/(f-n) ,  -2fn/(f-n)
//               0  ,                0      ,         -1     ,       0     ]
// ͶӰ֮�������ϵ���������ϵ��Z������Ļ���Ǹ�
glm::mat4 GetPerspectiveMatrix(const float & fovy, const float & aspect, const float & n, const float & f)
{
	glm::mat4 result = glm::mat4(0.0f);
	const float tanHalfFov = tan(fovy*0.5f);
	result[0][0] = 1.0f / (aspect*tanHalfFov);
	result[1][1] = 1.0f / (tanHalfFov);
	result[2][2] = -(f + n) / (f - n);
	result[2][3] = -1.0f;
	result[3][2] = (-2.0f*n*f) / (f - n);
	
	return result;
}

//��ģ�;�����ⷨ�߱任����
//��4X4ģ�;������Ͻǵ�3X3����ת�þ���
//���û�������������ŵĻ����߾������ģ�;������Ͻ�
glm::mat3 GetNormalMatrix(const glm::mat4 &model) {
	glm::mat3 result = model;
	result = glm::inverse(result);
	result = glm::transpose(result);
	return result;
}
void UpdateNormalMatrix() {
	NormalMatrix = GetNormalMatrix(ModelMatrix);
}

//��׶�������ƽ�淽�̣�������׶�޳�
//���õķ�����ָ���ڲ��ģ�����ԭ�㣩
void ViewingFrustumPlanes(std::vector<glm::vec4> & result , const glm::mat4 &vp) {
	//���  
	result[0].x = vp[0][3] + vp[0][0];
	result[0].y = vp[1][3] + vp[1][0];
	result[0].z = vp[2][3] + vp[2][0];
	result[0].w = vp[3][3] + vp[3][0];
	//�Ҳ�
	result[1].x = vp[0][3] - vp[0][0];
	result[1].y = vp[1][3] - vp[1][0];
	result[1].z = vp[2][3] - vp[2][0];
	result[1].w = vp[3][3] - vp[3][0];
	//�ϲ�
	result[2].x = vp[0][3] - vp[0][1];
	result[2].y = vp[1][3] - vp[1][1];
	result[2].z = vp[2][3] - vp[2][1];
	result[2].w = vp[3][3] - vp[3][1];
	//�²�
	result[3].x = vp[0][3] + vp[0][1];
	result[3].y = vp[1][3] + vp[1][1];
	result[3].z = vp[2][3] + vp[2][1];
	result[3].w = vp[3][3] + vp[3][1];
	//Near
	result[4].x = vp[0][3] + vp[0][2];
	result[4].y = vp[1][3] + vp[1][2];
	result[4].z = vp[2][3] + vp[2][2];
	result[4].w = vp[3][3] + vp[3][2];
	//Far
	result[5].x = vp[0][3] - vp[0][2];
	result[5].y = vp[1][3] - vp[1][2];
	result[5].z = vp[2][3] - vp[2][2];
	result[5].w = vp[3][3] - vp[3][2];
}

//�㵽ƽ����� d =  Ax + By + Cz + D;
// d < 0 ����ƽ�淨�򷴷�����ָ������
// d > 0 ����ƽ�淨����ָ������
// d = 0 ��ƽ����
// d < 0Ϊ false
bool Point2Plane(const glm::vec3 &v,const glm::vec4 &p) {
	
	float sb =  p.x * v.x + p.y * v.y + p.z * v.z + p.w;
	return sb >= 0;
}


//������ֵ
glm::vec4 Lerp(const glm::vec4 &v1,const glm::vec4 &v2,float factor) {
	return (1.0f - factor) * v1 + factor * v2;
}
glm::vec3 Lerp(const glm::vec3 &v1, const glm::vec3 &v2, float factor) {
	return (1.0f - factor)*v1 + factor * v2;
}
glm::vec2 Lerp(const glm::vec2 &v1, const glm::vec2 &v2, float factor) {
	return (1.0f - factor)*v1 + factor * v2;
}

float Lerp(const float &f1, const float &f2, float factor) {
	return (1.0f - factor)*f1 + factor * f2;
}

float min(const float & a, const float &b) {
	return a > b ? b : a;
}
float max(const float & a, const float &b) {
	return a > b ? a : b;
}

bool equal(const float &a, const float &b) {
	float c = a - b;
	if (c > 1e-6 || c < -1e-6) {
		return false;
	}
	else
		return true;
}

float saturate(const float &v) {
	if (v > 1)
		return 1;
	if (v < 0)
		return 0;
	return v;
}
//����ļ��� 2n * cos(n,l) - l = r
//lightDir�ǹ�ָ��Ƭ�εķ���
glm::vec3 reflect(const glm::vec3 &lightDir,const glm::vec3 &normal) {

	return lightDir - 2 * glm::dot(normal, lightDir) * normal;
}

#endif