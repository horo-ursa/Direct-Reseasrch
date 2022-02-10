class Graphics;
class VertexBuffer
{
public:
	VertexBuffer(const void* vertData, uint32_t vertCount, uint32_t vertStride);
	VertexBuffer(
		const void* vertData, uint32_t vertCount, uint32_t vertStride,
		const void* indexData, uint32_t indexCount, uint32_t indexStride
	);
	~VertexBuffer();
	void Draw() const;

	const void* data;
	UINT datasize;

private:
	ID3D11Buffer* verBuffer;
	UINT stride;
	UINT offset;
	int vertices;
	Graphics* mGraphic;

	ID3D11Buffer* indexBuffer;
	int index;
	UINT index_stride;
	DXGI_FORMAT format;
};