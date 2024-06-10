#pragma once

class indexBuffer {
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	indexBuffer(const unsigned int* data, unsigned int count);
	~indexBuffer();

	void bind() const;
	void unbind() const;

	inline unsigned int getCount() const { return m_Count; }
};