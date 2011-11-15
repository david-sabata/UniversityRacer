#ifndef MODELCONTAINER_H
#define MODELCONTAINER_H

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "BaseModel.h"
#include "3ds/3ds.h"

class ModelContainer
{

	public:
		ModelContainer(void);
		~ModelContainer(void);

		/**
		 * Prida do kontejneru model
		 * Pouziva se pro predani dat. Pokud chceme model
		 * vykreslovat vicekrat, staci ho pridat pouze jednou
		 * a pote jej nekolikrat pridat do kreslici fronty kontejneru.
		 * @param name unikatni jmeno modelu
		 * @param model ukazatel na pridavany model
		 */
		void addModel(std::string name, BaseModel *model);


		/**
		 * Prida model do vykreslovaci fronty. Tato fronta se v kazdem
		 * snimku postupne prochazi a modely se s prislusnymi maticemi vykresluji.
		 * Model je mozne pridat podle jeho jmena anebo ukazatele, ale vzdy
		 * musi byt nejdrive 'fyzicky pridan' do kontejneru (metoda addModel).
		 *
		 * Vicenasobnym pridanim modelu do kreslici fronty s ruznymi maticemi
		 * se model rozkopiruje do sceny, pricemz jeho data jsou ulozeny pouze
		 * jednou.
		 *
		 * Modelovou matici je mozne vynechat; v takovem pripade se pouzije identita.
		 */
		void queueDraw(std::string modelName);
		void queueDraw(std::string modelName, glm::mat4 mat);
		void queueDraw(BaseModel* model);
		void queueDraw(BaseModel* model, glm::mat4 mat);


		/**
		 * Vraci asociativni pole modelu v kontejneru
		 */
		//std::map<std::string, BaseModel*> const &getModels();
		std::vector<BaseModel*> const &getModels();



		/**
		 * Polozka kreslici fronty
		 */
		typedef struct {
			BaseModel* model;
			glm::mat4 matrix;
		} DRAWINGQUEUEITEM;

		/**
		 * Vraci referenci na kreslici frontu
		 * Pro pridavani modelu do fronty slouzi metoda queueDraw().
		 */
		std::vector<DRAWINGQUEUEITEM> const &getDrawingQueue();



		/**
		 * Vraci offset zacatku indexu modelu v ramci indexoveho VBO
		 * Pouziva se k vykresleni konkretniho modelu
		 * @param ukazatel na model, ziskany napr. z volani metody addModel
		 */
		unsigned int getModelIndexOffset(BaseModel* model);


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
		 * Nacte scenu z 3DS souboru a vrati ukazatel na nacteny model
		 * @param filename cesta k .3ds souboru
		 */
		BaseModel* load3DS(std::string filename);



	protected:
		/**
		 * Asociativni pole modelu podle jejich nazvu (pro moznost dohledani 
		 * i po ztraceni ukazatele, tj. mimo init); hodnota je index do vektoru models
		 */
		std::map<std::string, unsigned int> modelNames;


		/**
		 * Pole modelu, usporadane podle pridani
		 */
		std::vector<BaseModel*> models;


		/**
		 * Offset prvniho indexu (pocet predchazejicich uint hodnot
		 * v indexovem VBO) modelu v ramci kontejneru.
		 * Ten je nutne znat pokud chceme vykreslit napr. jeden
		 * konkretni model. Index by bylo mozne ziskat i z modelu a 
		 * jeho pozice v ModelContainer::models, ale jelikoz je potreba
		 * pri kresleni, slouzi tato mapa jako jakasi cache.
		 */
		std::map<BaseModel*, unsigned int> modelsIndexOffsets;

		/**
		 * Celkovy pocet facu vsech modelu v kontejneru.
		 * Pouziva se pro urceni offsetu indexu pridavaneho modelu
		 */
		unsigned int totalFacesCount;

		/**
		 * Fronta modelu k vykresleni (ktera je ve skutecnosti vektor)
		 */
		std::vector<DRAWINGQUEUEITEM> drawingQueue;
};

#endif
