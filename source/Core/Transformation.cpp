#include <r3d/Core/Transformation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace r3d
{
	Transformation::Transformation(const glm::vec3 &t, const glm::vec3 &r, 
			const glm::vec3 &s)
		: m_translation(t), m_rotation(r), m_scale(s), dirty(true)
	{

	}

	const glm::mat4 &Transformation::getMatrix() const
	{
		if(dirty)
		{
			const glm::mat4 &scale=glm::scale(glm::mat4(1.0f), m_scale);
			m_cacheRotationMatrix=glm::eulerAngleXYZ(m_rotation.x, m_rotation.y, m_rotation.z);
			const glm::mat4 &translation=glm::translate(glm::mat4(1.0f), m_translation);
			dirty=false;
			m_cacheMatrix=translation*m_cacheRotationMatrix*scale;
		}
		return m_cacheMatrix;
	}

	const glm::mat4 &Transformation::getRotationMatrix() const
	{
		if(dirty)
			getMatrix();
		return m_cacheRotationMatrix;
	}
}

#ifdef _DEBUG_MAIN
#include <iostream>
using namespace std;
int main()
{
	r3d::Transformation t;
	t.setTranslation();
	glm::mat4 ans=t.getMatrix();
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			//glm is column major
			cout<<ans[j][i]<<" ";
		}
		cout<<endl;
	}
}
#endif