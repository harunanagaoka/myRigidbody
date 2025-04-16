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

	int IndexOfFurthestPoint(const array<SimpleMath::Vector3, 8>& vertices,
							 const SimpleMath::Vector3& direction)
	{
		float maxDot = -FLT_MAX;
		int index;

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
		int i = IndexOfFurthestPoint(vertices_1, direction);

		int j = IndexOfFurthestPoint(vertices_2, -direction);

		return vertices_1[i] - vertices_2[j];
	}

	SimpleMath::Vector3 TripleProduct(const SimpleMath::Vector3& a, const SimpleMath::Vector3& b, const SimpleMath::Vector3& c)
	{
		return b * a.Dot(c) - c * a.Dot(b);
	}

	bool GJK(const BoxCollider& collider_1,
			 const BoxCollider& collider_2)
	{
		const array<SimpleMath::Vector3, 8> vertices_1 = collider_1.GetWorldVertices();
		const array<SimpleMath::Vector3, 8> vertices_2 = collider_2.GetWorldVertices();

		array<SimpleMath::Vector3, g_maxSimplexSize> simplex;
		SimpleMath::Vector3 ao, ab,ac, a, b, c,abperp, acperp;

		//コライダーの中心点を求める
		SimpleMath::Vector3 center_1 = collider_1.ComputeCenter();
		SimpleMath::Vector3 center_2 = collider_2.ComputeCenter();

		//center_2→center_1の方向を求める
		SimpleMath::Vector3 d = center_1 - center_2;
		
		if ((d.x == 0) && (d.y == 0) && (d.z == 0))
		{ 
			d = SimpleMath::Vector3::UnitX;//中心点の値が同じならひとまずX軸方向へ
		}

		//1と2のミンコフスキ差図形でd方向の内積が最大の点を求める
		a = simplex[0] = Support(vertices_1, vertices_2, d);

		if (a.Dot(d) <= 0)
		{
			return false;
		}

		int index = 1;//現在のsimplexの頂点数
		d = -a;//原点に向かうベクトル

		
		int count = 0;//無限ループ防止
		while (true)
		{
			//インクリメント危ない。超えないよう防止する方法を探す
			a = simplex[++index] = Support(vertices_1, vertices_2, d);

			if (a.Dot(d) <= 0)
			{
				return false;
			}

			ao = -a;//原点に向かうベクトル

			if (index < 2)
			{//例外処理。aoがちょうど同じ方向、または逆方向を向いたとき。
				b = simplex[0];
				ab = b - a;
				d = TripleProduct(ab,ao,ab);//のちほど作る// ABに垂直で、原点方向を向くベクトルを求める

			/*	if (lengthSquared(d) == 0)//ｄがゼロベクトルだったら
					d = perpendicular(ab);//abを基準にdを再設定する*/
				continue;
			}

			b = simplex[1];
			c = simplex[0];
			ab = b - a;
			ac = c - a;

			acperp = TripleProduct(ab,ac,ac);//ab,ac,ac

			if (acperp.Dot(ao) >= 0) {
				d = acperp;
			}
			else {
				abperp = TripleProduct(ac,ab,ab);//ac,ab,ab

				if (abperp.Dot(ao) < 0)
				{
					return true;
				}

				simplex[0] = simplex[1];// swap first element (point C)

				d = abperp; // new direction is normal to AB towards Origin

			}

			simplex[1] = simplex[2]; // swap element in the middle (point B)
			--index;

			count++;
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