//
// SimulationObject.h
//�����ڂƃ��W�b�h�{�f�B�̓����N���X

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\Core/Rigidbody.h"

enum class ShapeName {
	Tetrahedron,
	Box
};

class SimulationObject  {
public:
	SimulationObject() : m_rigidbody(nullptr) {
	}

	~SimulationObject() = default;

	void SetShape(ShapeName shape);

	void SetRigidbody(Rigidbody* rb) { m_rigidbody = rb; }

	void SetSize(SimpleMath::Vector3 size);

	void Render(DirectXTK::BasicEffect& effect);

private:
	DirectXTK::GeometricPrimitive m_shape;

	//Rigidbody��delete�͊Ǘ��N���X�ňꊇ�ōs���Ă��܂��B
	Rigidbody* m_rigidbody;
	SimpleMath::Vector3 m_size = SimpleMath::Vector3::One;
};