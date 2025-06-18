
#include "EPASolver.h"

/// <summary>
/// GJKSolverから頂点情報を受け取りコライダー同士の衝突深度を計算し、ContactInfoに保存します。
/// </summary>
bool EPASolver::EPA(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, array<PointInfo, g_maxSimplexSize>& simplex, size_t& index, ContactInfo& info) {

	unordered_set<SimpleMath::Vector3, Vector3Hash, Vector3Equal> visitedPoints;

    m_vertices_1 = collider_A->GetWorldVertices();
	m_vertices_2 = collider_B->GetWorldVertices();

	if (index > 0 && EncloseOrigin(simplex,index)) {//シンプレックスが4点に満たなかった場合、4点に育てます。

		m_polytope.clear();
		m_polytope.insert(m_polytope.end(), simplex.begin(), simplex.end());

		for (int i = 0; i < m_polytope.size(); i++) {
			//重複チェック用コンテナ初期化
			CheckDuplicationAndSet(visitedPoints, m_polytope[i].point);
		}

		//頂点の順番を右手系に整える
		const SimpleMath::Vector3& DA = m_polytope[3].point - m_polytope[0].point;
		const SimpleMath::Vector3& DB = m_polytope[1].point - m_polytope[3].point;
		const SimpleMath::Vector3& DC = m_polytope[2].point - m_polytope[3].point;

		if (DA.Dot(DB.Cross(DC)) < 0) {

			swap(m_polytope[0], m_polytope[1]);
		}

		// 初期ポリトープ生成処理
		InitPolytope();

		FaceInfo* best = FindBest();
		FaceInfo currentFace = *best;
		PointInfo nextPoint;
		unsigned int pass = 0;
		unsigned int iterations = 0;
		BindFaces(m_FacesInfo[0], 0, m_FacesInfo[1], 0);
		BindFaces(m_FacesInfo[0], 1, m_FacesInfo[2], 0);
		BindFaces(m_FacesInfo[0], 2, m_FacesInfo[3], 0);
		BindFaces(m_FacesInfo[1], 1, m_FacesInfo[3], 2);
		BindFaces(m_FacesInfo[1], 2, m_FacesInfo[2], 1);
		BindFaces(m_FacesInfo[2], 2, m_FacesInfo[3], 1);

		//EPAメインループ
		for (int i = 0; i < m_EPAMaxIterations; i++) {

			Horizon horizon;
			bool success = true;
			best->pass = ++pass;

			//一番原点に近い面の法線方向にサポート点を探索します。
			nextPoint = Support(m_vertices_1, m_vertices_2, best->normal);

			if (CheckDuplicationAndSet(visitedPoints, nextPoint.point)) {//重複チェック
				break;
			}

			float distance = best->normal.Dot(nextPoint.point) - best->distanceToOrigin;
			float distanceEpsilon = 0.1;
			if (distance > distanceEpsilon) {

				//ポリトープ拡張
				for (int j = 0; (j < 3) && success; j++) {
					success = ExpandPolytope(pass,nextPoint,best->neighbors[j],best->neighborEdges[j],horizon);
					if (!success) {
						break;
					}
				}

				if (success && horizon.numberOfFaces >= 3) {
					BindFaces(horizon.currentFace,1, horizon.firstFace, 2);
					RemoveFace(best);
					best = FindBest();
					currentFace = *best;
				}
				else {
					// Failed;
					// 個別の処理を追加予定
					break;
				}
			}
			else {
				// AccuraryReached; 原点に近づき切った
				break;
			}
		}

		info.hasValue = true;
		info.normal = currentFace.normal;
		info.penetrationDepth = currentFace.distanceToOrigin;
		PointInfo contactPoint = GetContactPoint(currentFace);
		info.contactPointA = contactPoint.supA;
		info.contactPointB = contactPoint.supB;
		CollisionSupport::GenerateFrictionBasis(info.normal, info.tangent1, info.tangent2);

		for (FaceInfo* f : m_FacesInfo) {
			delete f;
		}
		m_FacesInfo.clear();

		return true;
	}

	return false;
}

/// <summary>
/// シンプレックスが4点に満たない場合、4点に増やしてシンプレックスが退化していないかを判定します。
/// </summary>
/// <param name="simplex">gjkで得られたシンプレックス</param>
/// <param name="index">シンプレックスの点の数</param>
bool EPASolver::EncloseOrigin(array<PointInfo, g_maxSimplexSize>& simplex, size_t& index) {

	SimpleMath::Vector3 UnitX = SimpleMath::Vector3::Right;
	SimpleMath::Vector3 UnitY = SimpleMath::Vector3::Up;
	SimpleMath::Vector3 UnitZ = SimpleMath::Vector3::Forward;

	array <SimpleMath::Vector3, 3> axis;

	SimpleMath::Vector3 A;
	SimpleMath::Vector3 B;
	SimpleMath::Vector3 C;
	SimpleMath::Vector3 D;
	
	switch (index) {
		case 1: {

			axis = { UnitX,UnitY,UnitZ };

			for (int i = 0; i < 3; i++) {

				simplex[index] = Support(m_vertices_1, m_vertices_2, axis[i]);
				index++;
			}

			if (EncloseOrigin(simplex, index)) {
				return true;
			}
			else {
				array <SimpleMath::Vector3, 3> axis = { -UnitX,-UnitY,-UnitZ };
				index = 1;
				for (int i = 0; i < 3; i++) {

					simplex[index] = Support(m_vertices_1, m_vertices_2, axis[i]);
					index++;
				}

				if (EncloseOrigin(simplex, index)) {
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
					simplex[2] = Support(m_vertices_1, m_vertices_2, p);
					index = 3;
					if (EncloseOrigin(simplex, index)) {
						return true;
					}
					else {
						p = -p;
						simplex[2] = Support(m_vertices_1, m_vertices_2, p);
						index = 3;
						if (EncloseOrigin(simplex, index)) {
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

			SimpleMath::Vector3 normal = (B - A).Cross(C - A);

			if (normal.LengthSquared() > 0) {

				simplex[3] = Support(m_vertices_1, m_vertices_2, normal);
				index = 4;
				if (EncloseOrigin(simplex, index)) {
					return true;
				}
				else {
					normal = -normal;
					simplex[3] = Support(m_vertices_1, m_vertices_2, normal);
					index = 4;
					if (EncloseOrigin(simplex, index)) {
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

			const SimpleMath::Vector3 AB = B - A;
			const SimpleMath::Vector3 AC = C - A;
			const SimpleMath::Vector3 AD = D - A;

			//四面体の体積
			float volume = abs(AB.Dot(AC.Cross(AD))) / 6.0f;

			if (volume > 0)
			{
				const SimpleMath::Vector3 P = SimpleMath::Vector3::Zero;

				return true;
			}

			return false;
			break;
		}
	}
	return false;
}

/// <summary>
/// 新しい点を使いポリトープを拡張します。
/// </summary>
bool EPASolver::ExpandPolytope(unsigned char pass, PointInfo nextPoint, FaceInfo* face, unsigned int Index, Horizon& horizon) {
	static const unsigned int plus1[] = { 1, 2, 0 };
	static const unsigned int plus2[] = { 2, 0, 1 };//面作成用インデックス
	
	//m_bestFaceに隣接する3つの面に対して新しい点から見える面を消す
    //その境界を新しい点とつなげ再構築
	if (face->pass != pass) {
		
		const unsigned int Index1 = plus1[Index];

		if (face->normal.Dot(nextPoint.point) - face->distanceToOrigin < 0) {
			FaceInfo* newFace = AddFace(face->pointB, face->pointA, nextPoint);


			BindFaces(newFace, 0, face, Index);

			if (horizon.currentFace != nullptr) {
				BindFaces(horizon.currentFace, 1, newFace, 2);
			}
			else {
				horizon.firstFace = newFace;
			}

			horizon.currentFace = newFace;
			++horizon.numberOfFaces;

			return true;
		}
		else {
			
			const unsigned int Index2 = plus2[Index];
			face->pass = pass;
			if (ExpandPolytope(pass, nextPoint, face->neighbors[Index1], face->neighborEdges[Index1], horizon) &&
				ExpandPolytope(pass, nextPoint, face->neighbors[Index2], face->neighborEdges[Index2], horizon)) {
				
				
				return true;
			}
		}
	}
	return false;
}

/// <summary>
/// 初期ポリトープから面情報を作ります。
/// </summary>
void EPASolver::InitPolytope() {

	vector<size_t> faces = {
	0, 1, 2,   // 面0
	1, 0, 3,   // 面1
	2, 1, 3,   // 面2
	0, 2, 3    // 面3
	};

	for (int i = 0; i < faces.size(); i += 3) {
		//面を作成してリストに保存
		FaceInfo* newface = new FaceInfo();

		newface->pointA = m_polytope[faces[i]];
		newface->pointB = m_polytope[faces[i + 1]];
		newface->pointC = m_polytope[faces[i + 2]];
		
		SimpleMath::Vector3 ab = newface->pointB.point - newface->pointA.point;
		SimpleMath::Vector3 ac = newface->pointC.point - newface->pointA.point;
		newface->normal = ab.Cross(ac);

		if (newface->normal.Dot(newface->pointA.point) < 0) {
			newface->normal = -newface->normal;
		}

		newface->normal.Normalize();
		newface->distanceToOrigin = newface->normal.Dot(newface->pointA.point);

		m_FacesInfo.push_back(newface);
	}
}

/// <summary>
/// faceAとfaceBがedgeAとedgeBでつながっていることを記録します。
/// </summary>
void EPASolver::BindFaces(FaceInfo* faceA, unsigned char edgeA, FaceInfo* faceB, unsigned char edgeB) {
	faceA->neighbors[edgeA] = faceB;
	faceA->neighborEdges[edgeA] = edgeB;

	faceB->neighbors[edgeB] = faceA;
	faceB->neighborEdges[edgeB] = edgeA;
}

/// <summary>
/// 新しい面を作成し初期化を行います。
/// </summary>
EPASolver::FaceInfo* EPASolver::AddFace(PointInfo& pointA, PointInfo& pointB, PointInfo& pointC) {

	FaceInfo* newFace = new FaceInfo();
	InitNewFace(newFace, pointA, pointB, pointC);
	
	newFace->prev = m_Last;
	newFace->next = nullptr;

	if (m_Last) {

		m_Last->next = newFace;
	}
	else {

		m_Root = newFace;
	}

	m_Last = newFace;

	return newFace;
}

/// <summary>
/// 頂点情報から受け取ったFaceInfoの初期化を行い、リストに保存します。
/// </summary>
void EPASolver::InitNewFace(FaceInfo* newFace, PointInfo& pointA, PointInfo& pointB, PointInfo& pointC) {
	newFace->pointA = pointA;
	newFace->pointB = pointB;
	newFace->pointC = pointC;

	SimpleMath::Vector3 ab = newFace->pointB.point - newFace->pointA.point;	ab.Normalize();
	SimpleMath::Vector3 ac = newFace->pointC.point - newFace->pointA.point;	ac.Normalize();
	newFace->normal = ab.Cross(ac);
	newFace->normal.Normalize();

	if (newFace->normal.Dot(newFace->pointA.point) < 0) {
		newFace->normal = -newFace->normal;
	}

	newFace->distanceToOrigin = newFace->normal.Dot(newFace->pointA.point);


	m_FacesInfo.push_back(newFace);

}

/// <summary>
/// 指定した面をリストから削除します。
/// </summary>
/// <param name="target"></param>
void EPASolver::RemoveFace(FaceInfo* target) {
	auto it = std::remove(m_FacesInfo.begin(), m_FacesInfo.end(), target);
	if (it != m_FacesInfo.end()){
		m_FacesInfo.erase(it, m_FacesInfo.end());
	}
}

/// <summary>
/// リストから最も原点に近い面を算出します。
/// </summary>
/// <returns>最も原点に近い面のポインタ</returns>
EPASolver::FaceInfo* EPASolver::FindBest() {

	FaceInfo* bestf = nullptr;
	float bestd = FLT_MAX;
	for (auto& face : m_FacesInfo) {
		
		if (face->distanceToOrigin < bestd && face->normal.Dot(face->pointA.point) > 0) {
			bestd = face->distanceToOrigin;
			bestf = face;
		}

	}
	return bestf;
}

/// <summary>
/// 面情報から接触点を求めます。
/// </summary>
/// <returns>接触点の情報</returns>
PointInfo EPASolver::GetContactPoint(FaceInfo face) {

	PointInfo contactPoint;

	const SimpleMath::Vector3& A = face.pointA.point;
	const SimpleMath::Vector3& B = face.pointB.point;
	const SimpleMath::Vector3& C = face.pointC.point;

	SimpleMath::Vector3 AB = B - A;
	SimpleMath::Vector3 AC = C - A;
	SimpleMath::Vector3 AP = -A; // 原点 - A

	float d00 = AB.Dot(AB);
	float d01 = AB.Dot(AC);
	float d11 = AC.Dot(AC);
	float d20 = AP.Dot(AB);
	float d21 = AP.Dot(AC);
	float denom = d00 * d11 - d01 * d01;

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// それぞれの形状の補間点
	contactPoint.supA = u * face.pointA.supA + v * face.pointB.supA + w * face.pointC.supA;
	contactPoint.supB = u * face.pointA.supB + v * face.pointB.supB + w * face.pointC.supB;

	// 接触点
	contactPoint.point = (contactPoint.supA + contactPoint.supB) * 0.5f;

	return contactPoint;

}

