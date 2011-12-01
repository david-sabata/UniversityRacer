#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#include <SDL/SDL.h>
#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

#include "Debug.h"
#include "Exceptions.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

/*void updateFPS(unsigned int newTime);
double getFPS();*/

#include <ostream>
#include <iomanip>

class GameTime
{
public:
    GameTime(void) {}
    ~GameTime(void) {}

    void Init(unsigned int tickCount)
    {
        m_start = tickCount;
        m_previous = m_start;
    }

    void Update(unsigned int tickCount)
    {
        m_current = tickCount;
        m_elapsed = m_current - m_previous;
        m_total = m_current - m_start;        
        m_previous = m_current;
    }

    friend std::ostream& operator << (std::ostream& o, const GameTime & v)
    {
        o << std::setw(3) << v.Elapsed() << " ms";
        return o;
    }

    unsigned int Elapsed() const { return m_elapsed; }
    unsigned int Total() const { return m_total; }
    unsigned int Current() const { return m_current; }

private:    
    unsigned int m_total, m_elapsed, m_current;        
    unsigned int m_start, m_previous;
};

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
            m_current = static_cast<float>(m_frameCount) / static_cast<float>(m_totalElapsedTime) * 1000.0f;            
            
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
    
    //void Reset();
    
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
    unsigned int m_totalFrames;
    unsigned int m_totalElapsedTime;
    unsigned int m_frameCount;
};

#endif
