#ifndef PHYSICSDEBUGDRAW_H
#define PHYSICSDEBUGDRAW_H

#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

#include <btBulletDynamicsCommon.h>

/**
 * Pomocna trida pro debug vykresleni koliznich teles pouzitych v simulaci; kresleni pouze pomoci car.
 */
class PhysicsDebugDraw : public btIDebugDraw
{
public:
    struct LINE {
    	LINE(glm::vec3 a, glm::vec3 b, glm::vec3 color) : a(a), b(b), color(color) {};
    
    	glm::vec3 a;
    	glm::vec3 b;
    	glm::vec3 color;
    };

    PhysicsDebugDraw(void): m_debugMode(1) {}
    ~PhysicsDebugDraw(void) {}

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}
    virtual void drawTriangle(const btVector3 & a, const btVector3 & b, const btVector3 & c, const btVector3 & color, btScalar alpha) {}

    void reportErrorWarning(const char * warningString) { std::cout << "Physics debugger warning: " << warningString << std::endl; }
    
    virtual void draw3dText(const btVector3& location,const char* textString) {}
    virtual void setDebugMode(int debugMode) { m_debugMode = debugMode; }    
    virtual int getDebugMode() const { return m_debugMode; }

    std::vector<LINE> & GetLines() { return lines; }

private:
    // pole pomocnych car k vykresleni - caru definuji dva body a barva
    std::vector<LINE> lines;
    
    int m_debugMode;
};

#endif
