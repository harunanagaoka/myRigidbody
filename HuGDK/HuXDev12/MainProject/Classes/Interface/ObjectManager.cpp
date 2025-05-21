
#include "ObjectManager.h"

void ObjectManager::Initialize() {
	for (auto& [id1, obj1] : m_objectMap) {
		if (obj1->isTransformDirty()) {

			Transform* transform = obj1->GetTransform();
			const DirectX::SimpleMath::Matrix& world = transform->GetWorldMatrix();
			obj1->GetCollider()->UpdateWorldVertices(world);
		}
	}
}

void ObjectManager::Update() {

	//1,衝突判定 2，リジッドボディ計算
	if (m_objectMap.size() >= 2)
	{

		for (auto& [id1, obj1] : m_objectMap) {
			for (auto& [id2, obj2] : m_objectMap) {
				if (id1 >= id2) continue;
				CollisionDetection::ContactInfo info;
				info = m_detection.Collision(obj1->GetCollider(), obj2->GetCollider());
				if (info.hasValue)
				{
					obj1->GetRigidbody()->ResolveCollision(info, true);
					obj2->GetRigidbody()->ResolveCollision(info, false);
				}
				
			}
		}
	}

	if (m_objectMap.size() >= 1) {
		//2，リジッドボディ計算
		for (auto& [id1, obj1] : m_objectMap) {
			obj1->GetRigidbody()->Update();
		}
	}

	//3，マトリックス更新
	for (auto& [id1, obj1] : m_objectMap) {
		if (obj1->isTransformDirty()) {
			const DirectX::SimpleMath::Matrix& world = m_objectMap[id1]->GetTransform()->GetWorldMatrix();
			obj1->GetCollider()->UpdateWorldVertices(world);
		}
	}
}

int ObjectManager::CreateObject() {//bool isTransform, bool isRigidbody, bool isCollider
	static int id = 0;
	Object* obj = new Object(id++);
	m_objectMap[id] = obj;

	//if (isTransform)
	//{
		obj->AddTransform();
	//}

	//if(isRigidbody)
	//{
		obj->AddRigidbody();
	//}

	//if (isCollider)
	//{
		obj->AddBoxCollider();
	//}

	return id;
}

void ObjectManager::RemoveObject(int id) {
	auto it = m_objectMap.find(id);

	if (it != m_objectMap.end()) {
		delete it->second;
		m_objectMap.erase(it);
	}
}

void ObjectManager::Clear() {
	m_objectMap.clear();
}
void ObjectManager::ContactCheck() {
	m_objectMap[0]->GetTransform()->isDirty();
}

void ObjectManager::Draw(DirectXTK::BasicEffect& be, DirectXTK::GeometricPrimitive& shape,int a) {

	if (m_objectMap.size() == 0)
	{
		return;
	}

	if (m_objectMap[a]->GetTransform()) {
		be->SetWorld(m_objectMap[a]->GetTransform()->GetWorldMatrix());
		be->Apply(DXTK->CommandList);
		shape->Draw(DXTK->CommandList);
	}

}

/*	for (auto& [id1, obj1] : m_objectMap) {
		if (obj1->GetTransform()) {
			be->SetWorld(obj1->GetTransform()->GetWorldMatrix());
			be->Apply(DXTK->CommandList);
			shape->Draw(DXTK->CommandList);
		}
	}*/

void ObjectManager::SetStatic(int id,bool isStatic) {

	m_objectMap[id]->GetRigidbody()->SetStatic(isStatic);
}