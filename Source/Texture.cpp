#include "Texture.h"

Texture::Texture(const char* name,const char* filename) {
	this->name = name;
	Load(filename);
}

Texture::~Texture() {
	
}

string& Texture::GetName() {
	return name;
}

float3 Texture::Sample(float2 coordinate) { // nearest sampling
	// Perform mirror mapping
	float2 coordinateabs = coordinate.Abs();
	
	// Convert to pixel coordinates
	int xcoord = (int)fmod(coordinateabs.x * width,width);
	int ycoord = (int)fmod(coordinateabs.y * height,height);
	return data[xcoord][ycoord];
}

void Texture::Load(const char* filename) {
	
	// Obtain full path
	char* buffer = new char[MAX_PATH];
	GetFullPath(filename,"Data\\Textures",buffer);
	this->fullpath = buffer;
	
	// Save filename too
	ExtractFileName(this->fullpath.c_str(),buffer);
	this->filename = buffer;
	delete[] buffer;
	
	// Open the heightmap
	FILE* bitmap = fopen(this->fullpath.c_str(),"rb");
	if(bitmap == NULL) {
		return;
	}
	
	// Read file header
	BITMAPFILEHEADER bmfh;
	fread(&bmfh,sizeof(BITMAPFILEHEADER),1,bitmap);
	
	// Read info header
	BITMAPINFOHEADER bmih;
	fread(&bmih,sizeof(BITMAPINFOHEADER),1,bitmap);
	
	// Create 2D matrix in memory from pic
	width = bmih.biWidth;
	height = bmih.biHeight;
	data.resize(width);
	for(unsigned int i = 0;i < data.size();i++) {
		data[i].resize(height);
	}
	
	unsigned int sizebytes = bmfh.bfSize - bmfh.bfOffBits;	
	
	// Bij 8bpp wordt van elke pixel een index in de colortable opgeslagen, dus moeten we die tabel even aanmaken
	if(bmih.biBitCount == 8) {
		// Each pixel stores an index of this color table
		RGBQUAD* colortable = new RGBQUAD[256];
		fread(colortable,sizeof(RGBQUAD),256,bitmap);

		// Read the indices
		BYTE* colortableindices = new BYTE[sizebytes];
		fread(colortableindices,1,sizebytes,bitmap);
		
		// Convert array to matrix
		int counter = 0;
		for(unsigned int x = 0;x < width;x++) {
			for(unsigned int y = 0;y < height;y++) {
				data[x][y] = float3(colortable[colortableindices[counter]].rgbRed,
									colortable[colortableindices[counter]].rgbGreen,
									colortable[colortableindices[counter]].rgbBlue)/255;
				counter++;
			}
		}
		
		delete[] colortable;
		delete[] colortableindices;
	} else if(bmih.biBitCount == 24) {
		
		// Lees de RGB-componenten
		BYTE* rgb = new BYTE[sizebytes];
		fread(rgb,1,sizebytes,bitmap);
		
		// Convert array to matrix
		int counter = 0;
		for(unsigned int x = 0;x < width;x++) {
			for(unsigned int y = 0;y < height;y++) {
				data[x][y] = float3(rgb[counter+0],
									rgb[counter+1],
									rgb[counter+2])/255;
				counter+=3;
			}
		}

		delete[] rgb;
	} else {
		fclose(bitmap);
		return;
	}
	
	fclose(bitmap);
}
