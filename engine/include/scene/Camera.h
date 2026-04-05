#pragma once

#include <glm/glm.hpp>

enum ProjectionType
{
	PERSPECTIVE,
	ORTOGRAPHIC
};

class Camera
{
public:
	Camera(float fov, int width, int height, float nearClip, float farClip,
		ProjectionType type);
	Camera(float fov, int width, int height, float nearClip, float farClip,
		ProjectionType type, const glm::vec3& position);

	~Camera() = default;

	inline void SetViewportSize(int width, int height)
	{
		m_ViewportWidth = width; m_ViewportHeight = height;
		m_AspectRatio = m_ViewportWidth / (float)m_ViewportHeight;
		CalcProjectionMatrix();
	}

	void SetProjection(ProjectionType type)
	{
		m_Type = type;
		CalcProjectionMatrix();
	}

	void UpdateViewProjection();

	// Transform component
	glm::vec3& GetTransform() { return m_position; }
	const glm::vec3& GetPostion() const { return m_position; }

	bool IsOrthographic() const { return m_Type == ORTOGRAPHIC; }
	bool IsPerspective() const { return m_Type == PERSPECTIVE; }

	const glm::mat4& GetViewMatrix() { return m_ViewMatrix; }
	const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

	const glm::mat4& GetProjectionMatrix() { return m_ProjectionMatrix; }
	const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

	const glm::mat4& GetProjectionViewMatrix() { return m_ViewProjectionMatrix; }
	const glm::mat4& GetProjectionViewMatrix() const { return m_ViewProjectionMatrix; }

	const glm::vec3& GetPosition() const;

	float GetFOV() const { return m_FOVdeg; }
	float GetNearPlane() const { return m_NearPlane; }
	float GetFarPlane() const { return m_FarPlane; }
	float GetAspectRatio() const { return m_AspectRatio; }
	ProjectionType GetProjection() const { return m_Type; }

private:
	const float PITCH = 0.0f, YAW = -90.0f;

	void Init();

	void CalcProjectionMatrix();
	void CalcViewAndProjectionMatrix();

	glm::vec3 m_position;
	glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 m_Right;

	glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
	glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

	ProjectionType m_Type = PERSPECTIVE;

	// Field of view
	float m_FOVdeg = 45.0f; // degrees

	// Clipping planes
	float m_NearPlane = 0.1f;
	float m_FarPlane = 100.0f;

	// Viewport rect
	int m_ViewportWidth = 800;
	int m_ViewportHeight = 600;
	float m_AspectRatio = (float)800 / 600;
};