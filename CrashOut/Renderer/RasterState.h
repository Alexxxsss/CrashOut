#pragma once
#include <wrl/client.h>

class Renderer;

struct ID3D11RasterizerState;

class RasterState
{
	Renderer* myRenderer = nullptr;

public:
	RasterState(Renderer* renderer);
	virtual ~RasterState() = default;

	// Bind the raster state to the pipeline
	void Use();

	// Toggle between solid and wireframe
	void ToggleWireframe();

private:
	bool myRenderWireframe = false;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> myRasterStateSolid = nullptr;
	void CreateRasterStateSolid();

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> myRasterStateWireframe = nullptr;
	void CreateRasterStateWireframe();
};