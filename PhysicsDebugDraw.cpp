

#include "PhysicsDebugDraw.h"




PhysicsDebugDraw::PhysicsDebugDraw(void): m_debugMode(1)
{

}

PhysicsDebugDraw::~PhysicsDebugDraw(void)
{

}

void PhysicsDebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
    glPushMatrix();
    glColor3f(color.x(), color.y(), color.z());
    glBegin(GL_LINES);
    glVertex3f(from.x(), from.y(), from.z());
    glVertex3f(to.x(), to.y(), to.z());
    glEnd();
    glPopMatrix();
}

void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
    btVector3 to = PointOnB + normalOnB * distance;
    const btVector3 & from = PointOnB;
    glColor3f(color.getX(),color.getY(),color.getZ());
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex3d(from.getX(),from.getY(),from.getZ());
    glVertex3d(to.getX(), to.getY(), to.getZ());
    glEnd();
}

void PhysicsDebugDraw::drawTriangle(const btVector3 & a, const btVector3 & b, const btVector3 & c, const btVector3 & color, btScalar alpha)
{
    const btVector3 n = btCross( b-a, c-a ).normalized();
    glBegin(GL_TRIANGLES);        
    glColor4f(color.getX(), color.getY(), color.getZ(), alpha);
    glNormal3d(n.getX(),n.getY(),n.getZ());
    glVertex3d(a.getX(),a.getY(),a.getZ());
    glVertex3d(b.getX(),b.getY(),b.getZ());
    glVertex3d(c.getX(),c.getY(),c.getZ());
    glEnd();
}

void PhysicsDebugDraw::DrawCylinder(GLfloat *m, GLfloat radius, GLfloat height)
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
}

PhysicsDebugDraw::ShapeCache* PhysicsDebugDraw::cache(btConvexShape* shape)
{
    ShapeCache* sc=(ShapeCache*)shape->getUserPointer();
    if(!sc)
    {
        sc=new(btAlignedAlloc(sizeof(ShapeCache),16)) ShapeCache(shape);
        sc->m_shapehull.buildHull(shape->getMargin());
        m_shapecaches.push_back(sc);
        shape->setUserPointer(sc);
        /* Build edges    */ 
        const int            ni=sc->m_shapehull.numIndices();
        const int            nv=sc->m_shapehull.numVertices();
        const unsigned int*  pi=sc->m_shapehull.getIndexPointer();
        const btVector3*     pv=sc->m_shapehull.getVertexPointer();
        btAlignedObjectArray<ShapeCache::Edge*>    edges;
        sc->m_edges.reserve(ni);
        edges.resize(nv*nv,0);
        for(int i=0;i<ni;i+=3)
        {
            const unsigned int* ti=pi+i;
            const btVector3 nrm=btCross(pv[ti[1]]-pv[ti[0]],pv[ti[2]]-pv[ti[0]]).normalized();
            for(int j=2,k=0;k<3;j=k++)
            {
                const unsigned int a=ti[j];
                const unsigned int b=ti[k];
                ShapeCache::Edge*& e=edges[btMin(a,b)*nv+btMax(a,b)];
                if(!e)
                {
                    sc->m_edges.push_back(ShapeCache::Edge());
                    e=&sc->m_edges[sc->m_edges.size()-1];
                    e->n[0]=nrm;e->n[1]=-nrm;
                    e->v[0]=a;e->v[1]=b;
                }
                else
                {
                    e->n[1]=nrm;
                }
            }
        }
    }
    return(sc);
}
void PhysicsDebugDraw::DrawConvexShape(GLfloat *m, btCollisionShape* shape)
{
    if (!shape->isConvex())
        return;
    
    if (m)
    {
        glPushMatrix(); 
        glMultMatrixf(m);
    } 

    ShapeCache*    sc = cache((btConvexShape*)shape);
    btShapeHull* hull = &sc->m_shapehull;

    glColor3f(0.f, 0.f, 1.f);

    if (hull->numTriangles () > 0)
    {
        int index = 0;
        const unsigned int* idx = hull->getIndexPointer();
        const btVector3* vtx = hull->getVertexPointer();

        glBegin (GL_TRIANGLES);

        for (int i = 0; i < hull->numTriangles (); i++)
        {
            int i1 = index++;
            int i2 = index++;
            int i3 = index++;

            int index1 = idx[i1];
            int index2 = idx[i2];
            int index3 = idx[i3];

            btVector3 v1 = vtx[index1];
            btVector3 v2 = vtx[index2];
            btVector3 v3 = vtx[index3];

            btVector3 normal = ((v3-v1).cross(v2-v1)).normalize();
            glNormal3f(normal.getX(),normal.getY(),normal.getZ());
            
            glVertex3f (v1.x(), v1.y(), v1.z());
            glVertex3f (v2.x(), v2.y(), v2.z());
            glVertex3f (v3.x(), v3.y(), v3.z());

        }
        glEnd();
    }

    if (m)
        glPopMatrix();
}
