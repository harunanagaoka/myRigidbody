//
// BoxCollider.h
//

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include <array>

using namespace std;

class BoxCollider {
public:
    BoxCollider();//�Ȃ������Ȃ���ΒP�ʗ����̂��AVec3���ꂽ���͔C�ӃT�C�Y�ɂł���悤�ɂ���

    void SetSize(const SimpleMath::Vector3& size); // �C�ӃT�C�Y�̐ݒ�

    // Transform�𔽉f(Transform���ς�������A�K�����L�ґ�����Ăяo�����Ɓj
    void UpdateWorldVertices(const SimpleMath::Matrix& worldMatrix); 

    const array<SimpleMath::Vector3, 8> GetWorldVertices() const { return m_worldVertices; }

private:
    SimpleMath::Vector3 m_localSize = SimpleMath::Vector3(1.0f, 1.0f, 1.0f); // �R���C�_�[�̃��[�J���T�C�Y
     
    array<SimpleMath::Vector3, 8> m_localVertices;// ���[�J�����_ 

    array<SimpleMath::Vector3, 8> m_worldVertices; // ���[���h��Ԃ̒��_

    const std::array<SimpleMath::Vector3, 8> m_primitiveVertices =
    {   // �P�ʗ����́i���S���_�j
        //�e���_��size�ivector3�j���������size�̑傫���̒����̂ɂȂ�
        SimpleMath::Vector3(-0.5f, -0.5f, -0.5f),
        SimpleMath::Vector3( 0.5f, -0.5f, -0.5f),
        SimpleMath::Vector3(-0.5f,  0.5f, -0.5f),
        SimpleMath::Vector3( 0.5f,  0.5f, -0.5f),
        SimpleMath::Vector3(-0.5f, -0.5f,  0.5f),
        SimpleMath::Vector3( 0.5f, -0.5f,  0.5f),
        SimpleMath::Vector3(-0.5f,  0.5f,  0.5f),
        SimpleMath::Vector3( 0.5f,  0.5f,  0.5f),
    };

    //�i�O����j�Փ˂��Ă�J�����R���C�_�[����transform���烏�[���h���_�擾
    //���[���h���_�Ԃ������̃{�b�N�X�̃��[���h���_�Ɣ�ׂďՓ˓_�𒲂ׂĂ��炤

    // �O�p�`�C���f�b�N�X�i�e��2���j �K�v���͓�
    //const std::array<unsigned int, 36> m_triangleIndices = {
    //    // �O�� (z+)
    //    4, 6, 5,
    //    5, 6, 7,

    //    // �w�� (z-)
    //    0, 1, 2,
    //    1, 3, 2,

    //    // ���� (x-)
    //    0, 2, 4,
    //    4, 2, 6,

    //    // �E�� (x+)
    //    1, 5, 3,
    //    5, 7, 3,

    //    // ��� (y+)
    //    2, 3, 6,
    //    3, 7, 6,

    //    // ���� (y-)
    //    0, 4, 1,
    //    1, 4, 5
    //};
};