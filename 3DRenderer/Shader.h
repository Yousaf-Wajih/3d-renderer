#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
	Shader(const std::string& vertexCode, const std::string& fragmentCode);
	void Use();

	void SetValue(const std::string& name, float value);
	void SetValue(const std::string& name, glm::vec3 value);
	void SetValue(const std::string& name, glm::mat4 value);

private:
	uint32_t programID;
};
