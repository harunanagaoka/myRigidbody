//
// Rigidbody.cpp
//

#include "Rigidbody.h"

void Rigidbody::Update() {


	m_solver.Update(m_transform);
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());

}

void Rigidbody::ImpactResponse() {
	//�Փ�info�������Ƃ�rigidbody��transform�ƈꏏ�ɓn��

	//transform�̍X�V������Δ��f
	//m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());


}
