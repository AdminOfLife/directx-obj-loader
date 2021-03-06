#include "DXUT.h"
#include "TTexture.h"

struct SimpleVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texUV;
};


TTexture::TTexture(ID3D10Device *device, TEffect *effect, ID3D10EffectTechnique *technique, LPCTSTR textureFileName)
{
	this->device = device;
	this->effect = effect;
	this->technique = technique;

	materialDiffuseColour = D3DXVECTOR4(1, 1, 1, 1);

	HRESULT hr;		//Used by the V() macro.  

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		{ D3DXVECTOR3( -1.0f, 0.0f, -1.0f ), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2( 0.0f, 0.0f ) },
		{ D3DXVECTOR3( 1.0f, 0.0f, -1.0f ),  D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2( 1.0f, 0.0f ) },
		{ D3DXVECTOR3( 1.0f, 0.0f, 1.0f ),   D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2( 1.0f, 1.0f ) },
		{ D3DXVECTOR3( -1.0f, 0.0f, 1.0f ),  D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2( 0.0f, 1.0f ) },
	};

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( SimpleVertex ) * 4;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	hr = device->CreateBuffer( &bd, &InitData, &pVertexBuffer );
	V( hr ) 


	//D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( SimpleVertex ) * 4;
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	//D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	// Create index buffer
	DWORD indices[] =
	{
		3,1,0,
		2,1,3    
	};


	InitData.pSysMem = indices;
	hr = device->CreateBuffer( &bd, &InitData, &pIndexBuffer );

	// Load the Texture
	hr = D3DX10CreateShaderResourceViewFromFile( device, textureFileName, NULL, NULL, 
		&ptxtResourceViewInShader, NULL );
	V( hr );
}


TTexture::~TTexture(void)
{
	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(pIndexBuffer);
	SAFE_RELEASE(ptxtResourceViewInShader);
}

void TTexture::Render()
{
	SetupForRender();
	FastRender();
}

void TTexture::FastRender()
{
	D3D10_TECHNIQUE_DESC techDesc;
	technique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		technique->GetPassByIndex( p )->Apply( 0 );
		device->DrawIndexed(6, 0, 0 );
	}
}

void TTexture::SetupForRender()
{
	effect->g_p_MaterialDiffuseColourInShader->SetFloatVector((float *) &materialDiffuseColour);

	// Set vertex buffer
	UINT stride = sizeof( SimpleVertex );
	UINT offset = 0;
	device->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );
	device->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Set primitive topology
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//Set the texture. 
	effect->g_p_txDiffuseInShader->SetResource( ptxtResourceViewInShader );
	
}
