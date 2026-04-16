#pragma once
#pragma once

// GLM library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
 *	The pattern Dirty Flag allows us to know if an entity was moved during the previous frame
 */
class Transform
{
protected:
	/*SPACE INFORMATION*/
	//Local space information
	glm::vec3 m_LocalPosition = glm::vec3(0.0f);
	// euler angles
	glm::vec3 m_LocalRotation = glm::vec3(0.0f);
	glm::vec3 m_LocalScale = glm::vec3(1.0f);

	//Global space information concatenate in matrix
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);

	// Dirty flag
	bool m_isDirty = true;

protected:
	// Return local model matrix
	glm::mat4 getLocalModelMatrix()
	{
		const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
			glm::radians(m_LocalRotation.x),
			glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
			glm::radians(m_LocalRotation.y),
			glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
			glm::radians(m_LocalRotation.z),
			glm::vec3(0.0f, 0.0f, 1.0f));

		// Y * X * Z
		const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

		// translation * rotation * scale (also know as TRS matrix)
		return glm::translate(glm::mat4(1.0f), m_LocalPosition) *
			rotationMatrix *
			glm::scale(glm::mat4(1.0f), m_LocalScale);
	}

public:

	Transform() {};
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
		: m_LocalPosition(position), m_LocalRotation(rotation), m_LocalScale(scale)
	{
		computeModelMatrix();
	}

	void computeModelMatrix()
	{
		m_modelMatrix = getLocalModelMatrix();
		m_isDirty = false;
	}

	void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
	{
		m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
		m_isDirty = false;
	}

	void setLocalPosition(const glm::vec3& newPosition)
	{
		m_LocalPosition = newPosition;
		m_isDirty = true;
	}

	void setLocalRotation(const glm::vec3& newRotation)
	{
		m_LocalRotation = newRotation;
		m_isDirty = true;
	}

	void setLocalScale(const glm::vec3& newScale)
	{
		m_LocalScale = newScale;
		m_isDirty = true;
	}

	const glm::vec3& getLocalPosition() const
	{
		return m_LocalPosition;
	}

	const glm::vec3& getLocalRotation() const
	{
		return m_LocalRotation;
	}

	const glm::vec3& getLocalScale() const
	{
		return m_LocalScale;
	}

	const glm::mat4& getModelMatrix() const
	{
		return m_modelMatrix;
	}

	bool isDirty() const
	{
		return m_isDirty;
	}

	/*
	 * Move the local position by units.
	 */
	void move(const glm::vec3& units)
	{
		m_LocalPosition += units;
		m_isDirty = true;
	}

	/*
	 *	Rotate the local rotation by units.
	 *
	 *	The units used are euler angles.
	 */
	void rotate(glm::vec3& units)
	{
		m_LocalRotation += units;
		m_isDirty = true;
	}

	/*
	 * Increase the local scale value by a number units
	 */
	void scale(glm::vec3 units)
	{
		m_LocalScale += units;
		m_isDirty = true;
	}


	//const glm::vec3& getWorldPosition();
	//const glm::vec3& getWorldRotation();
	//const glm::vec3& getWorldScale();
};
