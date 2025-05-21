
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

bool CollisionDetection::IsDuplication(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, SimpleMath::Vector3 newPoint) {

	if (simplex.size() == 0)//常にfalseになるのでなおす
	{
		return false;
	}

	for (int i = 0; i < simplex.size(); i++)
	{
		if ((simplex[i] - newPoint).LengthSquared() < 1e-6f)
		{
			return true;
		}
	}
	return false;
}

/// <summary>
/// 重複してたらfalse 登録されない
/// </summary>
bool CollisionDetection::CheckDuplicationAndSet(unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal>& container, SimpleMath::Vector3& value)
{
	auto result = container.insert(value);
	return result.second;
}

bool CollisionDetection::SimplexHitTest(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t index) 
{
	if (index == 1)
	{
		return simplex[0] == SimpleMath::Vector3(0, 0, 0);
	}

	if (index == 2)
	{
		SimpleMath::Vector3 A = simplex[1];
		SimpleMath::Vector3 B = simplex[0];
		SimpleMath::Vector3 AB = B - A;
		SimpleMath::Vector3 AO = -A;

		float ABdotAO = AB.Dot(AO);
		float t = ABdotAO / AB.LengthSquared();
		t = clamp(t, 0.0f, 1.0f);
		SimpleMath::Vector3 closestPoint = A + AB * t;
		float distance = closestPoint.Length();

		if (0.0f <= t && t <= 1.0f && distance < 1e-6f)//線分が原点を含包するか
		{
			return true;//かなりはなれててもtrueになっちゃう
		}
		else
		{
			return false;
		}

		//SimpleMath::Vector3 A = simplex[1];
		//SimpleMath::Vector3 B = simplex[0];
		//SimpleMath::Vector3 AB = B - A;
		//SimpleMath::Vector3 AO = -A;

		//float ABdotAO = AB.Dot(AO);
		//float t = AB.Dot(AO) / AB.LengthSquared();
		//t = std::clamp(t, 0.0f, 1.0f);

		//SimpleMath::Vector3 closestPoint = A + AB * t;
		//float distance = closestPoint.Length();

		//if (distance < 1e-6f)
		//{
		//	return true;
		//}
		//else
		//{
		//	return false;
		//}

		//float t = ABdotAO / AB.LengthSquared();//(AB * ABで割るとエラーになるぞ！ベクトルでの割り算は存在しないため）
		//SimpleMath::Vector3 closestPoint;
		//if (0 < t && t < 1)
		//{
		//	closestPoint = A + AB * t;//AからスタートしてAB*t倍進んだところ
		//	//	float test = closestPoint.Length();
		//}
		//else if (t <= 0)
		//{
		//	closestPoint = A;
		//}
		//else if (t >= 1)
		//{
		//	closestPoint = B;
		//}

		//if (0 <= ABdotAO && ABdotAO <= AB.LengthSquared() && closestPoint.Length() < 1e-6f)//線分が原点を含包するか
		//{
		//	return true;
		//}
		//else
		//{
		//	return false;
		//}

	}

	if (index == 3)
	{
		SimpleMath::Vector3 A, B, C, AB, BC, CA,closestPointAB, closestPointBC, closestPointCA;
		A = simplex[2];
		B = simplex[1];
		C = simplex[0];
		AB = B - A;
		BC = C - B;
		CA = A - C;

		//線分のどこらへんに原点があるか算出
		float t_AB = -A.Dot(AB) / AB.LengthSquared();
		float t_BC = -B.Dot(BC) / BC.LengthSquared();
		float t_CA = -C.Dot(CA) / CA.LengthSquared();

		//最接近点を求める
		// 原点が線分の外側にあるとき、延長線との距離になってしまうので、Clampで制限しています）
		closestPointAB = A + AB * clamp(t_AB, 0.0f,1.0f);
		closestPointBC = B + BC * clamp(t_BC, 0.0f, 1.0f);
		closestPointCA = C + CA * clamp(t_CA, 0.0f, 1.0f);
		
		if (closestPointAB.Length() < 1e-6f || closestPointBC.Length() < 1e-6f || closestPointCA.Length() < 1e-6)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool CollisionDetection::HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d) {
	// 四面体の頂点
	const SimpleMath::Vector3& A = simplex[3];
	const SimpleMath::Vector3& B = simplex[2];
	const SimpleMath::Vector3& C = simplex[1];
	const SimpleMath::Vector3& D = simplex[0];

	// 原点に向かうベクトル
	SimpleMath::Vector3 AO = -A;

	// 各面の法線
	SimpleMath::Vector3 ABCNormal = (B - A).Cross(C - A);// ABC面
	SimpleMath::Vector3 ACDNormal = (C - A).Cross(D - A);// ACD面
	SimpleMath::Vector3 ADBNormal = (D - A).Cross(B - A);// ADB面

	// 法線を内向きに揃える ・・・右手系だし、いらんのでは？
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
		d.Normalize();
		//m_polytope.erase(m_polytope.begin() + 0);
		return false;
	}

	if (ACDNormal.Dot(AO) < 0)
	{
		simplex = { D, C, A };
		index = 3;
		d = -ACDNormal;
		d.Normalize();
		//m_polytope.erase(m_polytope.begin() + 2);
		return false;
	}

	if (ADBNormal.Dot(AO) < 0)
	{
		simplex = { B, D, A };
		index = 3;
		d = -ADBNormal;
		d.Normalize();
		//m_polytope.erase(m_polytope.begin() + 1);
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

		SimpleMath::Vector3 normal = ab.Cross(ac);

		SimpleMath::Vector3 acperp = normal.Cross(ac);//外向き法線ac.Cross(ab).Cross(ac)

		//acperp.Dot(ao) <= 0 かつ abperp.Dot(ao) <= 0であれば三角形の内側に原点がある

		if (acperp.Dot(ao) > 0)//原点がACの外にいるかどうか
		{
			//点Cを捨て再度三角形を作成
			simplex[0] = simplex[1];
			simplex[1] = simplex[2];
			index = 2;
			direction = acperp;
			direction.Normalize();
			return false;
		}

		SimpleMath::Vector3 abperp = normal.Cross(ab);//ab.Cross(ac).Cross(ab)

		if (abperp.Dot(ao) > 0)//原点がABの外にいるかどうか
		{
			//点Cを捨て再度三角形を作成
			simplex[0] = simplex[1];
			simplex[1] = simplex[2];
			index = 2;
			direction = abperp;
			direction.Normalize();
			return false;
		}

		//ここまで来たら原点が三角形の内側にある→四面体に点を増やす

		/*SimpleMath::Vector3 normal = ab.Cross(ac);*///面の法線
		if (normal.Dot(ao) < 0)//原点方向に探索
		{
			normal = -normal;
		}
		direction = normal;
		direction.Normalize();
		return true;
}

bool CollisionDetection::GJK(const BoxCollider* collider_1,const BoxCollider* collider_2, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;//重複チェック用のコンテナ
	int maxDuplication = 5;
	int duplicationCount = 0;

	const array<SimpleMath::Vector3, 8> vertices_1 = collider_1->GetWorldVertices();
	const array<SimpleMath::Vector3, 8> vertices_2 = collider_2->GetWorldVertices();

	array<SimpleMath::Vector3, g_maxSimplexSize> simplex;
	SimpleMath::Vector3 ao,bo, ab, ac, a, b, c, abperp, acperp;

	size_t index = 0;//現在のsimplexの頂点数

	//コライダーの中心点
	SimpleMath::Vector3 center_1 = collider_1->ComputeCenter();
	SimpleMath::Vector3 center_2 = collider_2->ComputeCenter();

	//1点目
	SimpleMath::Vector3 d = center_1 - center_2;
	

	if (d.LengthSquared() < 1e-6f)
	{
		d = SimpleMath::Vector3::UnitX;//中心点が同じ時の処理
	}
	d.Normalize();

	a = simplex[0] = Support(vertices_1, vertices_2, d);
	CheckDuplicationAndSet(visitedPoints, a);
	index++;

	if (a.Dot(d) < 0)
	{
		return false;
	}

	//2点目
	d = -a;

	if (d.LengthSquared() < 1e-6f)
	{
		//接触→EPA
		return true;
	}
	d.Normalize();

	a = Support(vertices_1, vertices_2, d);

	CheckDuplicationAndSet(visitedPoints, a);
	simplex[index] = a;
	index++;

	if (a.Dot(d) < 0)
	{
		//原点にそれ以上近づけない→GJKを終了し線分との接触を判定
		//ムリだったのでむりやり三角形にして内側にあるかを判定

			//ABとAOが作る平面（または直線）の“垂直方向”に補助点を取る
			SimpleMath::Vector3 A = simplex[1];
			SimpleMath::Vector3 B = simplex[0];
			SimpleMath::Vector3 AB = B - A;
			SimpleMath::Vector3 AO = -A;
			SimpleMath::Vector3 normal = AB.Cross(AO);
			a = simplex[index] = Support(vertices_1, vertices_2, d);
			index++;
		if (CheckDuplicationAndSet(visitedPoints,a) == false)
		{
			//退化ケース
			return false;
		}

		if(SimplexHitTest(simplex,index) == true)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//3点目
	d = ao = -a;

	if (d.LengthSquared() < 1e-6f)
	{
		if (SimplexHitTest(simplex, index) == true)
		{
			//接触→EPA
			return true;
		}
		return false;
	}
	d.Normalize();

	int count = 0;
	while (count++ < 50)
	{
		a = Support(vertices_1, vertices_2, d);

		if (a.Dot(d) < 0 )
		{
			if (SimplexHitTest(simplex, index) == true)
			{
				//接触→EPA
				return true;
			}
			return false;
		}

		if (CheckDuplicationAndSet(visitedPoints,a) == false)
		{
			duplicationCount++;
			if (duplicationCount >= maxDuplication)
			{
				if (SimplexHitTest(simplex, index) == true)
				{
					//接触→EPA
					return true;
				}
				return false;
			}
			//探索方向dの変更
			
			b = simplex[0];
			ab = b - simplex[1];
			ao = -simplex[1];
			d = ab.Cross(ao).Cross(ab);

			if (d.LengthSquared() < 1e-6f)
			{
				if (SimplexHitTest(simplex, index) == true)
				{
					//接触→EPA
					return true;
				}
				return false;
			}
			continue;
		}

		simplex[index] = a;//3点目
		index++;

		if (HandleTriangle(simplex, index, d))
		{
			break;
		}
	}

	//4点目
	count = 0;//無限ループ防止
	while (count++ < 50)
	{
		a = Support(vertices_1, vertices_2, d);

		if (a.Dot(d) < 0)
		{
			if (SimplexHitTest(simplex, index) == true)
			{
				//接触→EPA
				return true;
			}
			return false;
		}

		if (CheckDuplicationAndSet(visitedPoints, a) == false)
		{
			duplicationCount++;
			if (duplicationCount >= maxDuplication)
			{
				if (SimplexHitTest(simplex, index) == true)
				{
					//接触→EPA
					return true;
				}
				return false;
			}
			//ここでは探索方向の変更を行いcontinue
			continue;
		}

		simplex[index] = a;
		index++;

		if (HandleTetrahedron(simplex, index, d))
		{
			info.hasValue = true;
			//info = EPA(collider_1, collider_2, simplex,info);
			return true;
		}
	}
	return false;
}

