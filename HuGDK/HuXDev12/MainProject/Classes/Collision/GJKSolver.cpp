//
// GJKSolver.cpp
//

#include "GJKSolver.h"

bool GJKSolver::GJK(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;//重複チェック用のコンテナ
	int maxDuplication = 1000;//ToDo:なくす予定
	int duplicationCount = 0;
	int gjkMaxIter = 1000;

	bool checkSimplex = false;

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

	a = simplex[0] = Support(vertices_A, vertices_B, d);
	CheckDuplicationAndSet(visitedPoints, a);
	index++;

	//if (a.Dot(d) < 0)
	//{
	//	return false;
	//}//1点目はなんも考えず追加

	//2点目
	d = -a;

	//if (d.LengthSquared() < 1e-6f)
	//{
	//	//接触→EPA　1点目が取れてるなら必ずこれも有効なので消した
	//	return true;
	//}

	for (int i = 0; i < gjkMaxIter; i++)
	{

		a = Support(vertices_A, vertices_B, d);

		if (a.Dot(d) < 0)
		{
			//交差していない　status = -1;
			break;
		}

		CheckDuplicationAndSet(visitedPoints, a);//重複した場合のロジック追加
		simplex[index] = a;
		index++;

		//シンプレックスと原点の衝突判定
		//方向を更新
		if (HandleSimplex(simplex, index, d) == true)//衝突
		{
			break;
		}
		else
		{//非衝突
			break;
		}

		//更新後の方向の長さチェック
		if (d.LengthSquared() < 1e-6f)
		{
			//status = -1;
			break;
		}
	}


	//以下CheckConvergence()　4まで育てながらgjkが収束してるかチェック
	while (true) {

		float squaredDistance = numeric_limits<float>::max();

		//もう一度サポート点を取得し距離を測る
		SimpleMath::Vector3 checkPoint;
		checkPoint = Support(vertices_A, vertices_B, d);

		//delta = 今の探索方向に対して、新しいサポート点がどれくらい前進できたか。
		float delta = checkPoint.Dot(d);

		// 内積が0より大きい　かつ　新サポート点が、もう原点に届かないレベルで遠い場合
		if (delta > 0 && delta * delta > squaredDistance)//&& (delta * delta > squaredDistance * input.m_maximumDistanceSquared)
		{
			checkSimplex = true;
			break;
		}

		if (!CheckDuplicationAndSet(visitedPoints, checkPoint)) {//重複チェック　checkduplicationを流用
			checkSimplex = true;
			break;
		}
		
		float approach = squaredDistance - delta;//「距離がどれくらい改善されたか」
		float relativeEpsilon = squaredDistance * 1e-6f;//「収束とみなしてよい、最小限の変化幅」REL_ERROR2は超小さい点。

		if (approach <= relativeEpsilon)//近づけなかった時
		{
			if (approach < 1e-6f)
			{
				//		//完全に停滞した（ゼロ進捗）＝極端な退化ケースの発生
				//		m_degenerateSimplex = 2;
			}
			else
			{
				//		//「ゼロじゃないけど、進捗が超少ない」
				//		m_degenerateSimplex = 11;
			}

			checkSimplex = true;
			break;
		}

		//上にあてはまらなかったら新しい有効なサポート点なのでシンプレックスに追加、最接近点を求める
		CheckDuplicationAndSet(visitedPoints, checkPoint);
		simplex[index] = checkPoint;
		index++;

		//closest()で最接近点を求める
		SimpleMath::Vector3 closestPoint;
		if (ComputeClosestPoint(simplex, index, closestPoint) == false) {
			//	m_degenerateSimplex = 3;
			checkSimplex = true;
			break;
		}

		if (closestPoint.LengthSquared() < 1e-6f) {
			//closestPointを分離軸として保存
			// m_degenerateSimplex = 6;
			checkSimplex = true;
			break;
		}

		//最短ベクトル探すやつ
		float previousSquaredDistance = squaredDistance;
		squaredDistance = closestPoint.LengthSquared();

		if (previousSquaredDistance - squaredDistance <= 1e-6f * previousSquaredDistance) {
			checkSimplex = true;
			//m_degenerateSimplex = 12;
			break;
		}

		d = closestPoint;

		//無限ループ対策ここに入れる

	//if ループが基準いじょうになったら　エラー　break;
		bool maxIndex = (index == 4);

		if (maxIndex)
		{
			//m_degenerateSimplex = 13;//シンプレックスがいっぱいで探索できずに終わった系の退化
			break;//checkSimplexは行わない。
		}
	}

	//デストラクタ的な処理
	if (checkSimplex)//checkSimplex
	{
		SimpleMath::Vector3 closest;
		ComputeClosestPoint(simplex, index, closest);

		float a = closest.LengthSquared();

		if (closest.LengthSquared() <= 0.1f)
		{
			return true;
		}
		
		/*今のシンプレックス（1～4点）上で、
		　原点に最も近い点を探して
		　そのときの A, B それぞれの最近点を出す*/

		//この時点で衝突深度を近似できる
		//回転トルクも求められる

	}

	//以下EPAつかうなら使う。
	return false;
}

bool GJKSolver::HandleSimplex(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {
	switch (index) {
	case 2:
		return HandlePoint(simplex, index, direction);
		break;
	case 3:
		return HandleTriangle(simplex, index, direction);
		break;
	case 4:
		return HandleTetrahedron(simplex, index, direction);
		break;
	default:
		break;
	}

	return false;
}

bool GJKSolver::ComputeClosestPoint(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& closest)
{
	switch (index) {
	case 0:

		return false;
		
		break;
	case 1:

		closest = simplex[0];

		break;
	case 2:

		closest = ClosestPointOnSegment(simplex);

		break;
	case 3:

		closest = ClosestPointOnTriangle(simplex);

		break;
	case 4:

		closest = ClosestPointOnTetrahedron(simplex);

		break;
	default:
		return false;
		break;
	}

	return true;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnSegment(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = simplex[1];
	B = simplex[0];
	AB = B - A;
	AO = -A;

	float abDotab = AB.Dot(AB);
	if (abDotab == 0) {
		return A;		//A == B
	}

	float t = AB.Dot(AO) / abDotab;
	t = std::clamp(t, 0.0f, 1.0f);//線分の中に制限する

	return A + t * AB;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnSegment(SimpleMath::Vector3 pointA, SimpleMath::Vector3 pointB) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = pointA;
	B = pointB;
	AB = B - A;
	AO = -A;

	float abDotab = AB.Dot(AB);
	if (abDotab == 0) {
		return A;		//A == B
	}

	float t = AB.Dot(AO) / abDotab;
	t = std::clamp(t, 0.0f, 1.0f);//線分の中に制限する

	return A + t * AB;

}

SimpleMath::Vector3 GJKSolver::ClosestPointOnTriangle(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex) {
	//原点 (0, 0, 0) から三角形平面に投影して、その点が三角形内にあるかを判定する
	//三角形外なら、線分AB,BC,CAのうち最も近い線分の中の一点を計算して返す
	SimpleMath::Vector3 A, B, C, AB, AC,normal;
	A = simplex[2];
	B = simplex[1];
	C = simplex[0];

	AB = B - A;
	AC = C - A;
	normal = AB.Cross(AC);

	float area = normal.Length();
	if (area < 1e-6f)//三角形がつぶれている場合
	{
		SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B);
		SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C);
		SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A);

		float dAB = pAB.LengthSquared();
		float dBC = pBC.LengthSquared();
		float dCA = pCA.LengthSquared();

		if (dAB <= dBC && dAB <= dCA) return pAB;
		if (dBC <= dCA) return pBC;
		return pCA;
	}

	normal.Normalize();

	float distance = A.Dot(normal);
	SimpleMath::Vector3 projected = -distance * normal;

	SimpleMath::Vector3 v0 = AB;
	SimpleMath::Vector3 v1 = AC;
	SimpleMath::Vector3 v2 = projected - A;

	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f)
		return A;

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// 三角形内部かどうかチェック
	if (u >= 0 && v >= 0 && w >= 0)
	{
		return u * A + v * B + w * C;
	}

	// 外部なら辺上の最近点を探す
	SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B);
	SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C);
	SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A);

	float dAB = pAB.LengthSquared();
	float dBC = pBC.LengthSquared();
	float dCA = pCA.LengthSquared();

	if (dAB <= dBC && dAB <= dCA) return pAB;
	if (dBC <= dCA) return pBC;
	return pCA;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnTriangle(SimpleMath::Vector3 pointA, SimpleMath::Vector3 pointB, SimpleMath::Vector3 pointC) {
	SimpleMath::Vector3 A, B, C, AB, AC, normal;
	A = pointA;
	B = pointB;
	C = pointC;

	AB = B - A;
	AC = C - A;
	normal = AB.Cross(AC);

	float area = normal.Length();
	if (area < 1e-6f)//三角形がつぶれている場合
	{
		SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B);
		SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C);
		SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A);

		float dAB = pAB.LengthSquared();
		float dBC = pBC.LengthSquared();
		float dCA = pCA.LengthSquared();

		if (dAB <= dBC && dAB <= dCA) return pAB;
		if (dBC <= dCA) return pBC;
		return pCA;
	}

	normal.Normalize();

	float distance = A.Dot(normal);
	SimpleMath::Vector3 projected = -distance * normal;

	SimpleMath::Vector3 v0 = AB;
	SimpleMath::Vector3 v1 = AC;
	SimpleMath::Vector3 v2 = projected - A;

	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f)
		return A;

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// 三角形内部かどうかチェック
	if (u >= 0 && v >= 0 && w >= 0)
	{
		return u * A + v * B + w * C;
	}

	// 外部なら辺上の最近点を探すケロ
	SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B);
	SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C);
	SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A);

	float dAB = pAB.LengthSquared();
	float dBC = pBC.LengthSquared();
	float dCA = pCA.LengthSquared();

	if (dAB <= dBC && dAB <= dCA) return pAB;
	if (dBC <= dCA) return pBC;
	return pCA;
}

SimpleMath::Vector3 GJKSolver::ClosestPointOnTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex) {
	SimpleMath::Vector3 A, B, C, D;
	A = simplex[3];
	B = simplex[2];
	C = simplex[1];
	D = simplex[0];

	// 四面体の4つの面を構成する三角形
	SimpleMath::Vector3 pABC = ClosestPointOnTriangle(A, B, C);
	SimpleMath::Vector3 pACD = ClosestPointOnTriangle(A, C, D);
	SimpleMath::Vector3 pABD = ClosestPointOnTriangle(A, B, D);
	SimpleMath::Vector3 pBCD = ClosestPointOnTriangle(B, C, D);

	float dABC = pABC.LengthSquared();
	float dACD = pACD.LengthSquared();
	float dABD = pABD.LengthSquared();
	float dBCD = pBCD.LengthSquared();

	// 一番近いやつを返すケロ
	if (dABC <= dACD && dABC <= dABD && dABC <= dBCD) return pABC;
	if (dACD <= dABD && dACD <= dBCD) return pACD;
	if (dABD <= dBCD) return pABD;
	return pBCD;
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

bool GJKSolver::HandlePoint(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {
	SimpleMath::Vector3 A, B, AB, AO; 
	A = simplex[1];
	B = simplex[0];
	AB = B - A;
	AO = -A;

	// 原点とAが同じ位置なら含まれるとみなす
	if (A.LengthSquared() < 1e-6f) return true;
	if (B.LengthSquared() < 1e-6f) return true;

	// 原点が線分AB上にあるなら、AOとABは同じ方向（または逆方向）で内積が正、かつ t ∈ [0,1]
	float abLenSq = AB.LengthSquared();
	if (abLenSq < 1e-6f) return false; // ABがほぼゼロ長なら false

	float t = AB.Dot(AO) / abLenSq;

	// 原点がAとBの間にあるかチェック
	if (t < 0.0f || t > 1.0f)
		return false;

	// 線分上の最近点との距離がほぼゼロかチェック
	SimpleMath::Vector3 closestPoint = A + t * AB;
	return closestPoint.LengthSquared() < 1e-6f;
}

bool GJKSolver::HandleTriangle(std::array<SimpleMath::Vector3, 4>& simplex, size_t& index, SimpleMath::Vector3& direction) {

	const SimpleMath::Vector3& a = simplex[2];
	const SimpleMath::Vector3& b = simplex[1];
	const SimpleMath::Vector3& c = simplex[0];

	SimpleMath::Vector3 ao = -a;//原点方向
	SimpleMath::Vector3 ab = b - a;
	SimpleMath::Vector3 ac = c - a;

	SimpleMath::Vector3 normal = ab.Cross(ac);

	//原点と各頂点が一致する　＝　三角形の上に原点がある
	if (ao == a || ao == b || ao == c) {
		

		if (normal.Dot(ao) < 0)//原点方向に探索
		{
			normal = -normal;
		}
		direction = normal;
		return true;
	}

	SimpleMath::Vector3 acperp = normal.Cross(ac);//外向き法線ac.Cross(ab).Cross(ac)

	//acperp.Dot(ao) <= 0 かつ abperp.Dot(ao) <= 0であれば三角形の内側に原点がある

	float aiueo = acperp.Dot(ao);

	if (acperp.Dot(ao) > 0)//原点がACの外にいるかどうか
	{
		//点Cを捨て再度三角形を作成
		simplex[0] = simplex[1];
		simplex[1] = simplex[2];
		index = 2;
		direction = acperp;
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
		return false;
	}

	//ここまで来たら原点が三角形の内側にある→四面体に点を増やす

	/*SimpleMath::Vector3 normal = ab.Cross(ac);*///面の法線
	direction = normal;
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

