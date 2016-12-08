#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#define  _CRT_SECURE_NO_WARNINGS 1
#include "CompletionPort.h"
#include "Overlapped.h"
#include "ObjectPool.h"
#include <vector>
#include <memory>
#include <thread>
class ThreadPool : public CompletionPort /*, public SimpleObjectPool<OverlappedEx>*/
{
    std::vector<std::unique_ptr<std::thread>> m_pthreads;
    static thread_local TLSObjectPool<OverlappedEx> m_ObjectPool;
    void Execute ( );
    ThreadPool ( );
    virtual ~ThreadPool ( );
public:
    static ThreadPool& Instance ( );   
    OverlappedEx* Allocate ( );
    void Free ( OverlappedEx* );
};
#endif // _THREAD_POOL_H_
