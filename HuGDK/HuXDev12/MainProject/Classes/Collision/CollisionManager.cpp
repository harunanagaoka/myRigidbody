
#include "CollisionManager.h"

void CollisionManager::Update()
{
	ContactCheck();
	for (size_t i = 0; i < m_colliders.size(); ++i) {
		//m_colliders[i]->UpdateWorldVertices(m_colliders[i]->GetRigidBody()->m_transform.GetWorldMatrix());
	}

}

void CollisionManager::AddCollider(BoxCollider* collider) {

	m_colliders.push_back(collider);
}

void CollisionManager::RemoveCollider(BoxCollider* collider) {

	auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);

	if (it != m_colliders.end()) {

		m_colliders.erase(it); 

	}
}

void CollisionManager::Clear() {

	m_colliders.clear();
}

void CollisionManager::ContactCheck() {

	//for (size_t i = 0; i < m_rigidbodyMap.size(); ++i) {
	//	//for (size_t j = i + 1; j < m_colliders.size(); ++j) {
	//	//	CollisionDetection::ContactInfo info;
	//	//	info = m_Detection.Collision(m_colliders[i], m_colliders[j]);
	//	//	 Rigidbody* rb = m_colliders[i]->GetRigidBody();
	//	//	 rb->ResolveCollision(info, true);
	//	//	 Rigidbody* rbj =  m_colliders[j]->GetRigidBody();
	//	//	 rbj->ResolveCollision(info, false);

	//	//	//ÚG”»’è
	//	//}
	//}

}

void CollisionManager::DrawOne(DirectXTK::BasicEffect& be, DirectXTK::GeometricPrimitive& shape) {
	//be->SetWorld(m_colliders[0]->GetRigidBody()->Draw());
	//be->Apply(DXTK->CommandList);
	//shape->Draw(DXTK->CommandList);
}

void CollisionManager::DrawTwo(DirectXTK::BasicEffect& be, DirectXTK::GeometricPrimitive& shape) {

	//be->SetWorld(m_colliders[1]->GetRigidBody()->Draw());
	//be->Apply(DXTK->CommandList);
	//shape->Draw(DXTK->CommandList);
}