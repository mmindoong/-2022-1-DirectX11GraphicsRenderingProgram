#pragma once

#include "Common.h"

#include "Cube/BaseCube.h"
#include "Renderer/DataTypes.h"
#include "Renderer/Renderable.h"

class CustomCube : public BaseCube
{
public:
	CustomCube(const std::filesystem::path& textureFilePath);
	CustomCube() = default;
	~CustomCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
	XMMATRIX mSpin = XMMatrixIdentity();
	XMMATRIX mOrbit = XMMatrixIdentity();
};