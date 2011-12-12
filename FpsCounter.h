#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include <ostream>
#include <iomanip>

#include "GameTime.h"

/**
 * Pocita FPS a udrzuje nejlepsi, nejhorsi a prumerne hodnoty.
 * Inspirovano podle knihovny OGRE (http://www.ogre3d.org/)
 * Konkretne tridou RenderTarget (http://www.ogre3d.org/docs/api/html/classOgre_1_1RenderTarget.html)
 */
class FpsCounter
{
public:
    FpsCounter(): m_current(0.0f), m_average(0.0f), m_best(0.0f), m_worst(9999.0f),
                  m_totalFrames(0), m_frameCount(0), m_totalElapsedTime(0) {}
    
    void Update(const GameTime & gameTime)
    {
        m_totalElapsedTime += gameTime.Elapsed();
        m_frameCount++;

        if (m_totalElapsedTime > 1000)
        {
            m_current = static_cast<float>(m_frameCount) / static_cast<float>(m_totalElapsedTime.ms()) * 1000.0f;            
            
            if (m_current > m_best ) m_best  = m_current;
            if (m_current < m_worst) m_worst = m_current;
        
            if (m_average == 0.0f)
                m_average = m_current;
            else
                m_average = (m_average + m_current) / 2.0f;
            
            m_frameCount = 0;
            m_totalElapsedTime = 0;       
        }
   
        m_totalFrames++;
    }
    
    float Current() const { return m_current; }
    float Average() const { return m_average; }
    float Best()    const { return m_best; }
    float Worst()   const { return m_worst; }    
    unsigned int TotalFrames() const { return m_totalFrames; }      

    friend std::ostream& operator << (std::ostream& o, const FpsCounter & v)
    {
        o << "current: " << std::setw(7) << std::fixed << std::setprecision(4) << v.Current() << ", avg: " <<
            std::setw(7) << v.Average() << ", best: " << std::setw(7) << v.Best() << ", worst: " << std::setw(7) << v.Worst();
        return o;
    }

private:
    float m_current, m_average, m_best, m_worst;
    unsigned int m_frameCount, m_totalFrames;
    TimeSpan m_totalElapsedTime;
};

#endif
