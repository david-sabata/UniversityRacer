

#include "PhysicsDebugDraw.h"


PhysicsDebugDraw::PhysicsDebugDraw(void): m_debugMode(1)
{

}

PhysicsDebugDraw::~PhysicsDebugDraw(void)
{

}

void PhysicsDebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
    glm::vec3 src(from.x(), from.y(), from.z());
    glm::vec3 dst(to.x(), to.y(), to.z());
    glm::vec3 col(color.x(), color.y(), color.z());
    
    LINE l(src, dst, col);    
    lines.push_back(l);
}


/*void PhysicsDebugDraw::DrawCylinder(GLfloat *m, GLfloat radius, GLfloat height)
{
    static GLUquadricObj *quadricObj = 0;

    if (quadricObj == 0)
    {
        quadricObj = gluNewQuadric();
        gluQuadricDrawStyle(quadricObj, GLU_LINE);
    }

    if (m)
    {
        glPushMatrix(); 
        glMultMatrixf(m);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, -height/2.f);
        
    }        
    
    glColor3f(1.f, 0.f, 1.f);
    gluCylinder(quadricObj, radius, radius, height, 10, 1);

    if (m)
        glPopMatrix();
}*/