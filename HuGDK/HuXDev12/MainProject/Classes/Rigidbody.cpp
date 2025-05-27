//
// Rigidbody.cpp
//

#include "Rigidbody.h"

void Rigidbody::Update() {


	m_solver.Update(m_transform);
	m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());

}

void Rigidbody::ImpactResponse() {
	//Õ“Ëinfo‚ð‚¤‚¯‚Æ‚èrigidbody‚Étransform‚Æˆê‚É“n‚·

	//transform‚ÌXV‚ª‚ ‚ê‚Î”½‰f
	//m_collider->UpdateWorldVertices(m_transform.GetWorldMatrix());


}
