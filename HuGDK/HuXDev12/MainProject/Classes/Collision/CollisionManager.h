//
// CollisionManager.h
// コライダー管理クラス

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "BoxCollider.h"
#include "..\Rigidbody.h"
#include "CollisionDetection.h"

class CollisionManager {
public:
    void Update();
    void AddCollider(BoxCollider* collider); 
    void RemoveCollider(BoxCollider* collider); 
    void Clear(); 
    void ContactCheck();
    void DrawOne(DirectXTK::BasicEffect& be, DirectXTK::GeometricPrimitive& shape);
    void DrawTwo(DirectXTK::BasicEffect& be, DirectXTK::GeometricPrimitive& shape);

    const std::vector<BoxCollider*>& GetAllColliders() const { return m_colliders; }

private:

    std::vector<BoxCollider*> m_colliders;

    CollisionDetection m_Detection;

};