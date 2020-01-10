#pragma once
#include "Vertex.h"

//��������
// Ax + By + C < 0 �����ڲ�
const std::vector<glm::vec3> ViewLines = {
	glm::vec3(-1,0,-1),
	glm::vec3(0,-1,-1),
	glm::vec3(1,0,-1),
	glm::vec3(0,1,-1)
};

bool Inside(const glm::vec3 &line,const glm::vec3 &p) {
	return line.x * p.x + line.y * p.y + line.z < 0;
}

bool AllVertexsInside(const glm::vec4 &v1, const glm::vec4 &v2, const glm::vec4 &v3) {
	if (v1.x > 1 || v1.x < -1)
		return false;
	if (v1.y > 1 || v1.y < -1)
		return false;
	if (v2.x > 1 || v2.x < -1)
		return false;
	if (v2.y > 1 || v2.y < -1)
		return false;
	if (v3.x > 1 || v3.x < -1)
		return false;
	if (v3.y > 1 || v3.y < -1)
		return false;
	return true;
}
//���㣬ͨ���˵��ֵ
V2F Intersect(const V2F &v1,const V2F &v2,const glm::vec3 &line) {
	if (line.x == -1) {
		float weight = (-1 - v1.windowPos.x) / (v2.windowPos.x - v1.windowPos.x);
		return V2F::lerp(v1, v2, weight);
	}
	if (line.x == 1) {
		float weight = (1 - v1.windowPos.x) / (v2.windowPos.x - v1.windowPos.x);
		return V2F::lerp(v1, v2, weight);
	}
	if (line.y == -1) {
		float weight = (-1 - v1.windowPos.y) / (v2.windowPos.y - v1.windowPos.y);
		return V2F::lerp(v1, v2, weight);
	}
	if (line.y == 1) {
		float weight = (1 - v1.windowPos.y) / (v2.windowPos.y - v1.windowPos.y);
		return V2F::lerp(v1, v2, weight);
	}
}

//���� �������� ��� �ü���Ķ�����
//ע�� �����ʱ��˳��Ҫ���ü��ߵı���˳��һ��
std::vector<V2F> SutherlandHodgeman(const V2F &v1, const V2F &v2, const V2F &v3) {
	std::vector<V2F> output = {v1,v2,v3};
	if (AllVertexsInside(v1.windowPos , v2.windowPos , v3.windowPos)) {
		return output;
	}
	for (int i = 0; i < 4; i++) {
		std::vector<V2F> input(output);
		output.clear();
		for (int j = 0; j < input.size(); j++) {
			V2F current = input[j];
			V2F last = input[(j + input.size() - 1) % input.size()];
			if (Inside(ViewLines[i], current.windowPos)) {
				if (!Inside(ViewLines[i],last.windowPos)) {
					V2F intersecting = Intersect(last, current, ViewLines[i]);
					output.push_back(intersecting);
				}
				output.push_back(current);
			}
			else if(Inside(ViewLines[i], last.windowPos)){
				V2F intersecting = Intersect(last, current, ViewLines[i]);
				output.push_back(intersecting);
			}
		}
	}
	return output;
}

