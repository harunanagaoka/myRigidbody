//
// SimulationObject.h
//見た目とリジッドボディの統合クラス

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\Rigidbody.h"

class SimulationObject  {
public:
	SimulationObject() : m_rigidbody(nullptr) {
	}

	~SimulationObject() = default;

	void SetShape(){
		m_shape = GeometricPrimitive::CreateTetrahedron();//テスト用四面体
	}

	void SetRigidbody(Rigidbody* rb) { m_rigidbody = rb; }

	void Render(DirectXTK::BasicEffect& effect);

private:
	DirectXTK::GeometricPrimitive m_shape;
	Rigidbody* m_rigidbody;//deleteは管理クラスで一括で行う
};




/*
//enum class ObjectType {種類増えてきたら使う
//	Tetrahedron
//
//};*/