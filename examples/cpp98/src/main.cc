#include "logger.h"
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <pthread.h>
#include <sys/time.h>
#endif

#define MAX_THREADS 10

void *threadFunction(void *arg) {
  int i = *(int *) arg;
  for (int j = 0; j < 10000; j++) {
#ifdef _WIN32
    DWORD this_id = GetCurrentThreadId();
#else
    pthread_t this_id = pthread_self();
#endif

    LOGDEBUG("Thread %d started with id %lu", i, this_id);
    LOGINFO("Thread %d started with id %lu", i, this_id);
    LOGERROR("Thread %d started with id %lu", i, this_id);
    LOGWARN("Thread %d started with id %lu", i, this_id);
  }
  return NULL;
}

#ifdef _WIN32
unsigned __stdcall threadFunctionWrapper(void *arg) {
  return (unsigned) threadFunction(arg);
}
#endif

int main() {
#ifdef _WIN32
  SYSTEMTIME start, end;
  GetSystemTime(&start);
#else
  struct timeval start, end;
  gettimeofday(&start, NULL);
#endif

  std::vector<
#ifdef _WIN32
    HANDLE
#else
    pthread_t
#endif
    >
    threads(MAX_THREADS);
  std::vector<int> thread_ids(MAX_THREADS);
  for (int i = 0; i < MAX_THREADS; i++) {
    thread_ids[i] = i;
#ifdef _WIN32
    threads[i] = (HANDLE) _beginthreadex(NULL, 0, threadFunctionWrapper, &thread_ids[i], 0, NULL);
#else
    pthread_create(&threads[i], NULL, threadFunction, &thread_ids[i]);
#endif
  }

  for (int i = 0; i < MAX_THREADS; i++) {
#ifdef _WIN32
    WaitForSingleObject(threads[i], INFINITE);
    CloseHandle(threads[i]);
#else
    pthread_join(threads[i], NULL);
#endif
  }

#ifdef _WIN32
  GetSystemTime(&end);
  double elapsed = (end.wSecond - start.wSecond) + (end.wMilliseconds - start.wMilliseconds) / 1e3;
#else
  gettimeofday(&end, NULL);
  double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
#endif
  std::cout << "Elapsed time: " << elapsed << "s\n";

  return 0;
}
