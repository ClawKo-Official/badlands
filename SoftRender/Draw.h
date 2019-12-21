
#ifndef DRAW_H
#define DRAW_H


#include "FrameBuffer.h"
#include "Model.h"
#include "Camera.h"
#include "Clip.h"
#include "BlinnPhongShader.h"


enum RenderMode {
	Line,
	Fill
};
enum Face {
	Back,
	Front
};

//��Ⱦ����
//����ͼԪ ������
class Draw {

private:
	int Width;
	int Height;
	FrameBuffer *FrontBuffer;

	std::vector<glm::vec4> ViewPlanes;

	bool faceCull;
	Face cullMod;
	RenderMode renderMod;


public:

	Draw(const int &w, const int &h) : 
		Width(w),Height(h),
		FrontBuffer(nullptr),
		faceCull(false),
		cullMod(Back),
		renderMod(Fill)
	{
		ViewPlanes.resize(6, glm::vec4(0));
	}
	~Draw(){
		if (FrontBuffer)
			delete FrontBuffer;
		FrontBuffer = nullptr;
	}

	void Init() {
		if (FrontBuffer)
			delete FrontBuffer;
		FrontBuffer = new FrameBuffer(Width, Height);
	}

	void Resize(const int& w, const int &h) {
		Width = w;
		Height = h;
		FrontBuffer->Resize(w, h);
	}

	void ClearBuffer(const glm::vec4 &color) {
		FrontBuffer->ClearColorBuffer(color);
	}

#pragma region Statemachine
	void EnableCull(Face f) {
		faceCull = true;
		cullMod = f;
	}
	void DisableCull() {
		faceCull = false;
	}
	void changeRenderMode() {
		if (renderMod == Fill)
			renderMod = Line;
		else
			renderMod = Fill;
	}

	void UpdateViewPlanes() {
		ViewingFrustumPlanes(ViewPlanes,  ProjectMatrix * ViewMatrix);
	}

#pragma endregion Statemachine

#pragma region Pipeline
	//͸�ӳ���
	void PerspectiveDivision(V2F & v) {
		v.windowPos /= v.windowPos.w;
		v.windowPos.w = 1.0f;
		v.windowPos.z = (v.windowPos.z + 1.0) * 0.5;
	}

	//����ռ����׶�޳�
	bool ViewCull(const glm::vec4 &v1, const glm::vec4 &v2, const glm::vec4 &v3) {

		glm::vec3 minPoint, maxPoint;
		minPoint.x = min(v1.x, min(v2.x, v3.x));
		minPoint.y = min(v1.y, min(v2.y, v3.y));
		minPoint.z = min(v1.z, min(v2.z, v3.z));
		maxPoint.x = max(v1.x, max(v2.x, v3.x));
		maxPoint.y = max(v1.y, max(v2.y, v3.y));
		maxPoint.z = max(v1.z, max(v2.z, v3.z));
		// Near �� Far �޳�ʱֻ������ȫ���ڵ�
		if (!Point2Plane(minPoint, ViewPlanes[4]) || !Point2Plane(maxPoint, ViewPlanes[4])) {
			return false;
		}
		if (!Point2Plane(minPoint, ViewPlanes[5]) || !Point2Plane(maxPoint, ViewPlanes[5])) {
			return false;
		}

		if (!Point2Plane(minPoint, ViewPlanes[0]) && !Point2Plane(maxPoint, ViewPlanes[0])) {
			return false;
		}
		if (!Point2Plane(minPoint, ViewPlanes[1]) && !Point2Plane(maxPoint, ViewPlanes[1])) {
			return false;
		}
		if (!Point2Plane(minPoint, ViewPlanes[2]) && !Point2Plane(maxPoint, ViewPlanes[2])) {
			return false;
		}
		if (!Point2Plane(minPoint, ViewPlanes[3]) && !Point2Plane(maxPoint, ViewPlanes[3])) {
			return false;
		}

		return true;
	}

	//���޳����޳����������������
	bool FaceCull(Face face, const glm::vec4 &v1, const glm::vec4 &v2, const glm::vec4 &v3) {

		glm::vec3 tmp1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
		glm::vec3 tmp2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);

		//��˵õ�������
		glm::vec3 normal = glm::normalize(glm::cross(tmp1, tmp2));
		glm::vec3 view = glm::normalize(glm::vec3(v1.x - camera->Position.x, v1.y - camera->Position.y, v1.z - camera->Position.z));
		if (cullMod == Back)
			return glm::dot(normal, view) < 0;
		else
			return glm::dot(normal, view) > 0;
	}

	void DrawModel(Model &model) {
		for (int i = 0; i < model.objects.size(); i++) {
			DrawObject(model.objects[i]);
		}
	}

	void DrawObject(Object &obj) {
		if (obj.mesh.EBO.empty()) {
			return;
		}
		currentMat = &obj.material;
		for (int i = 0; i < obj.mesh.EBO.size(); i += 3) {
			Vertex p1, p2, p3;
			p1 = obj.mesh.VBO[obj.mesh.EBO[i]];
			p2 = obj.mesh.VBO[obj.mesh.EBO[i + 1]];
			p3 = obj.mesh.VBO[obj.mesh.EBO[i + 2]];

			V2F v1, v2, v3;
			v1 = currentMat->shader->VertexShader(p1);
			v2 = currentMat->shader->VertexShader(p2);
			v3 = currentMat->shader->VertexShader(p3);

			if (!ViewCull(v1.worldPos / v1.Z, v2.worldPos / v2.Z, v3.worldPos / v3.Z)) {
				continue;
			}

			//��͸�ӳ��� �任��NDC
			PerspectiveDivision(v1);
			PerspectiveDivision(v2);
			PerspectiveDivision(v3);

			//�ü����ɶ��������
			std::vector<V2F> clipingVertexs = SutherlandHodgeman(v1, v2, v3);

			//�������յ�������
			int n = clipingVertexs.size() - 3 + 1;
			for (int i = 0; i < n; i++) {
				V2F v1 = clipingVertexs[0];
				V2F v2 = clipingVertexs[i + 1];
				V2F v3 = clipingVertexs[i + 2];
				v1.windowPos = ViewPortMatrix * v1.windowPos;
				v2.windowPos = ViewPortMatrix * v2.windowPos;
				v3.windowPos = ViewPortMatrix * v3.windowPos;

				//�޳�������
				if (faceCull && !FaceCull(Back, v1.worldPos / v1.Z, v2.worldPos / v2.Z, v3.worldPos / v3.Z)) {
					continue;
				}

				if (renderMod == Line) {
					DrawLine(v1, v2);
					DrawLine(v2, v3);
					DrawLine(v3, v1);
				}
				else {
					ScanLineTriangle(v1, v2, v3);
				}
			}
		}
	}

	void Show() {
		glDrawPixels(Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, FrontBuffer->colorBuffer.data());
	}

#pragma endregion Pipeline

#pragma region Rasterization
	//ɨ��������㷨
	//�����������Σ���Ϊ��������ƽ�����������
	void ScanLineTriangle(const V2F &v1, const V2F &v2, const V2F &v3) {

		std::vector<V2F> arr = { v1,v2,v3 };
		if (arr[0].windowPos.y > arr[1].windowPos.y) {
			V2F tmp = arr[0];
			arr[0] = arr[1];
			arr[1] = tmp;
		}
		if (arr[1].windowPos.y > arr[2].windowPos.y) {
			V2F tmp = arr[1];
			arr[1] = arr[2];
			arr[2] = tmp;
		}
		if (arr[0].windowPos.y > arr[1].windowPos.y) {
			V2F tmp = arr[0];
			arr[0] = arr[1];
			arr[1] = tmp;
		}
		//arr[0] ��������  arr[2]��������

		//�м���������ȣ��ǵ�������
		if (equal(arr[1].windowPos.y, arr[2].windowPos.y)) {
			DownTriangle(arr[1], arr[2], arr[0]);
		}//��������
		else if (equal(arr[1].windowPos.y, arr[0].windowPos.y)) {
			UpTriangle(arr[1], arr[0], arr[2]);
		}
		else {
			float weight = (arr[2].windowPos.y - arr[1].windowPos.y) / (arr[2].windowPos.y - arr[0].windowPos.y);
			V2F newEdge = V2F::lerp(arr[2], arr[0], weight);
			UpTriangle(arr[1], newEdge, arr[2]);
			DownTriangle(arr[1], newEdge, arr[0]);
		}


	}
	void UpTriangle(const V2F &v1, const V2F &v2, const V2F &v3) {
		V2F left, right, top;
		left = v1.windowPos.x > v2.windowPos.x ? v2 : v1;
		right = v1.windowPos.x > v2.windowPos.x ? v1 : v2;
		top = v3;
		left.windowPos.x = int(left.windowPos.x);
		int dy = top.windowPos.y - left.windowPos.y;
		int nowY = top.windowPos.y;
		//�������²�ֵ
		for (int i = dy; i >= 0; i--) {
			float weight = 0;
			if (dy != 0) {
				weight = (float)i / dy;
			}
			V2F newLeft = V2F::lerp(left, top, weight);
			V2F newRight = V2F::lerp(right, top, weight);
			newLeft.windowPos.x = int(newLeft.windowPos.x);
			newRight.windowPos.x = int(newRight.windowPos.x + 0.5);
			newLeft.windowPos.y = newRight.windowPos.y = nowY;
			ScanLine(newLeft, newRight);
			nowY--;
		}
	}
	void DownTriangle(const V2F &v1, const V2F &v2, const V2F &v3) {
		V2F left, right, bottom;
		left = v1.windowPos.x > v2.windowPos.x ? v2 : v1;
		right = v1.windowPos.x > v2.windowPos.x ? v1 : v2;
		bottom = v3;

		int dy = left.windowPos.y - bottom.windowPos.y;
		int nowY = left.windowPos.y;
		//�������²�ֵ
		for (int i = 0; i < dy; i++) {
			float weight = 0;
			if (dy != 0) {
				weight = (float)i / dy;
			}
			V2F newLeft = V2F::lerp(left, bottom, weight);
			V2F newRight = V2F::lerp(right, bottom, weight);
			newLeft.windowPos.x = int(newLeft.windowPos.x);
			newRight.windowPos.x = int(newRight.windowPos.x + 0.5);
			newLeft.windowPos.y = newRight.windowPos.y = nowY;
			ScanLine(newLeft, newRight);
			nowY--;
		}
	}
	void ScanLine(const V2F &left, const V2F &right) {

		int length = right.windowPos.x - left.windowPos.x;
		for (int i = 0; i < length; i++) {
			V2F v = V2F::lerp(left, right, (float)i / length);
			v.windowPos.x = left.windowPos.x + i;
			v.windowPos.y = left.windowPos.y;

			//��Ȳ���
			float depth = FrontBuffer->GetDepth(v.windowPos.x, v.windowPos.y);
			if (v.windowPos.z < depth) {

				float z = v.Z;
				v.worldPos /= z;
				v.texcoord /= z;
				v.color /= z;
				v.normal /= z;

				FrontBuffer->WritePoint(v.windowPos.x, v.windowPos.y, currentMat->shader->FragmentShader(v));
				FrontBuffer->WriteDepth(v.windowPos.x, v.windowPos.y, v.windowPos.z);
			}
		}
	}

	//bresenhamLine �����㷨
	void DrawLine(const V2F &from, const V2F &to)
	{
		int dx = to.windowPos.x - from.windowPos.x;
		int dy = to.windowPos.y - from.windowPos.y;
		int Xstep = 1, Ystep = 1;
		if (dx < 0)
		{
			Xstep = -1;
			dx = -dx;
		}
		if (dy < 0)
		{
			Ystep = -1;
			dy = -dy;
		}
		int currentX = from.windowPos.x;
		int currentY = from.windowPos.y;
		V2F tmp;
		//б��С��1
		if (dy <= dx)
		{
			int P = 2 * dy - dx;
			for (int i = 0; i <= dx; ++i)
			{
				tmp = V2F::lerp(from, to, ((float)(i) / dx));
				FrontBuffer->WritePoint(currentX, currentY, glm::vec4(255, 0, 0, 0));
				currentX += Xstep;
				if (P <= 0)
					P += 2 * dy;
				else
				{
					currentY += Ystep;
					P += 2 * (dy - dx);
				}
			}
		}
		//б�ʴ���1�����öԳ��Ի�
		else
		{
			int P = 2 * dx - dy;
			for (int i = 0; i <= dy; ++i)
			{
				tmp = V2F::lerp(from, to, ((float)(i) / dy));
				FrontBuffer->WritePoint(currentX, currentY, glm::vec4(255, 0, 0, 0));
				currentY += Ystep;
				if (P <= 0)
					P += 2 * dx;
				else
				{
					currentX += Xstep;
					P -= 2 * (dy - dx);
				}
			}
		}
	}
#pragma endregion Rasterization

};

#endif