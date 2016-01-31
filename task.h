#ifndef __TASK_H__
#define __TASK_H__

#include "headers.h"

using namespace std;

class CTask{
	protected:
		string m_strTaskName;
		void *m_ptrData;
	public:
		CTask(){}
		CTask(string taskName): m_strTaskName(taskName), m_ptrData(NULL) {}
		virtual int Run() = 0;
		void SetData(void *data);
};

#endif
