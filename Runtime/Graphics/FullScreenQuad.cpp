/*
Copyright(c) 2016-2017 Panos Karabelas

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

//= INCLUDES ==================
#include "FullScreenQuad.h"
#include "../Core/Helper.h"
#include "../Graphics/Vertex.h"
#include <winerror.h>
//=============================

//= NAMESPACES ================
using namespace std;
using namespace Directus::Math;
//=============================

namespace Directus
{
	FullScreenQuad::FullScreenQuad()
	{
		m_graphics = nullptr;
		m_vertexBuffer = nullptr;
		m_indexBuffer = nullptr;
	}

	FullScreenQuad::~FullScreenQuad()
	{
		SafeRelease(m_vertexBuffer);
		SafeRelease(m_indexBuffer);
	}

	bool FullScreenQuad::Initialize(int width, int height, Graphics* graphics)
	{
		m_graphics = graphics;
		if (!m_graphics->GetDevice())
			return false;

		// Calculate the screen coordinates of the left side of the window.
		float left = static_cast<float>((width / 2) * -1);

		// Calculate the screen coordinates of the right side of the window.
		float right = left + static_cast<float>(width);

		// Calculate the screen coordinates of the top of the window.
		float top = static_cast<float>(height / 2);

		// Calculate the screen coordinates of the bottom of the window.
		float bottom = top - static_cast<float>(height);

		// Create index and vertex arrays
		vector<VertexPosTex> vertices;
		vector<unsigned long> indices;
		int indexCount = 6;

		// Load the vertex array with data.
		// First triangle.
		VertexPosTex vertex;
		vertex.position = Vector3(left, top, 0.0f); // Top left.
		vertex.uv = Vector2(0.0f, 0.0f);
		vertices.push_back(vertex);

		vertex.position = Vector3(right, bottom, 0.0f); // Bottom right.
		vertex.uv = Vector2(1.0f, 1.0f);
		vertices.push_back(vertex);

		vertex.position = Vector3(left, bottom, 0.0f); // Bottom left.
		vertex.uv = Vector2(0.0f, 1.0f);
		vertices.push_back(vertex);

		// Second triangle.
		vertex.position = Vector3(left, top, 0.0f); // Top left.
		vertex.uv = Vector2(0.0f, 0.0f);
		vertices.push_back(vertex);

		vertex.position = Vector3(right, top, 0.0f);// Top right.
		vertex.uv = Vector2(1.0f, 0.0f);
		vertices.push_back(vertex);

		vertex.position = Vector3(right, bottom, 0.0f); // Bottom right.
		vertex.uv = Vector2(1.0f, 1.0f);
		vertices.push_back(vertex);

		// Load the index array with data.
		for (int i = 0; i < indexCount; i++)
		{
			indices.push_back(i);
		}

		// Set up the description of the vertex buffer.
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexPosTex) * vertices.size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = vertices.data();
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// Now finally create the vertex buffer.
		auto hResult = m_graphics->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(hResult))
			return false;

		// Set up the description of the index buffer.
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * indices.size();
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.
		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = indices.data();
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		hResult = m_graphics->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (FAILED(hResult))
			return false;

		return true;
	}

	void FullScreenQuad::SetBuffers()
	{
		// Set vertex buffer stride and offset.
		unsigned int stride = sizeof(VertexPosTex);
		unsigned int offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		m_graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		m_graphics->GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		m_graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}