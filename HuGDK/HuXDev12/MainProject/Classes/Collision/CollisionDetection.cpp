
#include "CollisionDetection.h"

size_t CollisionDetection::IndexOfFurthestPoint(const array<SimpleMath::Vector3, 8>& vertices,
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

//SimpleMath::Vector3 CollisionDetection::Support(const array<SimpleMath::Vector3, 8> vertices_1,
//	const array<SimpleMath::Vector3, 8> vertices_2,
//	const SimpleMath::Vector3& direction) {
//	size_t i = IndexOfFurthestPoint(vertices_1, direction);
//
//	size_t j = IndexOfFurthestPoint(vertices_2, -direction);
//
//	return vertices_1[i] - vertices_2[j];
//
//}

bool CollisionDetection::HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d) {
	// 四面体の頂点 A, B, C, D （Aが最新＝support点）
	const SimpleMath::Vector3& A = simplex[3];
	const SimpleMath::Vector3& B = simplex[2];
	const SimpleMath::Vector3& C = simplex[1];
	const SimpleMath::Vector3& D = simplex[0];

	// 原点に向かうベクトル
	SimpleMath::Vector3 AO = -A;

	// 各面の法線を計算　
	SimpleMath::Vector3 ABCNormal = (B - A).Cross(C - A);// ABC面
	SimpleMath::Vector3 ACDNormal = (C - A).Cross(D - A);// ACD面
	SimpleMath::Vector3 ADBNormal = (D - A).Cross(B - A);// ADB面

	// 法線を内向きに揃える
	if (ABCNormal.Dot(D - A) < 0)
	{
		ABCNormal = -ABCNormal;
	}
	if (ACDNormal.Dot(B - A) < 0)
	{
		ACDNormal = -ACDNormal;
	}
	if (ADBNormal.Dot(C - A) < 0)
	{
		ADBNormal = -ADBNormal;
	}


	//原点が含まれるか
	if (ABCNormal.Dot(AO) < 0)
	{
		simplex = { C, B, A };
		index = 3;
		d = -ABCNormal;
		m_polytope.erase(m_polytope.begin() + 0);//Dの情報を消去
		return false;
	}

	if (ACDNormal.Dot(AO) < 0)
	{
		simplex = { D, C, A };
		index = 3;
		d = -ACDNormal;
		m_polytope.erase(m_polytope.begin() + 2);
		return false;
	}

	if (ADBNormal.Dot(AO) < 0)
	{
		simplex = { B, D, A };
		index = 3;
		d = -ADBNormal;
		m_polytope.erase(m_polytope.begin() + 1);
		return false;
	}
	//全ての面の内向き法線が原点方向 → 四面体が原点を囲んでいる
	return true;

}

bool CollisionDetection::HandleTriangle(std::array<SimpleMath::Vector3, 4>& simplex, size_t& index, SimpleMath::Vector3& direction) {
	
		const SimpleMath::Vector3& a = simplex[2];
		const SimpleMath::Vector3& b = simplex[1];
		const SimpleMath::Vector3& c = simplex[0];

		SimpleMath::Vector3 ao = -a;//原点方向
		SimpleMath::Vector3 ab = b - a;
		SimpleMath::Vector3 ac = c - a;

		SimpleMath::Vector3 acperp = ac.Cross(ab).Cross(ac);//外向き法線

		//acperp.Dot(ao) <= 0 かつ abperp.Dot(ao) <= 0であれば三角形の内側に原点がある

		if (acperp.Dot(ao) > 0)//原点がACの外にいるかどうか
		{
			simplex[0] = simplex[1];
			simplex[1] = simplex[2];
			index = 2;
			direction = acperp;
			m_polytope.erase(m_polytope.begin());//Cを捨てる
			return false;
		}

		SimpleMath::Vector3 abperp = ab.Cross(ac).Cross(ab);

		if (abperp.Dot(ao) > 0)//原点がABの外にいるかどうか
		{
			simplex[0] = simplex[1];
			simplex[1] = simplex[2];
			index = 2;
			m_polytope.erase(m_polytope.begin());//Cを捨てる
			direction = abperp;
			return false;
		}

		//ここまで来たら原点が三角形の内側にある→四面体フェーズへ

		SimpleMath::Vector3 normal = ab.Cross(ac);//面の法線
		if (normal.Dot(ao) < 0)
		{
			normal = -normal;
		}
		direction = normal;//原点方向に探索する
		return true;
}

bool CollisionDetection::GJK(const BoxCollider& collider_1,const BoxCollider& collider_2) {
	const array<SimpleMath::Vector3, 8> vertices_1 = collider_1.GetWorldVertices();
	const array<SimpleMath::Vector3, 8> vertices_2 = collider_2.GetWorldVertices();

	array<SimpleMath::Vector3, g_maxSimplexSize> simplex;
	SimpleMath::Vector3 ao, ab, ac, a, b, c, abperp, acperp;

	size_t index = 0;//現在のsimplexの頂点数

	//コライダーの中心点を求める
	SimpleMath::Vector3 center_1 = collider_1.ComputeCenter();
	SimpleMath::Vector3 center_2 = collider_2.ComputeCenter();

	//index が1つの時　→シンプレックス0次元の判定
	//center_2→center_1の方向を求める
	SimpleMath::Vector3 d = center_1 - center_2;

	if (d.LengthSquared() < 1e-6f)
	{
		d = SimpleMath::Vector3::UnitX;//中心点が同じならひとまずX軸から原点方向へ
	}

	//最初のシンプレックスを求める
	a = simplex[0] = Support(vertices_1, vertices_2, d);
	index++;

	if (a.Dot(d) < 0)
	{
		return false;//接触なし
	}

	d = -a;//次の探索方向を決める

	//2つめのシンプレックスを求める
	a = Support(vertices_1, vertices_2, d);

	if (a.Dot(d) < 0)
	{
		return false;
	}

	simplex[index++] = a;//2点目

	d = ao = -a;//原点方向へのベクトル

	//ここから2点の時の例外処理
	if (d.LengthSquared() < 1e-6f)
	{
		b = simplex[0];
		ab = b - a;
		d = ab.Cross(ao).Cross(ab);

		if (d.LengthSquared() < 1e-6f)
		{
			d = ab.Cross(SimpleMath::Vector3::UnitX);

			if (d.LengthSquared() < 1e-6f)
			{
				d = ab.Cross(SimpleMath::Vector3::UnitY);

				if (d.LengthSquared() < 1e-6f)
				{
					d = SimpleMath::Vector3::UnitZ;
				}
			}
		}
	}
	//ここまで例外処理

	int count = 0;
	while (count++ < 50)
	{
		a = Support(vertices_1, vertices_2, d);

		if (a.Dot(d) < 0)
		{
			return false;
		}

		simplex[index++] = a;//3点目

		if (HandleTriangle(simplex, index, d))
		{
			break;
		}

	}

	if (index <= 2)
	{
		return false;//HandleTriangleがtrueにならなかった時
	}

	//無限ループ防止
	count = 0;
	while (count++ < 50)//シンプレックスが4の時の処理
	{
		a = Support(vertices_1, vertices_2, d);

		if (a.Dot(d) < 0)
		{
			return false;
		}

		simplex[index++] = a;//4点目追加

		if (HandleTetrahedron(simplex, index, d))
		{
			return true;
		}
	}
	return false;

}