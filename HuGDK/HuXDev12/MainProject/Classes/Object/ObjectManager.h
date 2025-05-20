//
// ObjectManager.h
//

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\Camera.h"
#include "Object/Object.h"
#include "ObjectRegistry.h"
#include "Object/ObjectFactory.h"
#include "..\ResourceManager\SpriteManager.h"
#include <unordered_map>
#include <unordered_set>


using namespace std;

class ObjectManager {
public:
	ObjectManager(Camera& camera) :m_camera(camera){}

	void Initialize();
	void InitAll();
	void UpdateAll();
	void CollisionAll();
	void CreateEnemyPosList();
	int CreateObject(ObjectType type);
	void SpawnBullet();
	void RemoveObject(int id);
	void DeleteDeadObject();
	void RenderAll();

private:

	Object* m_player;
	unordered_map<int, Object*> m_objectMap;
	unordered_set<int> m_enemiesID;
	unordered_set<int> m_bulletsID;

	



	//m_objectMap Ç©ÇÁ erase() ÇµÇΩÇÁÅAm_enemyMap Ç…Ç‡éËìÆÇ≈è¡Ç≥Ç»Ç´Ç·Ç¢ÇØÇ»Ç¢
	Camera& m_camera;

	vector<SimpleMath::Vector2> m_enemyPosList;
};