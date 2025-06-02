//
//#include "EPASolver.h"
//
//ContactInfo EPASolver::EPA(const PhysicsCollider* collider_A, const PhysicsCollider* collider_B, array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, ContactInfo& info)
//{
//	const vector<SimpleMath::Vector3> vertices_1 = collider_A->GetWorldVertices();
//	const vector<SimpleMath::Vector3> vertices_2 = collider_B->GetWorldVertices();
//
//	//GJKから受け取ったシンプレックスをvectorに移す（ポリトープ）
//	vector<SimpleMath::Vector3> polytope(simplex.begin(), simplex.end());
//	vector<size_t> faces = { 0, 1, 2,
//							0, 3, 1,
//							0, 2, 3,
//							1, 3, 2 };//四面体
//	vector<FaceInfo> facesInfo;
//
//	//すべての三角面の法線（+距離）を計算し、原点に最も近い面を探して返す
//	size_t minFace = GetFaceNormals(polytope, faces, facesInfo);//原点に一番近いと思われる面の法線と距離
//	//★戻り値をminFaceのみにする
//
//	SimpleMath::Vector3 minNormal;
//	float minDistance = FLT_MAX;
//	SimpleMath::Vector3 support;
//
//	minNormal = SimpleMath::Vector3(facesInfo[minFace].normal);//法線ベクトル
//	minDistance = facesInfo[minFace].distanceToOrigin;//距離
//
//	int count = 0;
//	while (count++ < 50) {//メインループ
//
//		support = Support(vertices_1, vertices_2, facesInfo[minFace].normal);
//
//		//★avoidfaceポリトープ破壊
//		std::vector<size_t> facesToRemove;
//		std::vector<std::pair<size_t, size_t>> uniqueEdges;
//
//		for (size_t i = 0; i < facesInfo.size(); i++) {
//			if (facesInfo[i].normal.Dot(support) > 0) {
//				size_t f = i * 3;
//
//				AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
//				AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
//				AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);
//
//				facesToRemove.push_back(i);//消す予定の番号
//			}
//		}
//		std::sort(facesToRemove.rbegin(), facesToRemove.rend());//重複除去　後ろから消す
//		for (size_t index : facesToRemove) {
//			// FaceInfo削除
//			facesInfo.erase(facesInfo.begin() + index);
//
//			// 対応するfacesインデックス3つを削除（index*3 から3連）
//			size_t base = index * 3;
//			faces.erase(faces.begin() + base, faces.begin() + base + 3);
//		}
//		//★avoidface
//
//		polytope.push_back(support);//サポート点追加
//
//		//★addnewface
//		std::vector<FaceInfo> newFacesInfo;
//		for (auto [edgeIndex1, edgeIndex2] : uniqueEdges) {
//			FaceInfo newFaceInfo;
//			size_t supportIndex = polytope.size() - 1;
//
//			newFaceInfo.indexA = polytope[edgeIndex1];
//			newFaceInfo.indexB = polytope[edgeIndex2];
//			newFaceInfo.indexC = polytope[supportIndex];
//			newFaceInfo.Anum = edgeIndex1;
//			newFaceInfo.Bnum = edgeIndex2;
//			newFaceInfo.Cnum = supportIndex;
//			newFacesInfo.push_back(newFaceInfo);
//			faces.push_back(edgeIndex1);
//			faces.push_back(edgeIndex2);
//			faces.push_back(supportIndex);
//		}
//
//		float oldMinDistance = facesInfo[minFace].distanceToOrigin;
//		minFace = GetFaceNormals(polytope, faces, newFacesInfo);
//		//原点→シンプレックス最接近点ｄを求める
//		//サポート関数でｄ方向で最も遠い点vを探す
//		//★addnewface
//
//		for (size_t i = 0; i < newFacesInfo.size(); i++) {
//			if (newFacesInfo[i].distanceToOrigin < oldMinDistance) {
//				minDistance = newFacesInfo[i].distanceToOrigin;
//				minFace = facesInfo.size() + i;
//			}
//		}
//		facesInfo.insert(facesInfo.end(), newFacesInfo.begin(), newFacesInfo.end());
//		//★addnewface
//		//★checkdistance
//		float sDistance = facesInfo[minFace].normal.Dot(support);
//
//		if (abs(sDistance - minDistance) <= 1e-6f) {//終了チェック
//			//minDistance = FLT_MAX;
//			break;
//		}
//		//★checkdistance
//
//		minDistance = FLT_MAX;//ループ継続
//	}
//
//	//デストラクタ
//	SimpleMath::Vector3 tangent1, tangent2, contactPointA, contactPointB;
//	//minNormal→跳ね返る方向
//	//tangent1→摩擦その①（たとえば横方向スリップ）横方向の滑りを止める
//	//tangent2→摩擦その②（たとえば前後スリップ）前後方向の滑りを止める
//
//	//★タンジェントの計算
//	ComputeTangentBasis(minNormal, tangent1, tangent2);
//
//
//	//接触点の計算
//	//★FaceInfo[minFace]を渡す
//	ComputeBarycentric(facesInfo[minFace], contactPointA, contactPointB);
//
//	info.contactPointA = contactPointA;
//	info.contactPointB = contactPointB;
//	info.tangent1 = tangent1;
//	info.tangent2 = tangent2;
//	info.normal = minNormal;
//	info.penetrationDepth = facesInfo[minFace].distanceToOrigin;
//	//info.penetrationDepth = max(0.0f, facesInfo[minFace].distanceToOrigin - bias);
//	//めりこむ場合は上記を入れてみる
//
//	//今はfalseを返してるけど衝突情報の構造体を返す予定
//	return info;
//}
//
//void EPASolver::ComputeBarycentric(FaceInfo minFace, SimpleMath::Vector3& contactPointA, SimpleMath::Vector3& contactPointB)
//{
//	//求め方
//	// 三角形 A, B, C と、内部の点 P に対して：
//
//	PointInfo& pA = m_polytope[minFace.Anum];
//	PointInfo& pB = m_polytope[minFace.Bnum];
//	PointInfo& pC = m_polytope[minFace.Cnum];
//
//	SimpleMath::Vector3 A, B, C, P;
//	A = minFace.indexA;
//	B = minFace.indexB;
//	C = minFace.indexC;
//	P = SimpleMath::Vector3::Zero;//原点
//
//	SimpleMath::Vector3 v0 = B - A;
//	SimpleMath::Vector3 v1 = C - A;
//	SimpleMath::Vector3 v2 = P - A;
//
//	float d00 = v0.Dot(v0);
//	float d01 = v0.Dot(v1);
//	float d11 = v1.Dot(v1);
//	float d20 = v2.Dot(v0);
//	float d21 = v2.Dot(v1);
//
//	float denom = d00 * d11 - d01 * d01;
//	float v = (d11 * d20 - d01 * d21) / denom;
//	float w = (d00 * d21 - d01 * d20) / denom;
//	float u = 1.0f - v - w;
//	//★座標も入った三角形の構造体受け取り計算
//	contactPointA = u * pA.supA + v * pB.supA + w * pC.supA;
//	contactPointB = u * pA.supB + v * pB.supB + w * pC.supB;
//}
//
//void EPASolver::ComputeTangentBasis(const SimpleMath::Vector3& normal, SimpleMath::Vector3& tangent1, SimpleMath::Vector3& tangent2)
//{
//	if (normal.x >= 0.57735f)//√(1 / 3) の近似値
//	{
//		tangent1 = SimpleMath::Vector3(normal.y, -normal.x, 0.0f);
//	}
//	else
//	{
//		tangent1 = SimpleMath::Vector3(0.0f, normal.z, -normal.y);
//	}
//
//	tangent1.Normalize();
//	tangent2 = normal.Cross(tangent1);
//}
//
//void EPASolver::AddIfUniqueEdge(
//	std::vector<std::pair<size_t, size_t>>& edges,
//	const std::vector<size_t>& faces,
//	size_t a,
//	size_t b)
//{
//	auto reverse = std::find(                       //      0--<--3
//		edges.begin(),                              //     / \ B /   A: 2-0
//		edges.end(),                                //    / A \ /    B: 0-2
//		std::make_pair(faces[b], faces[a]) //   1-->--2
//	);
//
//	if (reverse != edges.end()) {
//		edges.erase(reverse);
//	}
//
//	else {
//		edges.emplace_back(faces[a], faces[b]);
//	}
//}
//
////bool SameDirection(SimpleMath::Vector4 normal, SimpleMath::Vector3 support) {
////	SimpleMath::Vector3 vec3normal = SimpleMath::Vector3(normal.x, normal.y, normal.z);
//
////	return vec3normal.Dot(support) > 0;
////}
//
//size_t EPASolver::GetFaceNormals(const std::vector<SimpleMath::Vector3>& polytope,
//	const std::vector<size_t>& faces, vector<FaceInfo>& facesInfo)
//{
//	std::vector<SimpleMath::Vector4> normals;
//	size_t minTriangle = 0;
//	float  minDistance = FLT_MAX;
//
//	for (size_t i = 0; i < faces.size(); i += 3) {
//
//		FaceInfo faceInfo;
//
//
//		SimpleMath::Vector3 a = faceInfo.indexA = polytope[faces[i]];
//		faceInfo.Anum = faces[i];
//
//		SimpleMath::Vector3 b = faceInfo.indexB = polytope[faces[i + 1]];
//		faceInfo.Bnum = faces[i + 1];
//
//		SimpleMath::Vector3 c = faceInfo.indexC = polytope[faces[i + 2]];
//		faceInfo.Cnum = faces[i + 2];
//		//ここのabcのインデックスを保持する（vectorで保存するか迷っとる）
//
//		SimpleMath::Vector3 ab = b - a;
//		SimpleMath::Vector3 ac = c - a;
//		SimpleMath::Vector3 normal = ab.Cross(ac);
//
//		// 長さゼロ＝面積ゼロの三角形
//		if (normal.LengthSquared() < 1e-6f) {
//			continue; // ← このfaceは無効としてスキップ
//		}
//
//		normal.Normalize();
//
//		faceInfo.normal = normal;
//
//		float distance = normal.Dot(a);//原点（0,0,0）から法線方向に見た距離
//
//		if (distance < 0) {
//			normal *= -1;
//			distance *= -1;
//		}
//		faceInfo.distanceToOrigin = distance;
//
//		normals.emplace_back(SimpleMath::Vector4(normal.x, normal.y, normal.z, distance));
//
//		if (distance < minDistance) {
//			minTriangle = i / 3;
//			minDistance = distance;
//		}
//
//		faceInfo.normal = normal;
//		facesInfo.push_back(faceInfo);
//	}
//
//	return minTriangle;
//}
//
