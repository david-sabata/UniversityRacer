#ifndef GAMETIME_H
#define GAMETIME_H

#include <ostream>
#include <iomanip>

/**
 * Trida reprezentujici casovy udaj.
 */
class TimeSpan
{
    public:
        TimeSpan(): m_ms(0) {}
        TimeSpan(unsigned int ms): m_ms(ms) {}
    
        unsigned int ms() const { return m_ms; }
        float s() const { return m_ms / 1000.0f; }
    
        static TimeSpan FromMs(unsigned int ms) { return TimeSpan(ms); }
        static TimeSpan FromS(unsigned int s) { return TimeSpan(s * 1000); }
        static TimeSpan Zero() { return TimeSpan(); }
        
        TimeSpan operator -  (const TimeSpan & ts) const { return TimeSpan(m_ms -  ts.m_ms); }
        TimeSpan operator +  (const TimeSpan & ts) const { return TimeSpan(m_ms +  ts.m_ms); }
        TimeSpan operator += (const TimeSpan & ts)       { return TimeSpan(m_ms += ts.m_ms); }
        TimeSpan operator += (unsigned int ms)           { return TimeSpan(m_ms += ms);      }
        bool     operator == (const TimeSpan & ts) const { return m_ms == ts.m_ms; }
        bool     operator != (const TimeSpan & ts) const { return m_ms != ts.m_ms; }
        bool     operator >  (const TimeSpan & ts) const { return m_ms >  ts.m_ms; }
        bool     operator >= (const TimeSpan & ts) const { return m_ms >= ts.m_ms; }
        bool     operator <  (const TimeSpan & ts) const { return m_ms <  ts.m_ms; }
        bool     operator <= (const TimeSpan & ts) const { return m_ms <= ts.m_ms; }

        friend std::ostream& operator << (std::ostream& o, const TimeSpan & v)
        {
            unsigned int sec = v.ms() / 1000;
            
            o <<        std::setw(2) << std::setfill('0') << sec / 60
              << ":" << std::setw(2) << std::setfill('0') << sec % 60
              << "." << std::setw(3) << std::setfill('0') << v.ms() % 1000;

            return o;
        }
    
    private:
        unsigned int m_ms;
};

/**
 * Reprezentuje herni cas. Na jeho zaklade lze provadet akce nezavisle na FPS ci mereni casu.
 */
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
        o << std::setw(3) << v.Elapsed().ms() << " ms";
        return o;
    }

    TimeSpan Elapsed() const { return m_elapsed; }
    TimeSpan Total() const { return m_total; }
    TimeSpan Current() const { return m_current; }

private:    
    TimeSpan m_total, m_elapsed, m_current;        
    TimeSpan m_start, m_previous;
};

#endif