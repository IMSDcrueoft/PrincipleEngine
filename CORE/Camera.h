#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <tuple>
#include "D3DMethods.h"

class Camera
{
public:
	Camera();
	~Camera() = default;

	// Camera position setting.
	DirectX::XMVECTOR GetPosition()const;
	DirectX::XMFLOAT3 GetPosition3f()const;
	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);

	// Get camera xyz axis.
	DirectX::XMVECTOR GetRight()const;
	DirectX::XMVECTOR GetUp()const;
	DirectX::XMVECTOR GetLook()const;

	const DirectX::XMFLOAT3& GetRight3f()const;
	const DirectX::XMFLOAT3& GetUp3f()const;
	const DirectX::XMFLOAT3& GetLook3f()const;

	// Get frustum properties.
	const float& GetNearZ()const;
	const float& GetFarZ()const;
	const float& GetAspect()const;
	const float& GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	const float& GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	const float& GetFarWindowHeight()const;

	// Set the frustum of the camera's field of view.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	// Get View/Proj matrices.
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;

	const DirectX::XMFLOAT4X4& GetView4x4f()const;
	const DirectX::XMFLOAT4X4& GetProj4x4f()const;

	// Strafe/Walk/Rise the camera a distance -> d.
	void TransferPos(float d, DirectX::XMFLOAT3& dir);
	void Strafe(float d);
	void Walk(float d);
	void Rise(float d);

	// Rotate the camera clockwise .
	void RotateAroundAxis(const DirectX::XMFLOAT3& vec, float angle);
	void Pitch(float angle);
	void RotateY(float angle);
	void RowLook(float angle);

	// Every time you modify the camera position/orientation
	// you need to call the reconstruction view matrix.
	void UpdateViewMatrix();

private:

	DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f }; // POS
	DirectX::XMFLOAT3 right = { 1.0f, 0.0f, 0.0f }; // X
	DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f }; // Y
	DirectX::XMFLOAT3 look = { 0.0f, 0.0f, 1.0f }; // Z

	// Cache frustum properties.
	float nearZ = 0.0f;
	float farZ = 0.0f;
	float aspectR = 0.0f; // w/h
	float FovY = 0.0f; // alpha
	float nearWindowHeight = 0.0f;
	float farWindowHeight = 0.0f;

	bool viewDirty = true;

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 view = MathHelpTool::Identity4x4(); // view matrix
	DirectX::XMFLOAT4X4 proj = MathHelpTool::Identity4x4(); // projection matrix
};

//-----------------------------------------------------------------------------
inline Camera::Camera()
{
	SetLens(0.25f * MathHelpTool::Pi, 1.0f, 1.0f, 1000.0f);
}

inline DirectX::XMVECTOR Camera::GetPosition() const
{
	return XMLoadFloat3(&position);
}

inline DirectX::XMFLOAT3 Camera::GetPosition3f() const
{
	return position;
}

inline void Camera::SetPosition(const DirectX::XMFLOAT3& pos)
{
	position = pos;
	viewDirty = true;
}

inline void Camera::SetPosition(float x, float y, float z)
{
	position = DirectX::XMFLOAT3(x, y, z);
	viewDirty = true;
}

inline DirectX::XMVECTOR Camera::GetRight() const
{
	return DirectX::XMLoadFloat3(&right);
}

inline DirectX::XMVECTOR Camera::GetUp() const
{
	return DirectX::XMLoadFloat3(&up);
}

inline DirectX::XMVECTOR Camera::GetLook() const
{
	return DirectX::XMLoadFloat3(&look);
}

inline const DirectX::XMFLOAT3& Camera::GetRight3f() const
{
	return right;
}

inline const DirectX::XMFLOAT3& Camera::GetUp3f() const
{
	return up;
}

inline const DirectX::XMFLOAT3& Camera::GetLook3f() const
{
	return look;
}

inline const float& Camera::GetNearZ() const
{
	return nearZ;
}

inline const float& Camera::GetFarZ() const
{
	return farZ;
}

inline const float& Camera::GetAspect() const
{
	return aspectR;
}

inline const float& Camera::GetFovY() const
{
	return FovY;
}

inline float Camera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / nearZ);
}

inline float Camera::GetNearWindowWidth() const
{
	return aspectR * nearWindowHeight;
}

inline const float& Camera::GetNearWindowHeight() const
{
	return nearWindowHeight;
}

inline float Camera::GetFarWindowWidth() const
{
	return aspectR * farWindowHeight;
}

inline const float& Camera::GetFarWindowHeight() const
{
	return farWindowHeight;
}

inline void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	std::tie(FovY, aspectR, nearZ, farZ) = std::tie(fovY, aspect, zn, zf);

	float magnif = 2.0f * tanf(0.5f * FovY);

	nearWindowHeight = nearZ * magnif;
	farWindowHeight = farZ * magnif;

	XMStoreFloat4x4(&proj, DirectX::XMMatrixPerspectiveFovLH(FovY, aspectR, nearZ, farZ));
}

inline void Camera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp)
{
	DirectX::XMVECTOR L = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target, pos));
	DirectX::XMVECTOR R = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, L));
	DirectX::XMVECTOR U = DirectX::XMVector3Cross(L, R);

	DirectX::XMStoreFloat3(&position, pos);
	DirectX::XMStoreFloat3(&look, L);
	DirectX::XMStoreFloat3(&right, R);
	DirectX::XMStoreFloat3(&up, U);

	viewDirty = true;
}

inline void Camera::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
	DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&up);

	LookAt(P, T, U);

	viewDirty = true;
}

inline DirectX::XMMATRIX Camera::GetView() const
{
	assert(!viewDirty);
	return DirectX::XMLoadFloat4x4(&view);
}

inline DirectX::XMMATRIX Camera::GetProj() const
{
	return DirectX::XMLoadFloat4x4(&proj);
}

inline const DirectX::XMFLOAT4X4& Camera::GetView4x4f() const
{
	assert(!viewDirty);
	return view;
}

inline const DirectX::XMFLOAT4X4& Camera::GetProj4x4f() const
{
	return proj;
}

inline void Camera::TransferPos(float d, DirectX::XMFLOAT3& dir)
{
	// position += d*direction
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
	DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&dir);
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&position);
	DirectX::XMStoreFloat3(&position, DirectX::XMVectorMultiplyAdd(s, v, p));

	viewDirty = true;
}

inline void Camera::Strafe(float d)
{
	// position += d*right
	TransferPos(d, right);
}

inline void Camera::Walk(float d)
{
	// position += d*look
	TransferPos(d, look);
}

inline void Camera::Rise(float d)
{
	TransferPos(d, up);
}

inline void Camera::RotateAroundAxis(const DirectX::XMFLOAT3& vec, float angle)
{
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&vec), angle);

	DirectX::XMStoreFloat3(&right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&right), R));
	DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), R));
	DirectX::XMStoreFloat3(&look, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&look), R));

	viewDirty = true;
}

inline void Camera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&right), angle);

	DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), R));
	DirectX::XMStoreFloat3(&look, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&look), R));

	viewDirty = true;
}

inline void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);

	DirectX::XMStoreFloat3(&right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&right), R));
	DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), R));
	DirectX::XMStoreFloat3(&look, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&look), R));

	viewDirty = true;
}

inline void Camera::RowLook(float angle)
{
	// Rotate up and right vector about the look vector.

	DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&look), angle);

	DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up), R));
	DirectX::XMStoreFloat3(&right, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&right), R));

	viewDirty = true;
}

inline void Camera::UpdateViewMatrix()
{
	if (viewDirty)
	{
		DirectX::XMVECTOR R = DirectX::XMLoadFloat3(&right);
		DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&up);
		DirectX::XMVECTOR L = DirectX::XMLoadFloat3(&look);
		DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = DirectX::XMVector3Normalize(L);
		U = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = DirectX::XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, R));
		float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, U));
		float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, L));

		DirectX::XMStoreFloat3(&right, R);
		DirectX::XMStoreFloat3(&up, U);
		DirectX::XMStoreFloat3(&look, L);

		std::tie(view(0, 0), view(1, 0), view(2, 0), view(3, 0)) =
			std::tie(right.x, right.y, right.z, x);

		std::tie(view(0, 1), view(1, 1), view(2, 1), view(3, 1)) =
			std::tie(up.x, up.y, up.z, y);

		std::tie(view(0, 2), view(1, 2), view(2, 2), view(3, 2)) =
			std::tie(look.x, look.y, look.z, z);

		view(0, 3) = 0.0f;
		view(1, 3) = 0.0f;
		view(2, 3) = 0.0f;
		view(3, 3) = 1.0f;

		viewDirty = false;
	}
}

#endif // CAMERA_H