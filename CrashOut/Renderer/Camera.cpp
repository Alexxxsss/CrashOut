#include "Camera.h"
#include <Core/UtilityFunctions.h>
#include <stdexcept>
#include <cmath>



void Camera::Init(int width, int height)
{
	constexpr float pitch_radians =
		DirectX::XMConvertToRadians(30.0f);

	Rotate(pitch_radians, 0.0f);

	UpdateAspectRatio(width, height);
}

void Camera::UpdateAspectRatio(int width, int height)
{
	if (width <= 0 || height <= 0)
		return;

	myAspectRatio =
		static_cast<float>(width) /
		static_cast<float>(height);

	CalculateProjection();
}


void Camera::CalculateProjection()
{
	// Convert degrees to radians
	float field_of_view_radians = DirectX::XMConvertToRadians(myFieldOfViewDegrees);

	// Calculate camera's perspective
	myProjection = DirectX::XMMatrixPerspectiveFovLH(field_of_view_radians, myAspectRatio, myNearPlane, myFarPlane);
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
	DirectX::XMVECTOR offset =
		DirectX::XMLoadFloat3(&myPosition);

	DirectX::XMVECTOR eye =
		DirectX::XMVectorAdd(position, offset);

	DirectX::XMVECTOR at = offset;
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	myView = DirectX::XMMatrixLookAtLH(eye, at, up);
}

void Camera::SetFromTransform(const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT4& rotation, float fov,
    float nearPlane, float farPlane, int width, int height)
{
    using namespace DirectX;
    if (width <= 0 || height <= 0 || !(fov > 0 && fov < 180) ||
        !(nearPlane > 0 && farPlane > nearPlane) || !std::isfinite(farPlane))
        throw std::runtime_error("Invalid scene camera projection");
    XMVECTOR q = XMLoadFloat4(&rotation);
    float lengthSq = XMVectorGetX(XMVector4LengthSq(q));
    if (!std::isfinite(lengthSq) || lengthSq < 1e-8f)
        throw std::runtime_error("Invalid camera rotation");
    q = XMQuaternionNormalize(q);
    XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), q);
    XMVECTOR up = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), q);
    myView = XMMatrixLookToLH(XMLoadFloat3(&position), forward, up);
    myFieldOfViewDegrees = fov;
    myNearPlane = nearPlane;
    myFarPlane = farPlane;
    UpdateAspectRatio(width, height);
}

void Camera::UpdateFov(float fov)
{
    if (!(fov > 0 && fov < 180))
        throw std::runtime_error("Invalid FOV");
    myFieldOfViewDegrees = fov;
    if (myAspectRatio > 0) CalculateProjection();
}


void Camera::Move(float x, float y, float z)
{
	myPosition.x += x;
	myPosition.y += y;
	myPosition.z += z;

	Rotate(0.0f, 0.0f);
}