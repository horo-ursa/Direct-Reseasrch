class VertexBuffer;
class Graphics;
class RenderObj
{
public:
	struct bufferData {
		float time;
		float pad1;
		float pad2;
		float pad3;
	};

	RenderObj(const VertexBuffer* vertexBuffer);
	virtual ~RenderObj();
	virtual void Draw();
	void Update(float deltaTime);

private:
	const VertexBuffer* vBuffer;
	bufferData bufferData;
	ID3D11Buffer* constBuffer;
	Graphics* mGraphics;
};