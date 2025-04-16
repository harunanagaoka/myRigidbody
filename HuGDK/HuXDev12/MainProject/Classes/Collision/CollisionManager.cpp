
#include "CollisionManager.h"

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