#include "Gui.h"

using namespace std;

#define PROGRAM_NAME "gui"


#define TEX_COLS 16
#define TEX_ROWS 16
#define TEX_WIDTH 512
#define TEX_HEIGHT 512
#define LETTER_WIDTH ((float)TEX_WIDTH/TEX_COLS)
#define LETTER_HEIGHT ((float)TEX_HEIGHT/TEX_ROWS)




Gui::Gui(int screenWidth, int screenHeight) : 
	screenWidth(screenWidth), screenHeight(screenHeight) 
{
	// nacist shader
	ShaderManager::loadProgram(PROGRAM_NAME);
}





void Gui::updateData()
{
	vertices.clear();
	texcoords.clear();
	indices.clear();

	unsigned int indexI = 0;

	// rozmery pismena v souradnicich zarizeni
	float letterWidth = (LETTER_WIDTH / screenWidth) * 2;
	float letterHeight = (LETTER_HEIGHT / screenHeight) * 2;

	for (vector<pair<string, POSITION>>::iterator it = strings.begin(); it != strings.end(); it++)
	{
		pair<string, POSITION> p = (*it);
		string str = p.first;
		POSITION position = p.second;

		// rozmery bounding boxu v rozmerech zarizeni
		float boxWidth = str.size() * letterWidth;
		float boxHeight = letterHeight;

		// levy horni roh bounding boxu
		glm::vec2 topleft(-1, 1);
		
		switch (position.left)
		{
			case LEFT: // prilepene k levemu okraji
				topleft.x = -1;
				break;
			case CENTER: // vycentrovane
				topleft.x = 0 - boxWidth / 2;
				break;
			case RIGHT: // prilepene k pravemu okraji
				topleft.x = 1 - boxWidth;
				break;
			default: // procentualni pozice zleva
				topleft.x = ((float)position.left - 50) / 50;
		}

		switch (position.top)
		{
			case TOP: // prilepene k hornimu okraji
				topleft.y = 1;
				break;
			case CENTER: // vycentrovane
				topleft.y = 0 - boxHeight / 2;
				break;
			case BOTTOM: // prilepene k dolnimu okraji
				topleft.y = -1 + boxHeight;
				break;
			default: // procentualni pozice zleva
				topleft.y = ((float)position.top - 50) / 50;
		}
		

		for (unsigned int i = 0; i < str.size(); i++)
		{			
			LTRTEXCOORDS tex = getCharTexCoords(str[i]);

			vertices.push_back(topleft.x + i * letterWidth);
			vertices.push_back(topleft.y);

			texcoords.push_back(tex.topleft.x);
			texcoords.push_back(tex.topleft.y);
			
			vertices.push_back(topleft.x + (i + 1) * letterWidth);
			vertices.push_back(topleft.y);

			texcoords.push_back(tex.topright.x);
			texcoords.push_back(tex.topright.y);

			vertices.push_back(topleft.x + (i + 1) * letterWidth);
			vertices.push_back(topleft.y - letterHeight);

			texcoords.push_back(tex.btmright.x);
			texcoords.push_back(tex.btmright.y);

			vertices.push_back(topleft.x + i * letterWidth);
			vertices.push_back(topleft.y - letterHeight);

			texcoords.push_back(tex.btmleft.x);
			texcoords.push_back(tex.btmleft.y);

			indices.push_back(indexI);
			indices.push_back(indexI + 1);
			indices.push_back(indexI + 2);
			indices.push_back(indexI + 3);
			indexI += 4;
		}
	}	

}




Gui::LTRTEXCOORDS Gui::getCharTexCoords(char c)
{	
	// pozice znaku v texture
	unsigned int index = c;
	LTRTEXCOORDS tex;
	
	tex.topleft.x = tex.btmleft.x = (index % TEX_COLS) * (1.f / TEX_COLS);
	tex.topright.x = tex.btmright.x = tex.topleft.x + (1.f / TEX_COLS);
	
	tex.topleft.y = tex.topright.y = 1 - (index / TEX_COLS) * (1.f / TEX_ROWS);
	tex.btmleft.y = tex.btmright.y = tex.topleft.y - (1.f / TEX_ROWS);

	return tex;
}





void Gui::draw()
{
	// nastaveni kresleni
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	ShaderManager::PROGRAMBINDING activeBinding = ShaderManager::useProgram(PROGRAM_NAME);

	glDisableVertexAttribArray(activeBinding.matParams.ambient);
	glDisableVertexAttribArray(activeBinding.matParams.diffuse);
	glDisableVertexAttribArray(activeBinding.matParams.specular);
	glDisableVertexAttribArray(activeBinding.matParams.shininess);
	glDisableVertexAttribArray(activeBinding.normalAttrib);
	glDisableVertexAttribArray(activeBinding.tangentAttrib);
	glDisableVertexAttribArray(activeBinding.texposAttrib);
	glDisableVertexAttribArray(activeBinding.iEnabledLightsUniform);
	glDisableVertexAttribArray(activeBinding.vLightsUniform);
	glDisableVertexAttribArray(activeBinding.mMVInverseTranspose);
	glDisableVertexAttribArray(activeBinding.normalAttrib);
	glDisableVertexAttribArray(activeBinding.tangentAttrib);
	glDisableVertexAttribArray(activeBinding.texposAttrib);

	glEnableVertexAttribArray(activeBinding.positionAttrib);

	// vrcholy
	glEnableVertexAttribArray(activeBinding.positionAttrib);
	glVertexAttribPointer(activeBinding.positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)&(vertices.at(0)));

	// texturovaci souradnice
	glEnableVertexAttribArray(activeBinding.texposAttrib);
	glVertexAttribPointer(activeBinding.texposAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)&(texcoords.at(0)));
	
	// kresleni ctvercu
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, (void*)&(indices.at(0)));
}