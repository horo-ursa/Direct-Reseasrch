class VertexBuffer;
class RenderObj
{
public:
	RenderObj(const VertexBuffer* vertexBuffer);
	virtual ~RenderObj();
	virtual void Draw();

private:
	const VertexBuffer* vBuffer;
};