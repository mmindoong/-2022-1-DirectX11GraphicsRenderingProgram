#include "Cube/BigCube.h"

BigCube::BigCube(const std::filesystem::path& textureFilePath)
	:BaseCube(textureFilePath) {}

void BigCube::Update(_In_ FLOAT deltaTime)
{
	// 1st Cube: Rotate around the origin ´©Àû
	m_world *= XMMatrixRotationY(deltaTime);
}