#pragma once

#include "Common.h"

#include "Cube/BaseCube.h"
#include "Renderer/DataTypes.h"
#include "Renderer/Renderable.h"

class SmallCube : public BaseCube
{
public:
	SmallCube(const std::filesystem::path& textureFilePath);
	SmallCube() = default;
	~SmallCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
protected:
	XMMATRIX mSpin = XMMatrixIdentity();
	XMMATRIX mOrbit = XMMatrixIdentity();

};