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
	PointInfo Support(const vector<SimpleMath::Vector3>& vertices_A, const vector<SimpleMath::Vector3>& vertices_B, SimpleMath::Vector3& direction) {
		PointInfo pointInfo;

		size_t i = IndexOfFurthestPoint(vertices_A, direction);
		pointInfo.supA = vertices_A[i];

		size_t j = IndexOfFurthestPoint(vertices_B, -direction);
		pointInfo.supB = vertices_B[j];

		pointInfo.point = pointInfo.supA - pointInfo.supB;
		//m_polytope.push_back(pointInfo);

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

}