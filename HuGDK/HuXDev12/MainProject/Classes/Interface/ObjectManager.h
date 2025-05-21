//
// CollisionManager.h
// コライダー管理クラス

#pragma once

#include "..\..\Base\pch.h"
#include "..\..\Base\dxtk.h"
#include "..\Transform.h"
#include "..\Rigidbody.h"
#include "..\Collision\BoxCollider.h"
#include "..\Collision\CollisionDetection.h"
#include "Object.h"
#include <unordered_map>

//#include "BoxCollider.h"
//#include "..\Rigidbody.h"
//#include "CollisionDetection.h"

using namespace std;

class ObjectManager {
public:
    void Initialize();
    void Update();
    int CreateObject();//bool isTransform, bool isRigidbody, bool isCollider
    void RemoveObject(int id);
    void Clear(); 
    void ContactCheck();
    
    Transform* GetTransform(int id) { return m_objectMap[id]->GetTransform(); }
    Rigidbody* GetRigidbody(int id) { return m_objectMap[id]->GetRigidbody(); }
    BoxCollider* GetCollider(int id) { return m_objectMap[id]->GetCollider(); }

    void SetStatic(int id,bool isStatic);

    void Draw(DirectXTK::BasicEffect& be, DirectXTK::GeometricPrimitive& shape,int a);

    //const vector<Object*>& GetAllObjects() const { return m_objects; }

private:
    unordered_map<int, Object*> m_objectMap;
    vector<Object*> m_objects;

    CollisionDetection m_detection;

};