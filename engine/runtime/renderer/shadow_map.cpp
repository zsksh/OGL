#include<renderer/framebuffer.h>
#include<renderer/shadow_map.h>
namespace CGL {

	void ShadowMap::initialize(Framebuffer* frambuffer) {
		m_width = frambuffer->m_width;
		m_height = frambuffer->m_height;
		m_depth_buffer.assign(frambuffer->m_depth_buffer.begin(),
			frambuffer->m_depth_buffer.end());
	}



}