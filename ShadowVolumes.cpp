#include "ShadowVolumes.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

// vypnout otravne warningy Visual Studia; nasim iteratorum verime :)
#define _SCL_SECURE_NO_WARNINGS


#define PROGRAM_NAME "test"
#define INFINITY 10

using namespace std;



ShadowVolumes::ShadowVolumes(void)
{
	ShaderManager::loadProgram(PROGRAM_NAME);
}


ShadowVolumes::~ShadowVolumes(void)
{
	// uvolnit meshe
	for (vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		delete (*it);
	}
}



void ShadowVolumes::generate()
{
	// seskladat vsechny meshe a rovnou je vynasobime modelovou matici
	for (vector<pair<BaseModel*, glm::mat4>>::iterator modelIt = models.begin(); modelIt != models.end(); modelIt++)
	{
		for (vector<Mesh*>::iterator meshIt = (*modelIt).first->getMeshes().begin(); meshIt != (*modelIt).first->getMeshes().end(); meshIt++)
		{
			Mesh* mesh = (*meshIt);

			if (mesh->getName() != "deska_stol")
				continue;

			Mesh* newMesh = new Mesh((*mesh) * (*modelIt).second);
			meshes.push_back(newMesh);
		}
	}

	// vypocitat sousednosti jiz nad novymi meshi
	computeNeighboursAndVisibilities();


	VBOs = vector<GLuint>(lights.size());
	EBOs = vector<GLuint>(lights.size());

	// vyrobit GL buffery
	for (unsigned int lightI = 0; lightI < lights.size(); lightI++)
	{
		glm::vec3 light = lights[lightI];
		
		vector<glm::vec3> glVertices;
		vector<unsigned int> glIndices;

		for (unsigned int meshI = 0; meshI < meshes.size(); meshI++)
		{
			Mesh* mesh = meshes[meshI];
			vector<glm::ivec3> faces = mesh->getFaces();
			vector<glm::vec3> vertices = mesh->getVertices();

			for (unsigned int faceI = 0; faceI < faces.size(); faceI++)
			{
				glm::ivec3 face = faces[faceI];

				if (meshFacesVisibilities[meshI][faceI][lightI] == true) // face je z daneho svetla viditelny
				{
					// indexy sousednich facu
					Neighbours neighbours = facesNeighbours[mesh][faceI];

					// hrana facu definovana vzdy dvema indexy vrcholu + indexem souseda
					pair<glm::ivec2, int> edges[3] = {
						pair<glm::ivec2, int>(glm::ivec2(face.x, face.y), neighbours.a),
						pair<glm::ivec2, int>(glm::ivec2(face.y, face.z), neighbours.b),
						pair<glm::ivec2, int>(glm::ivec2(face.z, face.x), neighbours.c)
					};

					for (unsigned int edgeI = 0; edgeI < 3; edgeI++)
					{		
						int neighbourIndex = edges[edgeI].second;

						// aktualni hrana je stinova, pokud nema souseda anebo je soused odvraceny od svetla
						if (neighbourIndex == -1 || meshFacesVisibilities[meshI][neighbourIndex][lightI] == false)
						{
							glm::vec3 v1 = vertices[ edges[edgeI].first.x ];
							glm::vec3 v2 = vertices[ edges[edgeI].first.y ];

							glm::vec3 v3, v4;

							v3 = ( v1 - light ) * glm::vec3(INFINITY, INFINITY, INFINITY);
							v4 = ( v2 - light ) * glm::vec3(INFINITY, INFINITY, INFINITY);

							unsigned int index = glVertices.size();

							// kreslit se budou 4 vrcholy jako quad
							glVertices.push_back( v1 );
							//glVertices.push_back( v1 + v3 );
							glVertices.push_back( v2 );
							glVertices.push_back( v3 );
							//glVertices.push_back( v2 + v4 );
							glVertices.push_back( v4 );
							
							glIndices.push_back(index + 3);
							glIndices.push_back(index + 2);
							glIndices.push_back(index + 1);
							glIndices.push_back(index + 0);

							/*
							glIndices.push_back(index);
							glIndices.push_back(index + 1);
							glIndices.push_back(index + 2);
							glIndices.push_back(index + 3);
							*/
						}
					}
				}
			}
		}

		// zkopirovat data do VBO
		glGenBuffers(1, &VBOs[lightI]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[lightI]);
		glBufferData(GL_ARRAY_BUFFER, glVertices.size() * 3 * sizeof(float), NULL, GL_STATIC_DRAW);
		
		float* mappingVBO = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		
		for (vector<glm::vec3>::iterator it = glVertices.begin(); it != glVertices.end(); it++)
		{
			*(mappingVBO + 0) = (*it).x;
			*(mappingVBO + 1) = (*it).y;
			*(mappingVBO + 2) = (*it).z;
			
			mappingVBO += 3;
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// zkopirovat data do EBO
		glGenBuffers(1, &EBOs[lightI]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[lightI]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndices.size() * sizeof(unsigned int), NULL, GL_STATIC_DRAW);
		
		unsigned int* mappingEBO = (unsigned int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
		
		copy(glIndices.begin(), glIndices.end(), mappingEBO);

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		shadowVolumeIndices = glIndices.size();
	}

}



void ShadowVolumes::computeNeighboursAndVisibilities()
{
	facesNeighbours.clear();
	meshFacesVisibilities = std::vector<std::vector<std::vector<bool>>>(meshes.size());

	// pro kazdou mesh
	for (unsigned int meshI = 0; meshI < meshes.size(); meshI++)
	{
		Mesh* mesh = meshes[meshI];
		vector<glm::vec3> const &vertices = mesh->getVertices();
		vector<glm::ivec3> faces = mesh->getFaces();

		// vytvorit novy zaznam; sousednosti zatim neexistuji
		vector<Neighbours> dummy(faces.size());
		facesNeighbours.insert(pair<Mesh*, vector<Neighbours>>(mesh, dummy));
		
		// zde budeme shromazdovat informace o viditelnostech
		vector<vector<bool>> faceVisibilities(faces.size());

		// pro kazdou face
		for (unsigned int faceIA = 0; faceIA < faces.size(); faceIA++)
		{				
			glm::ivec3 face = faces[faceIA];

			// ---------- Vypocet viditelnosti ze svetel ---------------------

			glm::vec3 v1 = vertices[ face.x ];
			glm::vec3 v2 = vertices[ face.y ];
			glm::vec3 v3 = vertices[ face.z ];

			// vypocitat koeficienty rovnice plochy 
			float a = v1.y*(v2.z-v3.z) + v2.y*(v3.z-v1.z) + v3.y*(v1.z-v2.z);
			float b = v1.z*(v2.x-v3.x) + v2.z*(v3.x-v1.x) + v3.z*(v1.x-v2.x);
			float c = v1.x*(v2.y-v3.y) + v2.x*(v3.y-v1.y) + v3.x*(v1.y-v2.y);
			float d = -( v1.x*( v2.y*v3.z - v3.y*v2.z ) + v2.x*(v3.y*v1.z - v1.y*v3.z) + v3.x*(v1.y*v2.z - v2.y*v1.z) );

			vector<bool> visibilities(lights.size());

			// pro kazde svetlo urcime zda je face viditelny
			for (unsigned int lightI = 0; lightI < lights.size(); lightI++)
			{
				glm::vec3 lightPosition = lights[lightI];
				visibilities[lightI] = ( (a * lightPosition.x + b * lightPosition.y + c * lightPosition.z + d) > 0 );					
			}

			faceVisibilities[faceIA] = visibilities;


			// --------- Vypocet sousednosti ---------------------------------

			// hrany face definovane vzdy dvema indexy vrcholu
			glm::ivec2 edges[3] = {
				glm::ivec2(face.x, face.y),
				glm::ivec2(face.y, face.z),
				glm::ivec2(face.z, face.x)
			};

			// porovnat kazdou hranu
			for (unsigned int edgeI = 0; edgeI < 3; edgeI++)
			{
				glm::ivec2 e = edges[edgeI];

				int *destEdge = NULL;
				switch (edgeI) {
					case 0: destEdge = &facesNeighbours[mesh][faceIA].a; break;
					case 1: destEdge = &facesNeighbours[mesh][faceIA].b; break;
					case 2: destEdge = &facesNeighbours[mesh][faceIA].c; break;
				}

				if (*destEdge != -1)
					continue;

				for (unsigned int faceIB = 0; faceIB < faces.size(); faceIB++)				
				{
					glm::ivec3 faceB = faces[faceIB];

					// neporovnavat sam se sebou
					if (faceIA == faceIB)
						continue;

					if ((e.x == faceB.x && e.y == faceB.y) || (e.x == faceB.y && e.y == faceB.x)) {
						*destEdge = faceIB;
						facesNeighbours[mesh][faceIB].a = faceIA;
						continue;
					}

					if ((e.x == faceB.y && e.y == faceB.z) || (e.x == faceB.z && e.y == faceB.y)) {
						*destEdge = faceIB;
						facesNeighbours[mesh][faceIB].b = faceIA;
						continue;
					}

					if ((e.x == faceB.z && e.y == faceB.x) || (e.x == faceB.x && e.y == faceB.z)) {
						*destEdge = faceIB;
						facesNeighbours[mesh][faceIB].c = faceIA;
						continue;
					}					
				}
			}
		}

		// ulozit viditelnosti
		meshFacesVisibilities[meshI] = faceVisibilities;
	}	
}





void ShadowVolumes::draw(unsigned int lightI, glm::mat4 mView, glm::mat4 mProjection)
{
	ShaderManager::PROGRAMBINDING activeBinding = ShaderManager::useProgram(PROGRAM_NAME);
	
	// transformacni matice
	glm::mat4 mModel = glm::mat4(1.0f);
	glUniformMatrix4fv(activeBinding.mModelUniform, 1, GL_FALSE, glm::value_ptr(mModel));
	glUniformMatrix4fv(activeBinding.mViewUniform, 1, GL_FALSE, glm::value_ptr(mView));
	glUniformMatrix4fv(activeBinding.mProjectionUniform, 1, GL_FALSE, glm::value_ptr(mProjection));	

	// zdroje dat
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[lightI]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[lightI]);
		
	glEnableVertexAttribArray(activeBinding.positionAttrib);
	glVertexAttribPointer(activeBinding.positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		
/*
	glDisable(GL_BLEND); // We don't want lighting. We are only writing in stencil buffer for now
    glClear(GL_STENCIL_BUFFER_BIT); // We clear the stencil buffer
    glDepthFunc(GL_LESS); // We change the z-testing function to LESS, to avoid little bugs in shadow
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // We dont draw it to the screen
    glStencilFunc(GL_ALWAYS, 0, 0); // We always draw whatever we have in the stencil buffer
		
	glCullFace(GL_FRONT); // We are drawing the back faces first 
    glStencilOp(GL_KEEP, GL_INCR, GL_KEEP); // We increment if the depth test fails

	glDrawElements(GL_QUADS, shadowVolumeIndices, GL_UNSIGNED_INT, NULL);

    glCullFace(GL_BACK); // We are now drawing the front faces
    glStencilOp(GL_KEEP, GL_DECR, GL_KEEP); // We decrement if the depth test fails
*/
    glDrawElements(GL_QUADS, shadowVolumeIndices, GL_UNSIGNED_INT, NULL);
	

	/*
	// We draw our lighting now that we created the shadows area in the stencil buffer
    glDepthFunc(GL_LEQUAL); // we put it again to LESS or EQUAL (or else you will get some z-fighting)
    glCullFace(GL_BACK); // we draw the front face
    glEnable(GL_BLEND); // We enable blending
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // We enable color buffer
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Drawing will not affect the stencil buffer
    glStencilFunc(GL_EQUAL, 0x0, 0xff); // And the most important thing, the stencil function. Drawing if equal to 0
	*/
	
}