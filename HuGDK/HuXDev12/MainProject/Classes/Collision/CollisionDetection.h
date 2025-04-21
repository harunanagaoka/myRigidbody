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

	size_t IndexOfFurthestPoint(const array<SimpleMath::Vector3, 8>& vertices,
							 const SimpleMath::Vector3& direction)
	{
		float maxDot = -FLT_MAX;
		size_t index = 0;

		for (int i = 0;i < vertices.size();i++)
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

	//// Minkowski sum support function for GJK
	SimpleMath::Vector3 Support(const array<SimpleMath::Vector3, 8> vertices_1,
								const array<SimpleMath::Vector3, 8> vertices_2,
								const SimpleMath::Vector3& direction)
	{
		size_t i = IndexOfFurthestPoint(vertices_1, direction);

		size_t j = IndexOfFurthestPoint(vertices_2, -direction);

		return vertices_1[i] - vertices_2[j];
	}

	bool HandleTetrahedron(array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& d)
	{
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

	bool HandleTriangle(std::array<SimpleMath::Vector3, g_maxSimplexSize>& simplex, size_t& index, SimpleMath::Vector3& direction)
	{
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
			simplex[0] = simplex[1];//Cを捨てる
			simplex[1] = simplex[2];
			index = 2;
			direction = acperp;
			return false;
		}

		SimpleMath::Vector3 abperp = ab.Cross(ac).Cross(ab);

		if (abperp.Dot(ao) > 0)//原点がABの外にいるかどうか
		{
			simplex[0] = simplex[1];//Cを捨てる
			simplex[1] = simplex[2];
			index = 2;
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

	bool GJK(const BoxCollider& collider_1,
			 const BoxCollider& collider_2)
	{
		const array<SimpleMath::Vector3, 8> vertices_1 = collider_1.GetWorldVertices();
		const array<SimpleMath::Vector3, 8> vertices_2 = collider_2.GetWorldVertices();

		array<SimpleMath::Vector3, g_maxSimplexSize> simplex;
		SimpleMath::Vector3 ao, ab,ac, a, b, c,abperp, acperp;

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


private :
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

private:

};
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