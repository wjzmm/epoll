#ifndef __POOL_H__
#define __POOL_H__

#include "headers.h"
#include "task.h"

using namespace std;

class CThreadPool {
	private:
		static vector<CTask*> m_vecTaskList;
		static bool shutdown;
		int m_iThreadNum;
		pthread_t *pthread_id;

		static pthread_mutex_t m_pthreadMutex;
		static pthread_cond_t m_pthreadCond;

	protected:
		static void *ThreadFunc(void *threadData);
		static int MoveToIdle(pthread_t tid);
		static int MoveToBusy(pthread_t tid);

		int Create();
	
	public:
		CThreadPool(int threadNum = 10);
		int AddTask(CTask *task);
		int StopAll();
		int GetTaskSize();
};
#endif
