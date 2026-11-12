#include "Camera.h"
#include <Core/UtilityFunctions.h>



void Camera::Init(int width, int height)
{
	constexpr float pitch_radians = DirectX::XMConvertToRadians(30.0f);
	this->Rotate(pitch_radians, 0.0f);
	this->UpdateAspectRatio(width, height);
	this->CalculateProjection();
}

void Camera::UpdateAspectRatio(int width, int height)
{
	myAspectRatio = static_cast<float>(width) / static_cast<float>(height);
}


void Camera::CalculateProjection()
{
	// Convert degrees to radians
	float field_of_view_radians = DirectX::XMConvertToRadians(myFieldOfViewDegrees);

	// Calculate camera's perspective
	myProjection = DirectX::XMMatrixPerspectiveFovLH(field_of_view_radians, myAspectRatio, 0.01f, 100.0f);
}


void Camera::Rotate(float pitch_radians, float yaw_radians)
{
	myPitchRadians += pitch_radians;
	myYawRadians += yaw_radians;
	
	myPitchRadians = CommonUtilities::Clamp<float>(myPitchRadians, -(DirectX::XM_PIDIV2 - 0.1f), DirectX::XM_PIDIV2 - 0.1f);

	// Convert Spherical to Cartesian coordinates.
	const float radius = -8.0f;
	DirectX::XMMATRIX rotation_matrix = DirectX::XMMatrixRotationRollPitchYaw(myPitchRadians, myYawRadians, 0);
	DirectX::XMVECTOR position = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
	position = XMVector3TransformCoord(position, rotation_matrix);

	// Calculate camera's view
	DirectX::XMVECTOR eye = position;
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	myView = DirectX::XMMatrixLookAtLH(eye, at, up);
}
