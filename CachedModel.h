
#include <string>
#include <iostream>
#include <fstream>

#include "BaseModel.h"
#include "Mesh.h"



class CachedModel : public BaseModel
{
	public:
		CachedModel(BaseModel* model, std::string path);
		
		/**
		 * Nacte kesovana data ze zadane cesty; pokud soubor
		 * neexistuje, vraci NULL
		 */
		static CachedModel* load(std::string path);



	private:
		/**
		 * Privatni konstruktor pro vyrobeni objektu z nactenych dat
		 */
		CachedModel(std::vector<Mesh*> loadedMeshes);

};

