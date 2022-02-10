class Graphics;
class VertexBuffer
{
public:
	VertexBuffer(const void* vertData, uint32_t vertCount, uint32_t vertStride);
	~VertexBuffer();
	void Draw() const;
private:
	ID3D11Buffer* verBuffer;
	UINT stride;
	UINT offset;
	int vertices;
	Graphics* mGraphic;
};