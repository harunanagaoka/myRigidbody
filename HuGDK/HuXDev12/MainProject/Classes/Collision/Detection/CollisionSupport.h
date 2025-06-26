//
// CollisionSupport.h
//GJK,EPAで使用する関数、構造体管理クラス

#pragma once

#include "..\..\..\Base\pch.h"
#include "..\..\..\Base\dxtk.h"
#include <unordered_set>

using namespace std;

namespace CollisionSupport {

	//Support()で求めた点を保存します。
	struct PointInfo {
		SimpleMath::Vector3 point;
		SimpleMath::Vector3 supA, supB;
	};

	//衝突情報を保存します。
	struct ContactInfo {
		bool hasValue = false;//衝突したかどうか
		SimpleMath::Vector3 contactPointA = SimpleMath::Vector3::Zero;
		SimpleMath::Vector3 contactPointB = SimpleMath::Vector3::Zero;
		SimpleMath::Vector3 normal = SimpleMath::Vector3::Zero;        // 法線（A→B）
		float penetrationDepth = 0;									   // 衝突深度
		SimpleMath::Vector3 tangent1 = SimpleMath::Vector3::Zero;      // 摩擦方向１
		SimpleMath::Vector3 tangent2 = SimpleMath::Vector3::Zero;      // 摩擦方向２
	};

	struct Vector3Hash
	{
		std::size_t operator()(const SimpleMath::Vector3& v) const noexcept
		{
			std::size_t hx = std::hash<float>{}(v.x);
			std::size_t hy = std::hash<float>{}(v.y);
			std::size_t hz = std::hash<float>{}(v.z);
			return hx ^ (hy << 1) ^ (hz << 2);
		}
	};

	struct Vector3Equal
	{
		bool operator()(const SimpleMath::Vector3& lhs, const SimpleMath::Vector3& rhs) const noexcept
		{
			constexpr float epsilon = 1e-6f;
			return fabs(lhs.x - rhs.x) < epsilon &&
				fabs(lhs.y - rhs.y) < epsilon &&
				fabs(lhs.z - rhs.z) < epsilon;
		}
	};

	size_t IndexOfFurthestPoint(const vector<SimpleMath::Vector3>& vertices, const SimpleMath::Vector3& direction);

	PointInfo Support(const vector<SimpleMath::Vector3>& vertices_A, const vector<SimpleMath::Vector3>& vertices_B, SimpleMath::Vector3& direction);

	bool CheckDuplicationAndSet(unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal>& container, SimpleMath::Vector3& value);

	constexpr inline size_t g_maxSimplexSize = 4; /*三次元なのでシンプレックスを四面体に設定しています*/

	bool TryEncloseOrigin(const vector<SimpleMath::Vector3>& vertices_A, const vector<SimpleMath::Vector3>& vertices_B, array<PointInfo, g_maxSimplexSize>& simplex, size_t& index);

	bool IsEncloseOrigin(const SimpleMath::Vector3 pointA, const SimpleMath::Vector3 pointB, const SimpleMath::Vector3 pointC, const SimpleMath::Vector3 pointD);

	bool IsValidTetrahedron(SimpleMath::Vector3 pointA, SimpleMath::Vector3 pointB, SimpleMath::Vector3 pointC, SimpleMath::Vector3 pointD);

	void GenerateFrictionBasis(const SimpleMath::Vector3 normal, SimpleMath::Vector3& tangent1, SimpleMath::Vector3& tangent2);
};