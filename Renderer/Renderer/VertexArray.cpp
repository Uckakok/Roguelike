#include"pch.h"
#include"VertexArray.h"

#include"VertexBufferLayout.h"
#include"Renderer.h"

vertexArray::vertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

vertexArray::~vertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

// Metoda dodaj¹ca bufor wierzcho³ków do tablicy wierzcho³ków
void vertexArray::addBuffer(const vertexBuffer& vb, const vertexBufferLayout& layout)
{
	bind();
	vb.bind();
	const auto& elements = layout.getElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); ++i) {
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset));
		offset += element.count * vertexBufferElement::getSizeOfType(element.type);
	}
}

void vertexArray::bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void vertexArray::unbind() const
{
	GLCall(glBindVertexArray(0));
}
