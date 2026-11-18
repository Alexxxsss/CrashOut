#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
class Renderer;

class TextureSampler
{
	Renderer* myRenderer = nullptr;

public:
	TextureSampler(Renderer* renderer);
	virtual ~TextureSampler() = default;

	// Bind texture sampler to pipeline
	void Use();
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>LoadTextureFromFile(ID3D11Device* device, const std::wstring& filePath);
private:
	ComPtr<ID3D11SamplerState> m_SamplerState;
	void CreateSamplerState();
};