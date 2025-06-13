//
// SimulationObject.h
//見た目とリジッドボディの統合クラス

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\Rigidbody.h"

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
	Rigidbody* m_rigidbody;//deleteは管理クラスで一括で行う
	SimpleMath::Vector3 m_size = SimpleMath::Vector3::One;
};




/*
//enum class ObjectType {種類増えてきたら使う
//	Tetrahedron
//
//};*/