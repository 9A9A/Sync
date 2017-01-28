#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#define  _CRT_SECURE_NO_WARNINGS 1
#define OBJECT_POOL_SIZE 32
#include "CompletionPort.h"
#include "Overlapped.h"
#include "ObjectPool.h"
#include <vector>
#include <memory>
#include <thread>
class ThreadPool : public CompletionPort /*, public SimpleObjectPool<OverlappedEx>*/
{
    std::vector<std::unique_ptr<std::thread>> m_pthreads;
    SimpleObjectPool<OverlappedEx , OBJECT_POOL_SIZE> m_ObjectPool;
    void Execute ( );
    ThreadPool ( );
    virtual ~ThreadPool ( );
public:
    static ThreadPool& Instance ( );   
    OverlappedEx* Allocate ( );
    void Wait ( );
    size_t Size ( )const;
    void Shutdown ( );
    void Free ( OverlappedEx* );
};
#endif // _THREAD_POOL_H_
