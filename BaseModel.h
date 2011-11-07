#ifndef BASEMODEL_H
#define BASEMODEL_H


#include <vector>
#include "Mesh.h"


class BaseModel
{

	public:
		virtual ~BaseModel();

		/**
		 * Prida do modelu novou mesh
		 */
		void addMesh(Mesh*);

		/**
		 * Vrati pole vsech meshi v modelu
		 */
		std::vector<Mesh*> &getMeshes();

		/**
		 * Vraci pocet vrcholu v modelu (soucet vsech meshi)
		 */
		unsigned int vertexCount();

		/**
		 * Vraci pocet facu modelu (soucet vsech meshi)
		 */
		unsigned int facesCount();


	protected:
		std::vector<Mesh*> meshes;

};

#endif

