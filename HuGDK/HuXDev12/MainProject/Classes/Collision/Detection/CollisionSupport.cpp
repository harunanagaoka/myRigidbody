#pragma once

#include<array>

#include "CollisionSupport.h"


namespace CollisionSupport {

	/// <summary>
	/// direction方向のベクトルで一番遠い頂点のインデックスを返します.
	/// </summary>
	/// <param name="vertices">頂点データ</param>
	/// <param name="direction">探索方向</param>
	size_t IndexOfFurthestPoint(const vector<SimpleMath::Vector3>& vertices,
		const SimpleMath::Vector3& direction) {

		float maxDot = -FLT_MAX;
		size_t index = 0;

		for (int i = 0; i < vertices.size(); i++)
		{
			float dot = vertices[i].Dot(direction);//directionとvertexの内積

			if (dot > maxDot)//最大の点を求める
			{
				maxDot = dot;
				index = i;
			}
		}

		return index;
	}

	/// <summary>
	/// 2つの図形のミンコフスキ差図形のうち探索方向で一番遠い点を返します。
	/// </summary>
	/// <param name="vertices_A">図形Aの頂点データ</param>
	/// <param name="vertices_B">図形Bの頂点データ</param>
	/// <param name="direction">探索方向</param>
	PointInfo Support(const vector<SimpleMath::Vector3>& vertices_A, const vector<SimpleMath::Vector3>& vertices_B,SimpleMath::Vector3& direction) {
		PointInfo pointInfo;

		size_t i = IndexOfFurthestPoint(vertices_A, direction);
		pointInfo.supA = vertices_A[i];

		size_t j = IndexOfFurthestPoint(vertices_B, -direction);
		pointInfo.supB = vertices_B[j];

		pointInfo.point = pointInfo.supA - pointInfo.supB;

		return pointInfo;
	}

	/// <summary>
	/// 今まで算出した頂点と新頂点valueが重複していないか確認する。重複していなかったら頂点をコンテナに保存。
	/// </summary>
	/// <param name="container">重複チェックに使うコンテナを用意すること</param>
	/// <param name="value">新頂点</param>
	bool CheckDuplicationAndSet(unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal>& container, SimpleMath::Vector3& value)
	{
		auto result = container.insert(value);
		return result.second;
	}

	/// <summary>
	/// シンプレックスが退化した場合、あらゆる方向に探索し、シンプレックスが原点を含むか確認します。
	/// </summary>
	/// <param name="simplex">シンプレックスの頂点</param>
	/// <param name="index">シンプレックスの現在の成長具合</param>
	bool TryEncloseOrigin(const vector<SimpleMath::Vector3>& vertices_A, const vector<SimpleMath::Vector3>& vertices_B, array<PointInfo, g_maxSimplexSize>& simplex, size_t& index) {
		SimpleMath::Vector3 UnitX = SimpleMath::Vector3::Right;
		SimpleMath::Vector3 UnitY = SimpleMath::Vector3::Up;
		SimpleMath::Vector3 UnitZ = SimpleMath::Vector3::Forward;

		std::array<DirectX::SimpleMath::Vector3, 3> axis{ UnitX,UnitY,UnitZ };

		SimpleMath::Vector3 A;
		SimpleMath::Vector3 B;
		SimpleMath::Vector3 C;
		SimpleMath::Vector3 D;

		switch (index) {
		case 1: {

			axis = { UnitX,UnitY,UnitZ };

			for (int i = 0; i < 3; i++) {

				simplex[index] = Support(vertices_A, vertices_B, axis[i]);
				index++;
			}

			if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
				return true;
			}
			else {
				axis = { -UnitX,-UnitY,-UnitZ };
				index = 1;
				for (int i = 0; i < 3; i++) {

					simplex[index] = Support(vertices_A, vertices_B, axis[i]);
					index++;
				}

				if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
					return true;
				}
				else {
					index = 1;
					for (int i = 0; i < 3; i++) {

						simplex[index] = {};
						index++;
					}

					index = 1;
					return false;
				}

			}

			break;
		}
		case 2: {
			axis = { UnitX,UnitY,UnitZ };
			//線分を作る
			SimpleMath::Vector3 d = simplex[1].point - simplex[0].point;

			for (int i = 0; i < 3; i++) {
				SimpleMath::Vector3 p = d.Cross(axis[i]);

				if (p.LengthSquared() > 0)
				{
					simplex[2] = Support(vertices_A, vertices_B, p);
					index = 3;
					if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
						return true;
					}
					else {
						p = -p;
						simplex[2] = Support(vertices_A, vertices_B, p);
						index = 3;
						if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
							return true;
						}
					}
				}
			}

			return false;

			break;
		}
		case 3: {
			A = simplex[2].point;
			B = simplex[1].point;
			C = simplex[0].point;

			SimpleMath::Vector3 normal = (B - A).Cross(C - A);//右手系ベクトル

			if (normal.LengthSquared() > 0) {

				simplex[3] = Support(vertices_A, vertices_B, normal);
				index = 4;
				if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
					return true;
				}
				else {
					normal = -normal;
					simplex[3] = Support(vertices_A, vertices_B, normal);
					index = 4;
					if (TryEncloseOrigin(vertices_A, vertices_B, simplex, index)) {
						return true;
					}
				}
			}

			return false;
			break;
		}
		case 4: {
			A = simplex[3].point;
			B = simplex[2].point;
			C = simplex[1].point;
			D = simplex[0].point;

			return IsEncloseOrigin(A, B, C, D);
			break;
		}
		}
		return false;
	}

	/// <summary>
	/// 4点の座標が構成する四面体に原点が含まれているかどうかを判定します。
	/// </summary>
	bool CollisionSupport::IsEncloseOrigin(const SimpleMath::Vector3 pointA, const SimpleMath::Vector3 pointB, const SimpleMath::Vector3 pointC, const SimpleMath::Vector3 pointD) {
		auto SignTest = [](const SimpleMath::Vector3& p1, const SimpleMath::Vector3& p2, const SimpleMath::Vector3& p3, const SimpleMath::Vector3& testPoint) {
			SimpleMath::Vector3 n = (p2 - p1).Cross(p3 - p1);
			return n.Dot(testPoint - p1) >= 0.0f;
			};

		bool s1 = SignTest(pointA, pointB, pointC, SimpleMath::Vector3::Zero);
		bool s2 = SignTest(pointA, pointC, pointD, SimpleMath::Vector3::Zero);
		bool s3 = SignTest(pointA, pointD, pointB, SimpleMath::Vector3::Zero);
		bool s4 = SignTest(pointB, pointD, pointC, SimpleMath::Vector3::Zero);

		return (s1 == s2) && (s2 == s3) && (s3 == s4);
	}

	/// <summary>
	/// 摩擦の方向を算出します。
	/// </summary>
	/// <param name="normal">接触法線</param>
	void GenerateFrictionBasis(const SimpleMath::Vector3 normal, SimpleMath::Vector3& tangent1, SimpleMath::Vector3& tangent2) {

		if (abs(normal.x) > abs(normal.z)) {
			// normalと垂直なベクトルを作る（x軸成分が大きいとき）
			tangent1 = SimpleMath::Vector3(-normal.y, normal.x, 0.0f);
		}
		else {
			// z軸成分が大きいとき
			tangent1 = SimpleMath::Vector3(0.0f, -normal.z, normal.y);
		}

		tangent1.Normalize();

		// tangent2 は normal と tangent1 の外積で作る
		tangent2 = normal.Cross(tangent1);
	}
}

