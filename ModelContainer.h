#ifndef MODELCONTAINER_H
#define MODELCONTAINER_H

#include <string>
#include <iostream>
#include <map>

#include "BaseModel.h"
#include "3ds\3ds.h"

class ModelContainer
{

	public:
		ModelContainer(void);
		~ModelContainer(void);

		/**
		 * Prida do kontejneru model
		 * @param name unikatni jmeno modelu
		 * @param model ukazatel na pridavany model
		 */
		void addModel(std::string name, BaseModel *model);

		/**
		 * Vraci asociativni pole modelu v kontejneru
		 */
		std::map<std::string, BaseModel*> const &getModels();

		/**
		 * Vraci pocet modelu v kontejneru
		 */
		unsigned int modelsCount();

		/**
		 * Vraci pocet vsech vrcholu v kontejneru
		 */
		unsigned int verticesCount();

		/**
		 * Vraci pocet vsech facu v kontejneru
		 */
		unsigned int facesCount();

		/**
		 * Nacte scenu (jeden nebo vice modelu) z 3DS souboru a nove
		 * modely vlozi do kontejneru. Nazvy modelu budou prefixovany 
		 * nazvem souboru bez pripony a podtrzitkem, napr. car_wheel
		 * @param filename cesta k .3ds souboru
		 */
		void load3DS(std::string filename);

	protected:

		std::map<std::string, BaseModel*> models;
};

#endif
