#include "ShadowVolumes.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

// vypnout otravne warningy Visual Studia; nasim iteratorum verime :)
#define _SCL_SECURE_NO_WARNINGS

// glm neumi porovnat dva vektory
#define VEC3_EQ(A,B) ((A.x == B.x && A.y == B.y && A.z == B.z))

#define PROGRAM_NAME "test"
#define INFINITY 1
#define EPSILON 0.0005f


// pomocne definy pro testovani
#define DRAW_VISIBLE_FACES 0
#define DRAW_VOLUME 1
#define DRAW_VOLUME_LINES 0
#define DRAW_CAPS 1

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

			if (mesh->getName() != "deskastolu")
				continue;

			Mesh* newMesh = new Mesh((*mesh) * (*modelIt).second);
			meshes.push_back(newMesh);
		}
	}

	// vypocitat sousednosti jiz nad novymi meshi
	computeNeighboursAndVisibilities();

	VBOs = vector<GLuint>(lights.size());
	EBOs = vector<GLuint>(lights.size());
	capVBOs = vector<GLuint>(lights.size());
	capEBOs = vector<GLuint>(lights.size());

	// vyrobit GL buffery
	for (unsigned int lightI = 0; lightI < lights.size(); lightI++)
	{
		glm::vec3 light = lights[lightI];
		
		// steny teles - triangle strip
		vector<glm::vec3> glVertices;
		vector<unsigned int> glIndices;

		// vika teles - triangle
		vector<glm::vec3> glCapVertices;
		vector<unsigned int> glCapIndices;


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

#if DRAW_VISIBLE_FACES
					unsigned int x = glVertices.size();

					glVertices.push_back(vertices[face.x]);
					glVertices.push_back(vertices[face.y]);
					glVertices.push_back(vertices[face.z]);

					glIndices.push_back(x);
					glIndices.push_back(x+1);
					glIndices.push_back(x+2);
#endif

#if DRAW_VOLUME
					// steny telesa
					for (unsigned int edgeI = 0; edgeI < 3; edgeI++)
					{		
						int neighbourIndex = edges[edgeI].second;

						// aktualni hrana je stinova, pokud nema souseda anebo je soused odvraceny od svetla
						if (neighbourIndex == -1 || meshFacesVisibilities[meshI][neighbourIndex][lightI] == false)
						{
							glm::vec3 v1 = vertices[ edges[edgeI].first.x ];
							glm::vec3 v2 = vertices[ edges[edgeI].first.y ];

							glm::vec3 v3, v4;

							v3 = v1 + (( v1 - light ) * glm::vec3(INFINITY, INFINITY, INFINITY));
							v4 = v2 + (( v2 - light ) * glm::vec3(INFINITY, INFINITY, INFINITY));

							unsigned int index = glVertices.size();

#if DRAW_VOLUME_LINES
							glVertices.push_back(v1);
							glVertices.push_back(v2);

							glIndices.push_back(index);
							glIndices.push_back(index + 1);

#else
							// kreslit se budou 4 vrcholy jako ctverec
							glVertices.push_back(v2);
							glVertices.push_back(v1);
							glVertices.push_back(v3);
							glVertices.push_back(v4);

							glIndices.push_back(index);
							glIndices.push_back(index + 1);
							glIndices.push_back(index + 2);
							glIndices.push_back(index + 3);
#endif
						}
					}
#endif
				} // jestlize je face viditelny od svetla
				
				// z facu odvracenych od svetla vyrobime horni a dolni viko
				else {
					// horni viko
					unsigned int capIndex = glCapVertices.size();
					glCapVertices.push_back( vertices[face.z] + ( glm::normalize(vertices[face.z] - light) * glm::vec3(EPSILON, EPSILON, EPSILON) ));
					glCapVertices.push_back( vertices[face.y] + ( glm::normalize(vertices[face.y] - light) * glm::vec3(EPSILON, EPSILON, EPSILON) ));
					glCapVertices.push_back( vertices[face.x] + ( glm::normalize(vertices[face.x] - light) * glm::vec3(EPSILON, EPSILON, EPSILON) ));					
					
					glCapIndices.push_back(capIndex);
					glCapIndices.push_back(capIndex + 1);
					glCapIndices.push_back(capIndex + 2);
					
					// dolni viko - face protazeny od svetla do nekonecna
					capIndex = glCapVertices.size();
					glCapVertices.push_back( vertices[face.x] + ( (vertices[face.x] - light) * glm::vec3(INFINITY, INFINITY, INFINITY) ) );
					glCapVertices.push_back( vertices[face.y] + ( (vertices[face.y] - light) * glm::vec3(INFINITY, INFINITY, INFINITY) ) );
					glCapVertices.push_back( vertices[face.z] + ( (vertices[face.z] - light) * glm::vec3(INFINITY, INFINITY, INFINITY) ) );

					glCapIndices.push_back(capIndex);
					glCapIndices.push_back(capIndex + 1);
					glCapIndices.push_back(capIndex + 2);
				}

			}
		}

		// zkopirovat data sten do VBO
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



		// zkopirovat data vik do VBO
		glGenBuffers(1, &capVBOs[lightI]);
		glBindBuffer(GL_ARRAY_BUFFER, capVBOs[lightI]);
		glBufferData(GL_ARRAY_BUFFER, glCapVertices.size() * 3 * sizeof(float), NULL, GL_STATIC_DRAW);
		
		float* mappingCapVBO = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		
		for (vector<glm::vec3>::iterator it = glCapVertices.begin(); it != glCapVertices.end(); it++)
		{
			*(mappingCapVBO + 0) = (*it).x;
			*(mappingCapVBO + 1) = (*it).y;
			*(mappingCapVBO + 2) = (*it).z;
			
			mappingCapVBO += 3;
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);



		// zkopirovat data sten do EBO
		glGenBuffers(1, &EBOs[lightI]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[lightI]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndices.size() * sizeof(unsigned int), NULL, GL_STATIC_DRAW);
		
		unsigned int* mappingEBO = (unsigned int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
		
		if (glIndices.size() > 0)
			copy(glIndices.begin(), glIndices.end(), mappingEBO);

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



		// zkopirovat data vik do EBO
		glGenBuffers(1, &capEBOs[lightI]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, capEBOs[lightI]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, glCapIndices.size() * sizeof(unsigned int), NULL, GL_STATIC_DRAW);
		
		unsigned int* mappingCapEBO = (unsigned int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
		
		if (glCapIndices.size() > 0)
			copy(glCapIndices.begin(), glCapIndices.end(), mappingCapEBO);

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



		// zapamatovat si kolik se toho bude kreslit
		shadowVolumeIndices = glIndices.size();
		shadowVolumeCapsIndices = glCapIndices.size();
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
			glm::vec3 vA1 = vertices[ face.x ];
			glm::vec3 vA2 = vertices[ face.y ];
			glm::vec3 vA3 = vertices[ face.z ];

			// vypocitat koeficienty rovnice plochy 
			float a = vA1.y*(vA2.z-vA3.z) + vA2.y*(vA3.z-vA1.z) + vA3.y*(vA1.z-vA2.z);
			float b = vA1.z*(vA2.x-vA3.x) + vA2.z*(vA3.x-vA1.x) + vA3.z*(vA1.x-vA2.x);
			float c = vA1.x*(vA2.y-vA3.y) + vA2.x*(vA3.y-vA1.y) + vA3.x*(vA1.y-vA2.y);
			float d = -( vA1.x*( vA2.y*vA3.z - vA3.y*vA2.z ) + vA2.x*(vA3.y*vA1.z - vA1.y*vA3.z) + vA3.x*(vA1.y*vA2.z - vA2.y*vA1.z) );

			vector<bool> visibilities(lights.size());

			// pro kazde svetlo urcime zda je face viditelny
			for (unsigned int lightI = 0; lightI < lights.size(); lightI++)
			{
				glm::vec3 lightPosition = lights[lightI];
				visibilities[lightI] = ( (a * lightPosition.x + b * lightPosition.y + c * lightPosition.z + d) > 0 );					
				/*
				glm::vec3 normal = glm::cross(
					vA2 - vA1,
					vA3 - vA1
				);	
				
				visibilities[lightI] = glm::dot(lightPosition, normal) < 0;
				*/
			}

			faceVisibilities[faceIA] = visibilities;


			// --------- Vypocet sousednosti ---------------------------------

			// hrany face definovane vzdy dvema indexy vrcholu a ukazatelem, kam ulozime index nalezeneho souseda
			struct Edge {
				Edge(glm::vec3 A, glm::vec3 B, int* nbrIdx) : pointA(A), pointB(B), neighbourIndex(nbrIdx) {};
				glm::vec3 pointA;
				glm::vec3 pointB;
				int* neighbourIndex;
			};

			Edge edges[3] = {
				Edge(vA1, vA2, &facesNeighbours[mesh][faceIA].a),
				Edge(vA2, vA3, &facesNeighbours[mesh][faceIA].b),
				Edge(vA3, vA1, &facesNeighbours[mesh][faceIA].c),
			};

			// porovnat kazdou hranu
			for (unsigned int edgeI = 0; edgeI < 3; edgeI++)
			{
				Edge e = edges[edgeI];				

				if (*e.neighbourIndex != -1) // pokud uz nejakeho souseda ma, neresime
					continue;

				for (unsigned int faceIB = 0; faceIB < faces.size(); faceIB++)				
				{
					glm::ivec3 faceB = faces[faceIB];

					// neporovnavat sam se sebou
					if (faceIA == faceIB)
						continue;

					glm::vec3 vB1 = vertices[ faceB.x ];
					glm::vec3 vB2 = vertices[ faceB.y ];
					glm::vec3 vB3 = vertices[ faceB.z ];
					
					if ((VEC3_EQ(e.pointA, vB1) && VEC3_EQ(e.pointB, vB2)) || (VEC3_EQ(e.pointA, vB2) && VEC3_EQ(e.pointB, vB1))) {
						*e.neighbourIndex = faceIB;
						facesNeighbours[mesh][faceIB].a = faceIA;
						break;
					}

					if ((VEC3_EQ(e.pointA, vB2) && VEC3_EQ(e.pointB, vB3)) || (VEC3_EQ(e.pointA, vB3) && VEC3_EQ(e.pointB, vB2))) {
						*e.neighbourIndex = faceIB;
						facesNeighbours[mesh][faceIB].b = faceIA;
						break;
					}

					if ((VEC3_EQ(e.pointA, vB3) && VEC3_EQ(e.pointB, vB1)) || (VEC3_EQ(e.pointA, vB1) && VEC3_EQ(e.pointB, vB3))) {
						*e.neighbourIndex = faceIB;
						facesNeighbours[mesh][faceIB].c = faceIA;
						break;
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
	glm::mat4 mModelView = mView * mModel;
	glm::mat4 mMVP = mProjection * mModelView;
	glUniformMatrix4fv(activeBinding.mViewUniform, 1, GL_FALSE, glm::value_ptr(mView));
	glUniformMatrix4fv(activeBinding.mModelViewUniform, 1, GL_FALSE, glm::value_ptr(mModelView));	
	glUniformMatrix4fv(activeBinding.mModelViewProjectionUniform, 1, GL_FALSE, glm::value_ptr(mMVP));

	// vykreslit steny ------------------------------------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[lightI]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[lightI]);
		
	glEnableVertexAttribArray(activeBinding.positionAttrib);
	glVertexAttribPointer(activeBinding.positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	glLineWidth(3);

#if DRAW_VISIBLE_FACES
	glDrawElements(GL_TRIANGLES, shadowVolumeIndices, GL_UNSIGNED_INT, NULL);
#endif

#if DRAW_VOLUME
#if DRAW_VOLUME_LINES
	glDrawElements(GL_LINES, shadowVolumeIndices, GL_UNSIGNED_INT, NULL);
#else
	glDrawElements(GL_QUADS, shadowVolumeIndices, GL_UNSIGNED_INT, NULL);
#endif
#endif

	glLineWidth(1);
	
	// vykreslit vika -------------------------------------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, capVBOs[lightI]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, capEBOs[lightI]);
		
	glEnableVertexAttribArray(activeBinding.positionAttrib);
	glVertexAttribPointer(activeBinding.positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);

#if DRAW_CAPS
	glDrawElements(GL_TRIANGLES, shadowVolumeCapsIndices, GL_UNSIGNED_INT, NULL);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}