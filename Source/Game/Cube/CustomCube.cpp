#include "Cube/CustomCube.h"

void CustomCube::Update(_In_ FLOAT deltaTime)
{
	XMMATRIX mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

	// 2nd Cube:  Rotate around origin
	mSpin *= XMMatrixRotationZ(-deltaTime);
	mOrbit *= XMMatrixRotationY(-deltaTime * 3.0f);
	m_world = mScale * mSpin * mTranslate * mOrbit;
}