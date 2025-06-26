//
// GJKSolver.cpp
//

#include "GJKSolver.h"

/// <summary>
/// サポート関数を用いて2つのコライダーの頂点からミンコフスキ差図形上の点を取得し、その点が形成する図形が原点(0,0)を含むかどうかで衝突検出を行います。
/// </summary>
/// <param name="info">衝突情報を入れる構造体</param>
/// <returns>衝突したかどうか</returns>
bool GJKSolver::GJK(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;//重複チェック用のコンテナ

	bool checkSimplex = false;
	bool isDegenerate = false;
	bool isContact = false;

	const vector<SimpleMath::Vector3> vertices_A = collider_A->GetWorldVertices();
	const vector<SimpleMath::Vector3> vertices_B = collider_B->GetWorldVertices();

	//シンプレックスの頂点を保存する配列。
	array<PointInfo, g_maxSimplexSize> simplex;

	PointInfo newPoint;
	SimpleMath::Vector3 ao, bo, ab, ac, b, c, abperp, acperp;

	size_t index = 0;//現在のsimplexの頂点数

	//コライダーの中心点
	SimpleMath::Vector3 center_A = collider_A->ComputeCenter();
	SimpleMath::Vector3 center_B = collider_B->ComputeCenter();

	//探索方向
	SimpleMath::Vector3 d = center_A - center_B;

	if (d.LengthSquared() < 1e-6f)
	{
		d = SimpleMath::Vector3::UnitX;//中心点が同じ時はX方向に探索
	}

	newPoint = simplex[0] = Support(vertices_A, vertices_B, d);
	CheckDuplicationAndSet(visitedPoints, newPoint.point);
	index++;

	//2点目以降の探索
	d = -newPoint.point;
	for (int i = 0; i < m_gjkMaxIter; i++)
	{

		newPoint = Support(vertices_A, vertices_B, d);

		if (newPoint.point.Dot(d) < 0)
		{
			//交差していない
			break;
		}

		if (!CheckDuplicationAndSet(visitedPoints, newPoint.point))
		{
			isDegenerate = true;
			break;
		}
		simplex[index] = newPoint;
		index++;
		
		if (HandleSimplex(simplex, index, d) == true)//衝突
		{
			isContact = true;
			break;
		}

		//更新後の方向の長さチェック
		if (d.LengthSquared() < 1e-6f)
		{
			isContact = true;
			break;
		}
	}

	//ここからシンプレックスを育てながらgjkが収束しているかどうかや正確な最接近点、衝突情報チェックしています。

	int count = 0;

	while (index < g_maxSimplexSize || !isDegenerate || count <= m_gjkMaxIter || !isContact) {

		count++;

		float squaredDistance = numeric_limits<float>::max();

		//もう一度サポート点を取得し距離を測る
		PointInfo checkPoint;
		checkPoint = Support(vertices_A, vertices_B, d);

		//delta = 今の探索方向に対して、新しいサポート点がどれくらい前進できたか。
		float delta = checkPoint.point.Dot(d);

		if (!CheckDuplicationAndSet(visitedPoints, checkPoint.point)) {//重複チェック
			isDegenerate = true;
			break;
		}
		
		float approach = squaredDistance - delta;//距離がどれくらい改善されたか
		float relativeEpsilon = squaredDistance * 1e-6f;//収束とみなしてよい、最小限の変化幅

		if (approach <= relativeEpsilon)//近づけなかった かつ　シンプレックス完成してなかったとき
		{
			isDegenerate = true;
			break;
		}

		//ここまで来たら有効なサポート点なのでシンプレックスに追加し、最接近点を求める
		simplex[index] = checkPoint;
		index++;

		//最接近点を求める
		PointInfo closestPointInfo;
		if (ComputeClosestPoint(simplex, index, closestPointInfo) == false) {//indexの値に異常がある
			return false;
		}

		if (closestPointInfo.point.LengthSquared() < 1e-6f) {//接触
			isContact = true;
			break;
		}

		//最短ベクトル探す
		float previousSquaredDistance = squaredDistance;
		squaredDistance = closestPointInfo.point.LengthSquared();

		if (previousSquaredDistance - squaredDistance <= 1e-6f * previousSquaredDistance) {//接触
			isContact = true;
			break;
		}

		//次ループの探索方向
		d = closestPointInfo.point;
	}

	PointInfo closestPointInfo;
	ComputeClosestPoint(simplex, index, closestPointInfo);

	if (closestPointInfo.point.LengthSquared() <= m_closestPointEpsilon)
	{

		if (index < g_maxSimplexSize) {//シンプレックスが4点まで育っていなかった場合の処理

			bool Contact = TryEncloseOrigin(vertices_A, vertices_B, simplex, index);
			if (!Contact && !isContact) {
				return false;
			}

			ComputeClosestPoint(simplex, index, closestPointInfo);
		}

		info.hasValue = true; 

		//現在は必ずEPAにつなげていますが、退化や値のパターンにより処理を分岐させる予定です。
		bool calculateNormal = m_epaSolver.EPA(collider_A, collider_B, simplex, index, info);

		if (info.penetrationDepth < m_penetrationEpsilon || calculateNormal) {
			return true;
		}
	}
		
	info.hasValue = false;
	return false;
}

/// <summary>
/// シンプレックスが原点を含むかどうかを判定します。
/// </summary>
bool GJKSolver::HandleSimplex(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {

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

/// <summary>
/// シンプレックスが2点の場合原点を含むかどうかを判定します。
/// </summary>
bool GJKSolver::HandlePoint(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = simplex[1].point;
	B = simplex[0].point;
	AB = B - A;
	AO = -A;

	// 原点とAがほぼ同じ位置の場合
	if (A.LengthSquared() < 1e-6f) {return true;}
	if (B.LengthSquared() < 1e-6f) {return true;}

	// 原点が線分AB上にあるなら、AOとABは同じ方向（または逆方向）で内積が正、かつ t ∈ [0,1]
	float abLenSq = AB.LengthSquared();
	if (abLenSq < 1e-6f) { return false; }

	float t = AB.Dot(AO) / abLenSq;

	// 原点がAとBの間にあるかチェック
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	// 線分上の最近点との距離がほぼゼロかチェック
	SimpleMath::Vector3 closestPoint = A + t * AB;

	return closestPoint.LengthSquared() < 1e-6f;
}

/// <summary>
/// シンプレックスが3点の場合原点を含むかどうかを判定します。
/// </summary>
bool GJKSolver::HandleTriangle(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction) {

	const SimpleMath::Vector3& A = simplex[2].point;
	const SimpleMath::Vector3& B = simplex[1].point;
	const SimpleMath::Vector3& C = simplex[0].point;

	SimpleMath::Vector3 AO = -A;//原点方向
	SimpleMath::Vector3 AB = B - A;
	SimpleMath::Vector3 AC = C - A;

	SimpleMath::Vector3 normal = AB.Cross(AC);

	//原点と各頂点が一致する　＝　三角形の上に原点がある
	if (AO == A || AO == B || AO == C) {


		if (normal.Dot(AO) < 0)//原点方向に探索
		{
			normal = -normal;
		}
		direction = normal;
		return true;
	}

	//外向き法線
	SimpleMath::Vector3 acperp = normal.Cross(AC);

	if (acperp.Dot(AO) > 0)//原点がACの外にいるかどうか
	{
		//点Cを捨て再度三角形を作成
		simplex[0] = simplex[1];
		simplex[1] = simplex[2];
		index = 2;
		direction = acperp;
		return false;
	}

	SimpleMath::Vector3 abperp = normal.Cross(AB);

	if (abperp.Dot(AO) > 0)//原点がABの外にいるかどうか
	{
		//点Cを捨て再度三角形を作成
		simplex[0] = simplex[1];
		simplex[1] = simplex[2];
		index = 2;
		direction = abperp;
		return false;
	}

	//ここまで来たら原点が三角形の内側にある
	direction = normal;
	return true;
}

/// <summary>
/// シンプレックスが4点の場合原点を含むかどうかを判定します。
/// </summary>
bool GJKSolver::HandleTetrahedron(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d) {
	// 四面体の頂点
	const SimpleMath::Vector3& A = simplex[3].point;
	const SimpleMath::Vector3& B = simplex[2].point;
	const SimpleMath::Vector3& C = simplex[1].point;
	const SimpleMath::Vector3& D = simplex[0].point;

	// 原点に向かうベクトル
	SimpleMath::Vector3 AO = -A;

	// 各面の法線
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

	ABCNormal.Normalize();
	ACDNormal.Normalize();
	ADBNormal.Normalize();

	//原点が含まれるか
	if (ABCNormal.Dot(AO) < 0)
	{
		simplex = { C, B, A };
		index = 3;
		d = -ABCNormal;
		return false;
	}

	if (ACDNormal.Dot(AO) < 0)
	{
		simplex = { D, C, A };
		index = 3;
		d = -ACDNormal;
		return false;
	}

	if (ADBNormal.Dot(AO) < 0)
	{
		simplex = { B, D, A };
		index = 3;
		d = -ADBNormal;
		return false;
	}
	//全ての面の内向き法線が原点方向 → 四面体が原点を囲んでいる
	return true;
}

/// <summary>
/// 最接近点を計算します。
/// </summary>
/// <param name="closestPointInfo">最接近点の情報を入力します</param>
/// <returns>false indexの値に異常がある場合</returns>
bool GJKSolver::ComputeClosestPoint(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, PointInfo& closestPointInfo)
{
	switch (index) {
	case 0:

		return false;
		
		break;
	case 1:

		closestPointInfo = simplex[0];

		break;
	case 2:

		ClosestPointOnSegment(simplex, closestPointInfo);

		break;
	case 3:

		ClosestPointOnTriangle(simplex, closestPointInfo);

		break;
	case 4:

		ClosestPointOnTetrahedron(simplex, closestPointInfo);

		break;
	default:

		return false;
		break;
	}

	return true;
}

/// <summary>
/// シンプレックスが線の時の最接近点を計算します。
/// </summary>
SimpleMath::Vector3 GJKSolver::ClosestPointOnSegment(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = simplex[1].point;
	B = simplex[0].point;
	AB = B - A;
	AO = -A;

	float abDotab = AB.Dot(AB);
	if (abDotab == 0) {

		closestPointInfo = simplex[1];
		return A;		//A == B
	}

	float t = AB.Dot(AO) / abDotab;
	t = std::clamp(t, 0.0f, 1.0f);//線分の中に制限する

	closestPointInfo.point = A + t * AB;
	closestPointInfo.supA = simplex[1].supA * (1.0f - t) + simplex[0].supA * t;
	closestPointInfo.supB = simplex[1].supB * (1.0f - t) + simplex[0].supB * t;

	return closestPointInfo.point;
}

/// <summary>
/// 2点の間で最も原点に近い座標を計算します。
/// </summary>
SimpleMath::Vector3 GJKSolver::ClosestPointOnSegment(PointInfo& pointA, PointInfo& pointB, PointInfo& closestPointInfo) {
	SimpleMath::Vector3 A, B, AB, AO;
	A = pointA.point;
	B = pointB.point;
	AB = B - A;
	AO = -A;

	float abDotab = AB.Dot(AB);
	if (abDotab == 0) {
		closestPointInfo = pointA;
		return A;		//A == B
	}

	float t = AB.Dot(AO) / abDotab;
	t = std::clamp(t, 0.0f, 1.0f);//線分の中に制限する

	closestPointInfo.point = A + t * AB;
	closestPointInfo.supA = pointA.supA * (1.0f - t) + pointB.supA * t;
	closestPointInfo.supB = pointA.supB * (1.0f - t) + pointB.supB * t;

	return A + t * AB;

}

/// <summary>
/// シンプレックスが三角形の時の最接近点を計算します。
/// 原点 (0, 0, 0) から三角形平面に投影して、その点が三角形内にあるかを判定します。
/// 三角形外なら、線分AB,BC,CAのうち最も近い線分の中の一点を計算して返します。
/// </summary>
SimpleMath::Vector3 GJKSolver::ClosestPointOnTriangle(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo) {
	PointInfo A, B, C;
	SimpleMath::Vector3 AB, AC, normal;
	A = simplex[2];
	B = simplex[1];
	C = simplex[0];

	AB = B.point - A.point;
	AC = C.point - A.point;
	normal = AB.Cross(AC);

	float area = normal.Length();
	if (area < 1e-6f)//三角形がつぶれている場合
	{
		SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B,closestPointInfo);
		SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C, closestPointInfo);
		SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A, closestPointInfo);

		float dAB = pAB.LengthSquared();
		float dBC = pBC.LengthSquared();
		float dCA = pCA.LengthSquared();

		if (dAB <= dBC && dAB <= dCA) return pAB;
		if (dBC <= dCA) return pBC;
		return pCA;
	}

	normal.Normalize();

	float distance = A.point.Dot(normal);
	SimpleMath::Vector3 projected = -distance * normal;

	SimpleMath::Vector3 v0 = AB;
	SimpleMath::Vector3 v1 = AC;
	SimpleMath::Vector3 v2 = projected - A.point;

	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f)
	{
		closestPointInfo = simplex[2];
		return closestPointInfo.point;
	}

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// 三角形内部かどうかチェック
	if (u >= 0 && v >= 0 && w >= 0)
	{
		closestPointInfo.point = u * A.point + v * B.point + w * C.point;

		closestPointInfo.supA =
			simplex[2].supA * u +
			simplex[1].supA * v +
			simplex[0].supA * w;

		closestPointInfo.supB =
			simplex[2].supB * u +
			simplex[1].supB * v +
			simplex[0].supB * w;

		return closestPointInfo.point;
	}

	// 外部なら辺上の最近点を探す
	PointInfo closestAB, closestBC, closestCA;
	SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B, closestAB);
	SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C, closestBC);
	SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A, closestCA);

	float dAB = pAB.LengthSquared();
	float dBC = pBC.LengthSquared();
	float dCA = pCA.LengthSquared();

	if (dAB <= dBC && dAB <= dCA) {
		closestPointInfo = closestAB;
		return pAB;
	}
	if (dBC <= dCA) {
		closestPointInfo = closestBC;
		return pBC;
	}
	closestPointInfo = closestCA;
	return pCA;
}

/// <summary>
/// 3点の間で最も原点に近い座標を計算します。
/// 原点 (0, 0, 0) から三角形平面に投影して、その点が三角形内にあるかを判定します。
/// 三角形外なら、線分AB,BC,CAのうち最も近い線分の中の一点を計算して返します。
/// </summary>
SimpleMath::Vector3 GJKSolver::ClosestPointOnTriangle(PointInfo& pointA, PointInfo& pointB, PointInfo& pointC, PointInfo& closestPointInfo) {
	PointInfo A, B, C;
	SimpleMath::Vector3 AB, AC, normal;
	A = pointA;
	B = pointB;
	C = pointC;

	AB = B.point - A.point;
	AC = C.point - A.point;
	normal = AB.Cross(AC);

	float area = normal.Length();
	if (area < 1e-6f)//三角形がつぶれている場合
	{
		PointInfo ABtmp, BCtmp, CAtmp;

		SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B, ABtmp);
		SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C, BCtmp);
		SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A, CAtmp);

		float dAB = pAB.LengthSquared();
		float dBC = pBC.LengthSquared();
		float dCA = pCA.LengthSquared();

		if (dAB <= dBC && dAB <= dCA) { 
			closestPointInfo = ABtmp;
			return pAB; }

		if (dBC <= dCA) { 
			closestPointInfo = BCtmp;
			return pBC; }

		closestPointInfo = CAtmp;
		return pCA;
	}

	normal.Normalize();

	float distance = A.point.Dot(normal);
	SimpleMath::Vector3 projected = -distance * normal;

	SimpleMath::Vector3 v0 = AB;
	SimpleMath::Vector3 v1 = AC;
	SimpleMath::Vector3 v2 = projected - A.point;

	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f)
	{
		closestPointInfo = A;
		return A.point;
	}
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// 三角形内部かどうかチェック
	if (u >= 0 && v >= 0 && w >= 0)
	{
		closestPointInfo.point = u * A.point + v * B.point + w * C.point;
		closestPointInfo.supA = u * A.supA + v * B.supA + w * C.supA;
		closestPointInfo.supB = u * A.supB + v * B.supB + w * C.supB;

		return u * A.point + v * B.point + w * C.point;
	}

	// 外部なら辺上の最近点を探す
	PointInfo closestAB, closestBC, closestCA;
	SimpleMath::Vector3 pAB = ClosestPointOnSegment(A, B, closestAB);
	SimpleMath::Vector3 pBC = ClosestPointOnSegment(B, C, closestBC);
	SimpleMath::Vector3 pCA = ClosestPointOnSegment(C, A, closestCA);

	float dAB = pAB.LengthSquared();
	float dBC = pBC.LengthSquared();
	float dCA = pCA.LengthSquared();

	if (dAB <= dBC && dAB <= dCA) {
		closestPointInfo = closestAB;
		return pAB;
	}
	if (dBC <= dCA) {
		closestPointInfo = closestBC;
		return pBC;
	}
	closestPointInfo = closestCA;
	return pCA;
}

/// <summary>
/// シンプレックスが三角形の時の最接近点を計算します。
/// </summary>
SimpleMath::Vector3 GJKSolver::ClosestPointOnTetrahedron(array<PointInfo, g_maxSimplexSize>& simplex, PointInfo& closestPointInfo) {
	PointInfo A, B, C, D,tmpABC, tmpACD, tmpABD, tmpBCD;
	A = simplex[3];
	B = simplex[2];
	C = simplex[1];
	D = simplex[0];

	// 四面体の4つの面を構成する三角形それぞれの最接近点を求める
	SimpleMath::Vector3 pABC = ClosestPointOnTriangle(A, B, C, tmpABC);
	SimpleMath::Vector3 pACD = ClosestPointOnTriangle(A, C, D, tmpACD);
	SimpleMath::Vector3 pABD = ClosestPointOnTriangle(A, B, D, tmpABD);
	SimpleMath::Vector3 pBCD = ClosestPointOnTriangle(B, C, D, tmpBCD);

	float dABC = pABC.LengthSquared();
	float dACD = pACD.LengthSquared();
	float dABD = pABD.LengthSquared();
	float dBCD = pBCD.LengthSquared();

	// 一番近い値を返す
	if (dABC <= dACD && dABC <= dABD && dABC <= dBCD) {
		closestPointInfo = tmpABC;
		return pABC;
	}
		
	if (dACD <= dABD && dACD <= dBCD) {
		closestPointInfo = tmpACD;
		return pACD;
	}

	if (dABD <= dBCD) {
		closestPointInfo = tmpABD;
		return pABD;
	}

	closestPointInfo = tmpBCD;
	return pBCD;
}
