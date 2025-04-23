//
// CollisionDetection.h
// 衝突検知

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "BoxCollider.h"
#include <array>


using namespace std;

constexpr size_t g_maxSimplexSize = 4;

class CollisionDetection {
public:
	struct PointInfo {
		SimpleMath::Vector3 point;
		SimpleMath::Vector3 supA, supB;
	};

	struct FaceInfo {
		SimpleMath::Vector3 indexA, indexB, indexC;//頂点
		size_t Anum, Bnum, Cnum;//polytopeの数に対応している
		SimpleMath::Vector3 normal;//法線
		float distanceToOrigin;
	};

	struct ContactInfo {
		SimpleMath::Vector3 contactPointA = SimpleMath::Vector3::Zero; 
		SimpleMath::Vector3 contactPointB = SimpleMath::Vector3::Zero;
		SimpleMath::Vector3 normal = SimpleMath::Vector3::Zero;        // 法線（A→B）
		float penetrationDepth = 0;           // めり込み
		SimpleMath::Vector3 tangent1 = SimpleMath::Vector3::Zero;     // 摩擦方向１
		SimpleMath::Vector3 tangent2 = SimpleMath::Vector3::Zero;     // 摩擦方向２
	};

	size_t IndexOfFurthestPoint(const array<SimpleMath::Vector3, 8>& vertices,
		const SimpleMath::Vector3& direction);

	/*SimpleMath::Vector3 Support(const array<SimpleMath::Vector3, 8> vertices_1,const array<SimpleMath::Vector3, 8> vertices_2,
								const SimpleMath::Vector3& direction);*/

	bool HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d);

	bool HandleTriangle(std::array<SimpleMath::Vector3, 4>& simplex, size_t& index, SimpleMath::Vector3& direction);
	
	bool GJK(const BoxCollider& collider_1,const BoxCollider& collider_2);

	SimpleMath::Vector3 Support(const array<SimpleMath::Vector3, 8>& vertices_1, const array<SimpleMath::Vector3, 8>& vertices_2,  SimpleMath::Vector3& direction) {
		PointInfo pointInfo;

		size_t i = IndexOfFurthestPoint(vertices_1, direction);
		pointInfo.supA = vertices_1[i];

		size_t j = IndexOfFurthestPoint(vertices_2, -direction);
		pointInfo.supB = vertices_2[j];

		pointInfo.point = pointInfo.supA - pointInfo.supB;
		m_polytope.push_back(pointInfo);

		return pointInfo.point;
	}

	bool EPA(const BoxCollider& collider_1, const BoxCollider& collider_2,std::array<SimpleMath::Vector3, g_maxSimplexSize>& simplex)
	{
		const array<SimpleMath::Vector3, 8> vertices_1 = collider_1.GetWorldVertices();
		const array<SimpleMath::Vector3, 8> vertices_2 = collider_2.GetWorldVertices();

		//GJKから受け取ったシンプレックスをvectorに移す（ポリトープ）
		vector<SimpleMath::Vector3> polytope(simplex.begin(), simplex.end());
		vector<size_t> faces = {0, 1, 2,
								0, 3, 1,
								0, 2, 3,
								1, 3, 2};//四面体
		vector<FaceInfo> facesInfo;

		//すべての三角面の法線（+距離）を計算し、原点に最も近い面を探して返す
		size_t minFace = GetFaceNormals(polytope, faces, facesInfo);//原点に一番近いと思われる面の法線と距離
		//★戻り値をminFaceのみにする

		SimpleMath::Vector3 minNormal;
		float minDistance = FLT_MAX;
		SimpleMath::Vector3 support;

		minNormal = SimpleMath::Vector3(facesInfo[minFace].normal);//法線ベクトル
		minDistance = facesInfo[minFace].distanceToOrigin;//距離

		while (true) {//メインループ

			support = Support(vertices_1, vertices_2, facesInfo[minFace].normal);

			//★avoidfaceポリトープ破壊
			std::vector<size_t> facesToRemove;
			std::vector<std::pair<size_t, size_t>> uniqueEdges;

			for (size_t i = 0; i < facesInfo.size(); i++) {
				if (facesInfo[i].normal.Dot(support) > 0) {
					size_t f = i * 3;

					AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
					AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
					AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);

					facesToRemove.push_back(i);//消す予定の番号
				}
			}
			std::sort(facesToRemove.rbegin(), facesToRemove.rend());//重複除去　後ろから消す
			for (size_t index : facesToRemove) {
				// FaceInfo削除
				facesInfo.erase(facesInfo.begin() + index);

				// 対応するfacesインデックス3つを削除（index*3 から3連）
				size_t base = index * 3;
				faces.erase(faces.begin() + base, faces.begin() + base + 3);
			}
			//★avoidface

			polytope.push_back(support);//サポート点追加
			
			//★addnewface
			std::vector<FaceInfo> newFacesInfo;
			for (auto [edgeIndex1, edgeIndex2] : uniqueEdges) {
				FaceInfo newFaceInfo;
				size_t supportIndex = polytope.size() - 1;

				newFaceInfo.indexA = polytope[edgeIndex1];
				newFaceInfo.indexB = polytope[edgeIndex2];
				newFaceInfo.indexC = polytope[supportIndex];
				newFacesInfo.push_back(newFaceInfo);
				faces.push_back(edgeIndex1);
				faces.push_back(edgeIndex2);
				faces.push_back(supportIndex);
			}
			
			float oldMinDistance = facesInfo[minFace].distanceToOrigin; 
			minFace = GetFaceNormals(polytope, faces, newFacesInfo);
			//原点→シンプレックス最接近点ｄを求める
			//サポート関数でｄ方向で最も遠い点vを探す
			//★addnewface

			for (size_t i = 0; i < newFacesInfo.size(); i++) {
				if (newFacesInfo[i].distanceToOrigin < oldMinDistance) {
					minDistance = newFacesInfo[i].distanceToOrigin;
					minFace = facesInfo.size() + i;
				}
			}
			facesInfo.insert(facesInfo.end(), newFacesInfo.begin(), newFacesInfo.end());
			//★addnewface
			//★checkdistance
			float sDistance = facesInfo[minFace].normal.Dot(support);

			if (abs(sDistance - minDistance) <= 1e-6f) {//終了チェック
				//minDistance = FLT_MAX;
				break;
			}
			//★checkdistance

			minDistance = FLT_MAX;//ループ継続
		}

//デストラクタ
		SimpleMath::Vector3 tangent1, tangent2, contactPointA, contactPointB;
		//minNormal→跳ね返る方向
		//tangent1→摩擦その①（たとえば横方向スリップ）横方向の滑りを止める
		//tangent2→摩擦その②（たとえば前後スリップ）前後方向の滑りを止める

		//★タンジェントの計算
		ComputeTangentBasis(minNormal, tangent1, tangent2);


		//接触点の計算
		//★FaceInfo[minFace]を渡す
		ComputeBarycentric(facesInfo[minFace], contactPointA, contactPointB);

		ContactInfo info;
		info.contactPointA = contactPointA;
		info.contactPointB = contactPointB;
		info.tangent1 = tangent1;
		info.tangent2 = tangent2;
		info.normal = minNormal;
		info.penetrationDepth = facesInfo[minFace].distanceToOrigin;
		//info.penetrationDepth = max(0.0f, facesInfo[minFace].distanceToOrigin - bias);
		//めりこむ場合は上記を入れてみる

		//今はfalseを返してるけど衝突情報の構造体を返す予定
		return false;
	}

	void ComputeBarycentric(FaceInfo minFace,SimpleMath::Vector3& contactPointA, SimpleMath::Vector3& contactPointB)
	{
		//求め方
// 三角形 A, B, C と、内部の点 P に対して：

		PointInfo& pA = m_polytope[minFace.Anum];
		PointInfo& pB = m_polytope[minFace.Bnum];
		PointInfo& pC = m_polytope[minFace.Cnum];

		SimpleMath::Vector3 A, B, C, P;
		A = minFace.indexA;
		B = minFace.indexB;
		C = minFace.indexC;
		P = SimpleMath::Vector3::Zero;//原点

		SimpleMath::Vector3 v0 = B - A;
		SimpleMath::Vector3 v1 = C - A;
		SimpleMath::Vector3 v2 = P - A;

		float d00 = v0.Dot(v0);
		float d01 = v0.Dot(v1);
		float d11 = v1.Dot(v1);
		float d20 = v2.Dot(v0);
		float d21 = v2.Dot(v1);

		float denom = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0f - v - w;
		//★座標も入った三角形の構造体受け取り計算
		contactPointA = u * pA.supA + v * pB.supA + w * pC.supA;
		contactPointB = u * pA.supB + v * pB.supB + w * pC.supB;
	}

	void ComputeTangentBasis(const SimpleMath::Vector3& normal, SimpleMath::Vector3& tangent1, SimpleMath::Vector3& tangent2)
	{
		if (normal.x >= 0.57735f)//√(1 / 3) の近似値
		{
			tangent1 = SimpleMath::Vector3(normal.y, -normal.x, 0.0f);
		}
		else
		{
			tangent1 = SimpleMath::Vector3(0.0f, normal.z, -normal.y);
		}

		tangent1.Normalize();
		tangent2 = normal.Cross(tangent1);
	}

	void AddIfUniqueEdge(
		std::vector<std::pair<size_t, size_t>>& edges,
		const std::vector<size_t>& faces,
		size_t a,
		size_t b)
	{
		auto reverse = std::find(                       //      0--<--3
			edges.begin(),                              //     / \ B /   A: 2-0
			edges.end(),                                //    / A \ /    B: 0-2
			std::make_pair(faces[b], faces[a]) //   1-->--2
		);

		if (reverse != edges.end()) {
			edges.erase(reverse);
		}

		else {
			edges.emplace_back(faces[a], faces[b]);
		}
	}

	//bool SameDirection(SimpleMath::Vector4 normal, SimpleMath::Vector3 support) {
	//	SimpleMath::Vector3 vec3normal = SimpleMath::Vector3(normal.x, normal.y, normal.z);

	//	return vec3normal.Dot(support) > 0;
	//}

	 size_t GetFaceNormals(const std::vector<SimpleMath::Vector3>& polytope,
																	   const std::vector<size_t>& faces, vector<FaceInfo>& facesInfo)
	{
		std::vector<SimpleMath::Vector4> normals;
		size_t minTriangle = 0;
		float  minDistance = FLT_MAX;

		for (size_t i = 0; i < faces.size(); i += 3) {

			FaceInfo faceInfo;
			facesInfo.push_back(faceInfo);

			SimpleMath::Vector3 a = faceInfo.indexA = polytope[faces[i]];
			faceInfo.Anum = faces[i];

			SimpleMath::Vector3 b = faceInfo.indexB = polytope[faces[i + 1]];
			faceInfo.Bnum = faces[i + 1];

			SimpleMath::Vector3 c = faceInfo.indexC = polytope[faces[i + 2]];
			faceInfo.Cnum = faces[i + 2];
			//ここのabcのインデックスを保持する（vectorで保存するか迷っとる）

			SimpleMath::Vector3 normal = (b - a).Cross(c - a);
			normal.Normalize();
			faceInfo.normal = normal;

			float distance = normal.Dot(a);

			if (distance < 0) {
				normal *= -1;
				distance *= -1;
			}
			faceInfo.distanceToOrigin = distance;

			normals.emplace_back(SimpleMath::Vector4(normal.x,normal.y,normal.z,distance));

			if (distance < minDistance) {
				minTriangle = i / 3;
				minDistance = distance;
			}
		}

		return minTriangle;
	}


private:
	vector<PointInfo> m_polytope;
};


//サポート関数作成　図形Oと方向ｖを渡すと内積が最大（原点から遠い点）になる点を返す関数
//support(A, v) - support(B, -v)でAとBのミンコフスキ差図形とvのサポート点が求められる
//ミンコフスキー差の中心点はAと同じである。

//Aの原点からBの原点を結ぶベクトルを求める-＞V
//Vとミンコフスキ差図形のベクトルで一番原点に近い（同じ方向の）点を求める
//求めた点と原点を結んだベクトルを求める-＞W
//[2点を結んだ線]と原点の最接近点と原点のベクトルX求める
//ベクトルXで作った点も使い三角形作る
//三角形と原点の最接近点と原点でまた点を作り、四面体作る
//四面体が原点を含むか見る


//Johnson's Distanceアルゴリズム　めりこみ計測
//四面体が内包している原点から一番近い四面体の点を求め原点とのベクトルを求める
//上のベクトルの方向にある凸包上の一番遠い点Xを求める
//四面体を作っていた点に点Xを追加し、直方体にする
//直方体内で一番原点に近い点を求め、原点と結びベクトルに
//点を増やしていき、収束するまで続ける
//※新しく追加された頂点によって出来る面だけ再計算

//収束点は、オブジェクトA,Bの距離と等しい。
//原点と収束点を結んだベクトルが力の向きになる。



//BoxColliderどうしを、ミンコフスキ差を使い衝突判定を行う

/*
//-----------------------------------------------------------------------------
// 2つの点ベクトルの算術的な和を返す
// 1次元のベクトルは1つの成分（数直線上の1つの座標）しか持たない
// 一般的に、ベクトルは1つ以上の成分を持つ

vec sum (vec a, vec b) {
	return a + b; // [ a.x + b.x, a.y + b.y, a.z + b.z, ... ]
}

//-----------------------------------------------------------------------------
// ドット積（内積）は、両方のベクトルの対応する成分を掛け合わせたものを
// すべて合計した値

float dotProduct (vec a, vec b) {
	return a * b; // a.x * b.x + a.y * b.y + a.z * b.z + ...
}

//-----------------------------------------------------------------------------
// 指定された方向 d に沿って、最も遠い頂点を取得する
// これは、d とのドット積が最大となる点を探すのと同じ意味
// 1次元では、方向は常に 1 または -1（原点の右か左）

size_t indexOfFurthestPoint (const vec * vertices, size_t count, vec d) {
	size_t index = 0;
	float maxProduct = dotProduct (d, vertices[index]);
	for (size_t i = 1; i < count; i++) {
		float product = dotProduct (d, vertices[i]); // may be negative
		if (product > maxProduct) {
			maxProduct = product;
			index = i;
		}
	}
	return index;
}

//-----------------------------------------------------------------------------
// GJK におけるミンコフスキー和のサポート関数

vec support (const vec * vertices1, size_t count1, // first shape
			 const vec * vertices2, size_t count2, // second shape
			 vec d) {                              // direction

// まず、任意の方向に沿って最も遠い1つ目の形状の点を取得

size_t i = indexOfFurthestPoint (vertices1, count1, d);
//
// 次に、反対方向に沿って最も遠い2つ目の形状の点を取得
// ※このとき方向ベクトルは反転されていることに注意
//
size_t j = indexOfFurthestPoint (vertices2, count2, -d);

// 最後に、この2点のミンコフスキー和（a + (-b)）を返すことで、
// 両方の形状が“重なっているか”を判定することができる
 return sum (vertices1[i], -vertices2[j]);
}
*/
/*2点を求めた後に三角形を作る方法
* ※三角形の三つ目の点は「原点を向いた垂線ベクトル」から作ることができる。
* 「原点を向いた垂線ベクトル」
* やり方はこうです：
① 線分CBと、
② 線分CO（= C→原点）
のクロス積をとり、
③ その結果と再びCBのクロス積をとります。

(CB × CO) × CB

つまり、**2回連続のクロス積（交差積）**を使って、
原点方向の垂線ベクトルを求めるんです：これが「CB に垂直で、かつ原点を向く方向」になる

この処理にはもっと速い公式（展開形）もあります：
三重積の展開式（triple product expansion） と呼ばれていて、
少ない掛け算・引き算で効率的に計算できます：

A × (B × C) = B(A・C) − C(A・B)//クロス積より速いらしい

ただし、この展開式を使うときはベクトルの向きに注意する必要があります。
正しく使えば、**常に「原点を向いた垂線ベクトル」**が得られます。


「CB に垂直で、かつ原点を向く方向」これをもう一度サポート関数に渡し点を得る
次は、三角形の平面に垂直で、かつ原点の方向を向く方向の点を求める

/三角推（テトラ）を得た後
内部に原点が含まれてるか見る

・その面に垂直な法線ベクトル（＝原点の方向を向く垂線）

・原点から、その辺と向かい合う頂点へのベクトル

そして、この2つのベクトルの内積（ドット積）が正（0より大）であるかをチェックします…
※面の外側に向かう線と原点の向きが逆方向であるかどうか

*/