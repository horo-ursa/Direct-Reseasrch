class Graphics;
class VertexBuffer
{
public:
	VertexBuffer(
		const void* vertData, uint32_t vertCount, uint32_t vertStride,
		const void* indexData, uint32_t indexCount, uint32_t indexStride
	);
	VertexBuffer(const void* vertData, uint32_t vertCount, uint32_t vertStride);
	~VertexBuffer();
	void Draw() const;

	const void* data;
	UINT datasize;
private:
	ID3D11Buffer* verBuffer;
	ID3D11Buffer* indexBuffer;
	UINT stride;
	UINT offset;
	int vertices;
	int index;
	UINT index_stride;
	DXGI_FORMAT format;
	Graphics* mGraphic;
};