//
// CollisionManager.h
// コライダー管理クラス

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "BoxCollider.h"

class CollisionManager {
public:
    void AddCollider(BoxCollider* collider); 
    void RemoveCollider(BoxCollider* collider); 
    void Clear(); 

    const std::vector<BoxCollider*>& GetAllColliders() const { return m_colliders; }

private:

    std::vector<BoxCollider*> m_colliders;

};