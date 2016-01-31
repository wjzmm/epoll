#include "pool.h"
//#include <iostream>

vector<CTask*> CThreadPool::m_vecTaskList;

bool CThreadPool::shutdown = false;
pthread_mutex_t CThreadPool::m_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CThreadPool::m_pthreadCond = PTHREAD_COND_INITIALIZER;

CThreadPool::CThreadPool(int threadNum): m_iThreadNum(threadNum){
	printf("will create %d threads", threadNum);
	Create();
}

void *CThreadPool::ThreadFunc(void *threadData){
	pthread_t tid = pthread_self();
	while(1){
		pthread_mutex_lock(&m_pthreadMutex);
		while(m_vecTaskList.size() == 0 && !shutdown){
			pthread_cond_wait(&m_pthreadCond, &m_pthreadMutex);
		}
		if(shutdown){
			pthread_mutex_unlock(&m_pthreadMutex);
			printf("thread %lu will exit\n", pthread_self());
			pthread_exit(NULL);
		}
		printf("tid %lu run \n", tid);
		vector<CTask*>::iterator iter = m_vecTaskList.begin();

		CTask *task = *iter;
		if(iter != m_vecTaskList.end()){
			task = *iter;
			m_vecTaskList.erase(iter);
		}
		pthread_mutex_unlock(&m_pthreadMutex);
		task->Run();
		printf("tid: %lu idle\n", tid);
	}
	return (void *)0;
}

int CThreadPool::AddTask(CTask *task){
	pthread_mutex_lock(&m_pthreadMutex);
	this->m_vecTaskList.push_back(task);
	pthread_mutex_unlock(&m_pthreadMutex);
	pthread_cond_signal(&m_pthreadCond);
	return 0;
}

int CThreadPool::Create(){
	pthread_id = (pthread_t*)malloc(sizeof(pthread_t) * m_iThreadNum);
	for(int i=0; i<m_iThreadNum; ++i){
		pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);
	}
	return 0;
}

int CThreadPool::StopAll(){
	if(shutdown)
		return -1;
	printf("Now will end all threads!!\n");

	shutdown = true;
	pthread_cond_broadcast(&m_pthreadCond);

	for(int i=0; i<m_iThreadNum; ++i){
		pthread_join(pthread_id[i], NULL);
	}
	free(pthread_id);
	pthread_id = NULL;

	pthread_mutex_destroy(&m_pthreadMutex);
	pthread_cond_destroy(&m_pthreadCond);
	return 0;
}

int CThreadPool::GetTaskSize(){
	return m_vecTaskList.size();
}
