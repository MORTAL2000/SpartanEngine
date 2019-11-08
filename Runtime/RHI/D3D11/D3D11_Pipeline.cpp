/*
Copyright(c) 2016-2019 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= IMPLEMENTATION ===============
#include "../RHI_Implementation.h"
#ifdef API_GRAPHICS_D3D11
//================================

//= INCLUDES ===============
#include "../RHI_Pipeline.h"
//==========================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
	RHI_Pipeline::RHI_Pipeline(const shared_ptr<RHI_Device>& device, const RHI_PipelineState& pipeline_state)
	{
		m_rhi_device	= device;
		m_state			= &pipeline_state;
	}

	RHI_Pipeline::~RHI_Pipeline()
	{

	}

    void RHI_Pipeline::SetConstantBuffer(uint32_t slot, RHI_ConstantBuffer* constant_buffer)
    {

    }

    void RHI_Pipeline::SetSampler(uint32_t slot, RHI_Sampler* sampler)
    {

    }

    void RHI_Pipeline::SetTexture(uint32_t slot, RHI_Texture* texture)
    {

    }

    void* RHI_Pipeline::CreateDescriptorSet(uint32_t hash)
    {
        return nullptr;
	}
}
#endif
