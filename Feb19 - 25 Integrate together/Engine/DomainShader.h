#pragma once

class DomainShader
{
public:
	DomainShader();
	~DomainShader();
	bool Load(const WCHAR* fileName);
	void SetActive() const ;
private:
	ID3D11DomainShader* mShaderDS;
};