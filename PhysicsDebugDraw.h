
#pragma once

#ifdef _WIN32
    #define NOMINMAX    
    #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>
#include <iostream>

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionShapes/btConvexPolyhedron.h>

class PhysicsDebugDraw : public btIDebugDraw
{
protected:
    struct ShapeCache {
        struct Edge {
            btVector3 n[2];
            int v[2];
        };

        btShapeHull m_shapehull;
        btAlignedObjectArray<Edge> m_edges;    
        ShapeCache(btConvexShape* s) : m_shapehull(s) {}    
    };
    btAlignedObjectArray<ShapeCache*>    m_shapecaches; //clean-up memory of dynamically created shape hulls

public:
    PhysicsDebugDraw(void);
    ~PhysicsDebugDraw(void);

    virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
    virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    virtual void drawTriangle(const btVector3 & a, const btVector3 & b, const btVector3 & c, const btVector3 & color, btScalar alpha);

    void reportErrorWarning(const char * warningString)
    {
        std::cout << "Physics debugger warning: " << warningString << std::endl;
    }
    
    virtual void draw3dText(const btVector3& location,const char* textString) {}
    virtual void setDebugMode(int debugMode) { m_debugMode = debugMode; }    
    virtual int getDebugMode() const { return m_debugMode; }

    static void DrawCylinder(GLfloat *m, GLfloat radius, GLfloat height);
    
    void DrawConvexShape(GLfloat *m, btCollisionShape* shape);
    ShapeCache*    cache(btConvexShape* shape);

private:
    int m_debugMode;
};

