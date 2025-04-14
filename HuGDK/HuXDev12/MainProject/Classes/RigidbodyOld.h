///Rigidbody.h

#pragma once

#include <wrl/client.h> 
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "OriginalGeometric.h"

using namespace MyGeometory;

class RigidbodyOld {
public:
    bool isGravity = true;

    float ground = -3.5;
    bool isGround = false;
    bool isBound = false;

    float mass = 0.5;
    SimpleMath::Vector3 velocity;
    SimpleMath::Vector3 acceleration;
    SimpleMath::Vector3 position;
    SimpleMath::Vector3 initPos;//ç≈èâÇÃÉ|ÉWÉVÉáÉì
    SimpleMath::Vector3 m_halfExtents;
    SimpleMath::Vector3 forceAccum;
    SimpleMath::Vector3 correction;
    float hanpatsu = 0.6;

    SimpleMath::Matrix m_matrix;
    std::shared_ptr<Primitive> shape;

    RigidbodyOld(std::shared_ptr<Primitive> primitive, SimpleMath::Matrix matrix,SimpleMath::Vector3 pos,bool useGravity);//ID3D12Device device

    void Update();

    void ApplyForce(const SimpleMath::Vector3& force) {
        forceAccum += force;
    }

    void ApplyCollision();

    void ApplyCorrection(SimpleMath::Vector3& correct) {
        position += correct;
    };

    bool IsGravity() { return isGravity; };

    SimpleMath::Vector3 GetPos() { return position; };

    SimpleMath::Vector3 GetInitPos() { return initPos; };

    SimpleMath::Vector3 GetAABB() { return m_halfExtents; };

    SimpleMath::Vector3 ComputeCenterFromVertices(const std::vector<MyVertex>& vertices);

    SimpleMath::Vector3 ComputeHalfExtentsFromVertices(const std::vector<MyVertex>& vertices);

    SimpleMath::Matrix GetWorldMatrix() const {
        return SimpleMath::Matrix::CreateTranslation(position);
    }

    
    
    
    

private:
};

