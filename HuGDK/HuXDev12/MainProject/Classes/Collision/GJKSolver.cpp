//
// GJKSolver.cpp
//

#include "GJKSolver.h"

bool GJKSolver::GJK(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;//重複チェック用のコンテナ
	int maxDuplication = 1000;//ToDo:なくす予定
	int duplicationCount = 0;

	const vector<SimpleMath::Vector3> vertices_A = collider_A->GetWorldVertices();
	const vector<SimpleMath::Vector3> vertices_B = collider_B->GetWorldVertices();

	array<SimpleMath::Vector3, g_maxSimplexSize> simplex;//シンプレックス

	SimpleMath::Vector3 ao, bo, ab, ac, a, b, c, abperp, acperp;

	size_t index = 0;//現在のsimplexの頂点数

	//コライダーの中心点
	SimpleMath::Vector3 center_A = collider_A->ComputeCenter();
	SimpleMath::Vector3 center_B = collider_B->ComputeCenter();

	//1点目
	SimpleMath::Vector3 d = center_A - center_B;


	if (d.LengthSquared() < 1e-6f)
	{
		d = SimpleMath::Vector3::UnitX;//中心点が同じ時の処理
	}
	d.Normalize();

	a = simplex[0] = Support(vertices_A, vertices_B, d);
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

	a = Support(vertices_A, vertices_B, d);

	CheckDuplicationAndSet(visitedPoints, a);//重複した場合のロジック追加
	simplex[index] = a;
	index++;

	if (a.Dot(d) < 0)
	{
		return false;
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
	duplicationCount = 0;
	while (count++ < 50)
	{
		a = Support(vertices_A, vertices_B, d);

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

			d.Normalize();
			
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
	duplicationCount = 0;
	while (count++ < 50)
	{
		a = Support(vertices_A, vertices_B, d);

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
			//info = m_epaSolver.EPA(collider_A, collider_B, simplex, info);
			return true;
		}
	}
	return false;
}

bool GJKSolver::SimplexHitTest(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index)
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

	}

	if (index == 3)
	{
		SimpleMath::Vector3 A, B, C, AB, BC, CA, closestPointAB, closestPointBC, closestPointCA;
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
		closestPointAB = A + AB * clamp(t_AB, 0.0f, 1.0f);
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


bool GJKSolver::HandleTriangle(std::array<SimpleMath::Vector3, 4>& simplex, size_t& index, SimpleMath::Vector3& direction) {

	const SimpleMath::Vector3& a = simplex[2];
	const SimpleMath::Vector3& b = simplex[1];
	const SimpleMath::Vector3& c = simplex[0];

	SimpleMath::Vector3 ao = -a;//原点方向
	SimpleMath::Vector3 ab = b - a;
	SimpleMath::Vector3 ac = c - a;

	SimpleMath::Vector3 normal = ab.Cross(ac);
	normal.Normalize();

	//原点と各頂点が一致する　＝　三角形の上に原点がある
	if (ao == a || ao == b || ao == c) {
		

		if (normal.Dot(ao) < 0)//原点方向に探索
		{
			normal = -normal;
		}
		direction = normal;
		direction.Normalize();
		return true;
	}

	SimpleMath::Vector3 acperp = normal.Cross(ac);//外向き法線ac.Cross(ab).Cross(ac)
	normal.Normalize();

	//acperp.Dot(ao) <= 0 かつ abperp.Dot(ao) <= 0であれば三角形の内側に原点がある

	float aiueo = acperp.Dot(ao);

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

	float kakiku = abperp.Dot(ao);

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
	direction = normal;
	direction.Normalize();
	return true;
}


bool GJKSolver::HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d) {
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

	ABCNormal.Normalize();
	ACDNormal.Normalize();
	ADBNormal.Normalize();

	//原点が含まれるか
	if (ABCNormal.Dot(AO) < 0)
	{
		simplex = { C, B, A };
		index = 3;
		d = -ABCNormal;
		//m_polytope.erase(m_polytope.begin() + 0);
		return false;
	}

	if (ACDNormal.Dot(AO) < 0)
	{
		simplex = { D, C, A };
		index = 3;
		d = -ACDNormal;
		//m_polytope.erase(m_polytope.begin() + 2);
		return false;
	}

	if (ADBNormal.Dot(AO) < 0)
	{
		simplex = { B, D, A };
		index = 3;
		d = -ADBNormal;
		//m_polytope.erase(m_polytope.begin() + 1);
		return false;
	}
	//全ての面の内向き法線が原点方向 → 四面体が原点を囲んでいる
	return true;

}

