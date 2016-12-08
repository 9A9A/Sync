#ifndef _OBJECT_POOL_H_
#define _OBJECT_POOL_H_
#include <mutex>
#include <stack>
#include <vector>
template<typename ObjectType>
class SimpleObjectPool
{
    std::mutex m_lock;
    std::stack<ObjectType* , std::vector<ObjectType*>> m_pool;
public:
    void Free ( ObjectType* obj )
    {
        std::lock_guard<std::mutex> locker ( m_lock );
        if ( obj )
        {
            m_pool.push ( obj );
        }
    }
    ObjectType* Alloc ( )
    {
        std::lock_guard<std::mutex> locker ( m_lock );
        ObjectType* obj_t;
        if ( m_pool.empty ( ) )
        {
            obj_t = new ObjectType ( );
        }
        else
        {
            obj_t = m_pool.top ( );
            m_pool.pop ( );
        }
        return obj_t;
    }
    size_t Size ( ) const
    {
        return m_pool.size ( );
    }
};
template<typename ObjectType>
class TLSObjectPool
{
    std::stack<ObjectType* , std::vector<ObjectType*>> m_pool;
public:
    TLSObjectPool ( )
    {

    }
    ~TLSObjectPool ( )
    {
        while ( !m_pool.empty ( ) )
        {
            delete m_pool.top ( );
            m_pool.pop ( );
        }
        system ( "pause" );
    }
    void Free ( ObjectType* obj )
    {
        if ( obj )
        {
            m_pool.push ( obj );
        }
    }
    ObjectType* Alloc ( )
    {
        ObjectType* obj_t;
        if ( m_pool.empty ( ) )
        {
            obj_t = new ObjectType ( );
        }
        else
        {
            obj_t = m_pool.top ( );
            m_pool.pop ( );
        }
        return obj_t;
    }
    size_t Size ( ) const
    {
        return m_pool.size ( );
    }
};
#endif // _OBJECT_POOL_H_