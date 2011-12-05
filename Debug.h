#ifndef DEBUG_H
#define DEBUG_H

// definice pro detekovani leaku

#ifdef _DEBUG
    #define MYDEBUG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)    
#else
    #define MYDEBUG_NEW
#endif // _DEBUG


#endif