//
// Rigidbody.cpp
//

#include "Rigidbody.h"

void Rigidbody::Update() {


	m_solver.Update(m_transform);
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());

}

void Rigidbody::ImpactResponse() {
	//衝突infoをうけとりrigidbodyにtransformと一緒に渡す

	//transformの更新があれば反映
	//m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());


}
