#pragma once

#include <DirectXMath.h>

// Perspective orbital camera
class Camera
{
public:
	Camera() = default;
	virtual ~Camera() = default;

	void Init(int width, int height);

    // Scene camera: quaternion rotation, +Z forward, +Y up.
    void SetFromTransform(const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT4& rotation, float fov,
        float nearPlane, float farPlane, int width, int height);

	// Recalculates the view based on the pitch and yaw
	void Rotate(float pitch, float yaw);

	// Update aspect ratio
	void UpdateAspectRatio(int width, int height);

	// Set field of view
	void UpdateFov(float fov);

	// Get projection matrix
	inline DirectX::XMMATRIX GetProjection() const { return myProjection; }

	// Get view matrix
	inline DirectX::XMMATRIX GetView() const { return myView; }

	void Move(float x, float y, float z);

private:
	DirectX::XMFLOAT3 myPosition = { 0.0f, 0.0f, 0.0f };

	// Projection matrix
	DirectX::XMMATRIX myProjection;

	// View matrix
	DirectX::XMMATRIX myView;

	// Camera pitch in radians
	float myPitchRadians = 0.0f;

	// Camera yaw in radians
	float myYawRadians = 0.0f;

	// Camera field of view in degrees
	float myFieldOfViewDegrees = 50.0f;

	// Aspect ratio
	float myAspectRatio = 0.0f;
    float myNearPlane = 0.01f;
    float myFarPlane = 100.0f;

	// Recalculates the projection based on the new window size
	void CalculateProjection();
};
