#ifndef BASEMODEL_H
#define BASEMODEL_H


#include <vector>
#include "Debug.h"
#include "Mesh.h"


class BaseModel
{

	public:
		virtual ~BaseModel();

		/**
		 * Nastavi meshe modelu
		 */
		void setMeshes(std::vector<Mesh*> meshes);

		/**
		 * Vrati pole vsech meshi v modelu
		 */
		std::vector<Mesh*> &getMeshes();

		/**
		 * Vraci pocet vrcholu v modelu (soucet vsech meshi)
		 */
		inline unsigned int verticesCount() { return vertexCount; }

		/**
		 * Vraci pocet facu modelu (soucet vsech meshi)
		 */
		inline unsigned int facesCount() { return faceCount; }


	protected:
		std::vector<Mesh*> meshes;
		
		unsigned int vertexCount;
		unsigned int faceCount;
};

#endif

