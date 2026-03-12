#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

struct VertexType
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
};


class OBJLoader
{
public:
	static bool LoadOBJ(const string& filename, vector<VertexType>& vertices, vector<unsigned int>& indices);
private:
	
};

