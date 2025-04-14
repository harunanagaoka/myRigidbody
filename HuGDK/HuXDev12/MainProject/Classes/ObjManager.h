///OriginalGeometric.h
//Rigidbody量産用のコンテナ！

#pragma once
#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include <wrl/client.h> 
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "OriginalGeometric.h"
#include "Rigidbody.h"
#include <Effects.h>

using namespace MyGeometory;
using namespace std;
using namespace DirectX;

class ObjManager
{
public:
	void AddObject(float size,SimpleMath::Vector3 pos, bool useGravity)
	{
	    auto primitive = MyGeometory::Primitive::CreateCubePrimitive(size, DXTK->Device);
		m_primitives.push_back(primitive);

		CreatRigidBody(primitive,pos, useGravity);

	};

	void CreatRigidBody(shared_ptr<Primitive> primitive, SimpleMath::Vector3 pos, bool useGravity)
	{
		SimpleMath::Matrix matrix;
		matrix = SimpleMath::Matrix::Identity;
		auto rigidbody = std::make_shared<Rigidbody>(primitive,matrix,pos, useGravity);
		m_rigidbodies.push_back(rigidbody);
	}

	void UpdateAll()
	{
		for (auto& rb : m_rigidbodies)
		{
			rb->Update();
		}

		if (m_rigidbodies.size() <= 1)
		{
			return;
		}

		for (size_t i = 0; i < m_rigidbodies.size(); ++i) {
			for (size_t j = i + 1; j < m_rigidbodies.size(); ++j) {
				auto& a = m_rigidbodies[i];
				auto& b = m_rigidbodies[j];

				if (IsCollidingAABB(a->GetPos(), a->GetAABB(),
					b->GetPos(), b->GetAABB())) {
					GetPenetrationCorrection(a, b);//めりこみ補正
					a->ApplyCollision(); b->ApplyCollision();
				}
			}
		}
	}

	void  GetPenetrationCorrection(
		shared_ptr<Rigidbody>& a, shared_ptr<Rigidbody>& b)
	{
		//InitPosは初期座標、halfExtentsは初期座標を中心点としたときの頂点から中心点までの距離
		//a,bをもらってふつうに参照するわ‥‥
		
		//aとbの距離計算
		SimpleMath::Vector3 distance = a->GetPos() - b->GetPos();
		distance.x = abs(distance.x);
		distance.y = abs(distance.y);
		distance.z = abs(distance.z);

		//aとbの中心点までの距離の合計
		SimpleMath::Vector3 ABExtends = a->GetAABB() + b->GetAABB();

		//めりこみ量の計算
		SimpleMath::Vector3 overlap = ABExtends - distance;

		// 一番浅い（最小）めり込み軸を選ぶ
		float minOverlap = overlap.x;
		float n = (distance.x < 0 ? -1.0f : 1.0f);
		SimpleMath::Vector3 normal = SimpleMath::Vector3(n, 0.0f, 0.0f);

		if (overlap.y < minOverlap) {
			minOverlap = overlap.y;
			n = (distance.y < 0 ? -1.0f : 1.0f);
			normal = SimpleMath::Vector3(0.0f, n, 0.0f);
		}

		if (overlap.z < minOverlap) {
			minOverlap = overlap.z;
			n = (distance.z < 0 ? -1.0f : 1.0f);
			normal = SimpleMath::Vector3(0.0f, 0.0f, n);
		}

		// 修正ベクトル（MTV）を法線方向に minOverlap 分だけ
		SimpleMath::Vector3 correction = normal * minOverlap;//補正値

		if (a->IsGravity() && b->IsGravity())
		{
			SimpleMath::Vector3 cora = -correction * 0.5;
			a->ApplyCorrection(cora);
			SimpleMath::Vector3 corb = correction * 0.5;
			b->ApplyCorrection(corb);
			
		}

		if (a->IsGravity() && !b->IsGravity())
		{
			a->ApplyCorrection(correction);
		}

		if (!a->IsGravity() && b->IsGravity())
		{
			b->ApplyCorrection(correction);
		}
	}

	SimpleMath::Vector3 ComputeOverlap(SimpleMath::Vector3 distance, SimpleMath::Vector3 extends)
	{
		

		SimpleMath::Vector3 overlap = distance - extends;

		//xyzで一番差が小さいものを返す
		//min(abs(overlap.x), abs(overlap.y), abs(overlap.z));
		//abs(overlap.x);
		//abs(overlap.y);
		//abs(overlap.z);


		if (distance.x < extends.x ||
			distance.y < extends.y ||
			distance.z < extends.z)
		{
			
			
			SimpleMath::Vector3 lap = SimpleMath::Vector3::Zero;

				
		}
		
		return SimpleMath::Vector3::Zero;
	}

	void DrawAll(DirectX::BasicEffect* shader)
	{
		int count = 0;
		for (auto& pr : m_primitives)
		{
			shader->SetWorld(m_rigidbodies[count]->GetWorldMatrix());
			shader->Apply(DXTK->CommandList);
			pr->Draw(DXTK->CommandList);
			count++;
		}
	}

	bool IsCollidingAABB(const SimpleMath::Vector3& posA, const SimpleMath::Vector3& halfA,
		const SimpleMath::Vector3& posB, const SimpleMath::Vector3& halfB)
	{//重なりによる衝突検出
		if (std::abs(posA.x - posB.x) > (halfA.x + halfB.x)) return false;
		if (std::abs(posA.y - posB.y) > (halfA.y + halfB.y)) return false;
		if (std::abs(posA.z - posB.z) > (halfA.z + halfB.z)) return false;

		return true;
	}

private:
	vector<shared_ptr<Primitive>> m_primitives;

	vector<shared_ptr<Rigidbody>> m_rigidbodies;

};





