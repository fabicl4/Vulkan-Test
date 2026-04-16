#include <scene/Camera.h>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, int width, int height, float nearClip, float farClip,
	ProjectionType type)
	: m_FOVdeg(fov), m_NearPlane(nearClip), m_FarPlane(farClip),
	m_ViewportWidth(width), m_ViewportHeight(height),
	m_Type(type)
{
	Init();
}

Camera::Camera(float fov, int width, int height, float nearClip, float farClip,
	ProjectionType type, const glm::vec3& position)
	: m_FOVdeg(fov), m_NearPlane(nearClip), m_FarPlane(farClip),
	m_ViewportWidth(width), m_ViewportHeight(height),
	m_Type(type),
	m_position(position)
{
	Init();
}

void Camera::Init()
{
	m_AspectRatio = m_ViewportWidth / (float)m_ViewportHeight;
	CalcProjectionMatrix();
	CalcViewAndProjectionMatrix();
}

void Camera::UpdateViewProjection()
{
	// Calculate projection matrix.
	m_AspectRatio = m_ViewportWidth / (float)m_ViewportHeight;
	CalcProjectionMatrix();

	// Update view camera matrix
	CalcViewAndProjectionMatrix();
}

void Camera::CalcProjectionMatrix()
{
	m_ProjectionMatrix = glm::mat4(1.0f);
	if (IsPerspective())
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOVdeg),
			m_AspectRatio, m_NearPlane, m_FarPlane);

	else if (IsOrthographic())
	{
		/*
		// The center of the screen is the origin of coordinates (0,0)
		m_ProjectionMatrix = glm::ortho(
			-(m_ViewportWidth / 2.0f),	// left
			m_ViewportWidth / 2.0f,		// right
			-(m_ViewportHeight / 2.0f), // bottom
			(m_ViewportHeight / 2.0f), // top 
			m_NearPlane, m_FarPlane // near, far
		);*/

		// TOP-LEFT is the origin of coordinates (0,0)
		m_ProjectionMatrix = glm::ortho(
			0.0f, (float)m_ViewportWidth, 
			(float)m_ViewportHeight, 0.0f,
			m_NearPlane, m_FarPlane);
	}
}

void Camera::CalcViewAndProjectionMatrix()
{
	m_ViewMatrix = glm::lookAt(m_position, m_position + m_Front, m_Up);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

const glm::vec3& Camera::GetPosition() const
{
	return m_position;
}