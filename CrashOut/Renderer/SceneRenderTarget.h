#pragma once
#include "Renderer.h"

class SceneRenderTarget
{
public:
    void Resize(Renderer& renderer, UINT width, UINT height);
    void Bind(Renderer& renderer);

    ID3D11ShaderResourceView* GetSRV() const
    {
        return mySRV.Get();
    }

private:
    ComPtr<ID3D11Texture2D> myTexture;
    ComPtr<ID3D11RenderTargetView> myRTV;
    ComPtr<ID3D11ShaderResourceView> mySRV;

    ComPtr<ID3D11Texture2D> myDepthTexture;
    ComPtr<ID3D11DepthStencilView> myDSV;

    UINT myWidth = 0;
    UINT myHeight = 0;
};