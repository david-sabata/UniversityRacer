#ifndef MODELCONTAINER_H
#define MODELCONTAINER_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>

#include "Debug.h"
#include "BaseModel.h"
#include "3ds/3ds.h"
#include "ShaderManager.h"
#include "Light.h"

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
		 * Polozka vnejsi kreslici fronty (pracuje s ni Game) pracujici nad modely
		 */
		typedef struct {
			BaseModel* model;
			glm::mat4 matrix;
		} DRAWINGQUEUEITEM;


		/**
		 * Polozka vnitrni kreslici fronty pracujici nad Mesh
		 */
		typedef struct {
			Mesh* mesh;
			glm::mat4 matrix;
		} MESHDRAWINGQUEUEITEM;


		/**
		 * Prida model do vykreslovaci fronty a vraci index pridaneho zaznamu. Tato fronta se v kazdem
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
		unsigned int queueDraw(std::string modelName);
		unsigned int queueDraw(std::string modelName, glm::mat4 mat);
		unsigned int queueDraw(BaseModel* model);
		unsigned int queueDraw(BaseModel* model, glm::mat4 mat);

		/**
		 * Upravi matici polozky v kreslici fronte; index polozky lze ziskat napr. z volani queueDraw
		 */
		void updateDrawingMatrix(unsigned int drawingQueueIndex, glm::mat4 matrix);


		/**
		 * Vraci asociativni pole modelu v kontejneru
		 */
		//std::map<std::string, BaseModel*> const &getModels();
		std::vector<BaseModel*> const &getModels();






		/**
		 * Prida do kontejneru svetlo
		 * @param positioin pozice svetla
		 * @param color barva svetla
		 */
		void addLight(Light light);

		/**
		 * Vraci pole vsech svetel v kontejneru
		 */
		std::vector<Light> &getLights();





		/**
		 * Zoptimalizuje kreslici frontu - je nutne volat pred inicializaci Scene !!!
		 */
		void optimizeDrawingQueue();

		
		/**
		 * Vraci referenci na kreslici frontu
		 * Pro pridavani modelu do fronty slouzi metoda queueDraw().
		 */
		std::vector<DRAWINGQUEUEITEM> const &getDrawingQueue();


		/**
		 * Vraci referenci na vnitrni kreslici frontu nad meshi
		 */
		std::vector<ModelContainer::MESHDRAWINGQUEUEITEM> const &getMeshDrawingQueue();


		/**
		 * Vraci offset zacatku indexu modelu v ramci indexoveho VBO
		 * Pouziva se k vykresleni konkretniho modelu
		 * @param ukazatel na model, ziskany napr. z volani metody addModel
		 */
		unsigned int getModelIndexOffset(BaseModel* model);

		/**
		 * Vraci offset prvniho indexu meshe v ramci indexoveho VBO
		 * Pouziva se ke kresleni konkretni meshe
		 * @param ukazatel na mesh
		 */
		unsigned int getMeshIndexOffset(Mesh* mesh);


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
		 * Pole svetel v tomto kontejneru
		 */
		std::vector<Light> lights;

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
		 * Offset prvniho indexu meshe v ramci kontejneru,
		 * podobne jako modelsIndexOffsets
		 */
		std::map<Mesh*, unsigned int> meshesIndexOffsets;

		/**
		 * Celkovy pocet facu vsech modelu v kontejneru.
		 * Pouziva se pro urceni offsetu indexu pridavaneho modelu
		 */
		unsigned int totalFacesCount;

		/**
		 * Fronta modelu k vykresleni (ktera je ve skutecnosti vektor)
		 *
		 * Tato fronta slouzi pro vnejsi manipulaci a ovladani kresleni,
		 * ve skutecnosti se ale k samotnemu kresleni vyuziva fronta
		 * meshDrawingQueue, kde jsou obsazeny primo meshe. Mapovani mezi
		 * obema frontami zajistuje drawingQueuesMapping
		 */
		std::vector<DRAWINGQUEUEITEM> drawingQueue;

		/**
		 * Udrzuje mapovani mezi modely a jejich meshi, ktere muzou byt nasledkem
		 * optimalizace nahodne rozhazene po kreslici fronte
		 */
		std::map<unsigned int, std::vector<unsigned int>> drawingQueuesMapping;

		/**
		 * Kreslici fronta obsahujici jednotlive meshe a jejich matice
		 * (refelektujici matice modelu v modelove kreslici fronte)
		 */
		std::vector<MESHDRAWINGQUEUEITEM> meshDrawingQueue;
};

#endif
