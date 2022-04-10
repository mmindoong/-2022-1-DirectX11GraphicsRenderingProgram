#include "Cube/BigCube.h"

void BigCube::Update(_In_ FLOAT deltaTime)
{
	// 1st Cube: Rotate around the origin ´©Àû
	m_world *= XMMatrixRotationY(deltaTime);
}