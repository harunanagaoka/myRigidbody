//
// EPAclass.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include <array>

using namespace std;

bool EPA(std::array<SimpleMath::Vector3, 4>& simplex)
{
	//原点→シンプレックス最接近点ｄを求める
	//サポート関数でｄ方向で最も遠い点vを探す
	//

	return false;
}
//まずは最も簡単な例から。
//①原点がシンプレックスの中にある場合
//②シンプレックスが完成してない場合
//③原点がシンプレックスの表面上にちょうど重なる場合
//の順番で作っていくやで
//ゴール
//ミンコフスキー差図形の中の原点とそれと一番近い表面の点を見つけて
//貫通深度と押し戻し方向を求めるやで