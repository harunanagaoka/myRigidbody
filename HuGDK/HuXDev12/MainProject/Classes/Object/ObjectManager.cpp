//
// ObjectManager.cpp
//

#include "ObjectManager.h"

void ObjectManager::Initialize() {
}

void ObjectManager::UpdateAll() {
	m_player->Update();

		for (auto& pair : m_objectMap) {

			if (pair.second != nullptr) {
				pair.second->Update();
			}
		}

	SpawnBullet();

	DeleteDeadObject();
}

void ObjectManager::CreateEnemyPosList() {
	m_enemyPosList.clear();

	for (auto& id : m_enemiesID) {
		
		auto it = m_objectMap.find(id);

		if (it != m_objectMap.end() && it->second != nullptr) {
			m_enemyPosList.push_back(it->second->GetTransform()->GetPosition());
		}
	}
}

void ObjectManager::SpawnBullet() {
	if (m_player->IsAttack())
	{
		int id = CreateObject(ObjectType::PlayerBullet);
		CreateEnemyPosList();
		m_objectMap[id]->SetDirection(m_enemyPosList);
		m_player->ClearAttackFlag();
	}
}

void ObjectManager::InitAll() {

}

void ObjectManager::CollisionAll() {
	//collider専用のクラス作るべきかも
}

int ObjectManager::CreateObject(ObjectType type) {

	static int id = 0;

	Object* obj = nullptr;

	switch (type)
	{
	case ObjectType::Player:

		obj = ObjectFactory::CreateObject(type, id++, m_camera);
		m_player = obj;
		m_objectMap[id] = obj;
		obj->Initialize();
		break;

	case ObjectType::Enemy:
		obj = ObjectFactory::CreateObject(type, id++, m_camera);

		m_enemiesID.insert(id);
		m_objectMap[id] = obj;
		obj->Initialize();
		break;
	case ObjectType::PlayerBullet:
	case ObjectType::EnemyBullet:

		obj = ObjectFactory::CreateBullet(id++, m_camera, m_player->GetTransform()->GetPosition());
		m_bulletsID.insert(id);
		m_objectMap[id] = obj;
		obj->Initialize();
		break;

	default:

		return -1;
	}

	return id;
}

void ObjectManager::RemoveObject(int id) {

}

void ObjectManager::DeleteDeadObject() {
	//CleanObjectMap

	for (auto it = m_objectMap.begin(); it != m_objectMap.end(); ) {
		if (it->second->IsDead()) {
			int id = it->second->GetID();
			ObjectType type = it->second->GetObjectType();

			switch (type)
			{
			case ObjectType::Player:
				break;
			case ObjectType::Enemy:

				m_enemiesID.erase(id);

				break;
			case ObjectType::PlayerBullet:
			case ObjectType::EnemyBullet:

				m_bulletsID.erase(id);

				break;
			default:
				break;
			}

			delete it->second;
			it = m_objectMap.erase(it); // eraseは次のイテレータを返す
		}
		else {
			++it;
		}
	}
}

void ObjectManager::RenderAll(){

	SpriteManager& sp = SpriteManager::GetInstance();

	for (auto& pair : m_objectMap) {

		if (pair.second != nullptr) {
			pair.second->Render(sp);
		}
	}
}
