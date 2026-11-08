#pragma once

#include <DirectXMath.h>

// Perspective orbital camera
class Camera
{
public:
	Camera(int width, int height);
	virtual ~Camera() = default;

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

private:
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

	// Recalculates the projection based on the new window size
	void CalculateProjection();
};