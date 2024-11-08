#include "logger.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sys/time.h>
#endif
#include <stdio.h>

#define MAX_THREADS 10

#ifdef _WIN32
DWORD WINAPI threadFunction(LPVOID arg) {
#else
void *threadFunction(void *arg) {
#endif
  int i = *(int *) arg;
  int j;
  for (j = 0; j < 10000; j++) {
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
  return 0;
}

int main() {
#ifdef _WIN32
  LARGE_INTEGER frequency, start, end;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&start);
  HANDLE threads[MAX_THREADS];
#else
  struct timeval start, end;
  gettimeofday(&start, NULL);
  pthread_t threads[MAX_THREADS];
#endif
  int thread_ids[MAX_THREADS];
  int i;
  double elapsed;

  for (i = 0; i < MAX_THREADS; i++) {
    thread_ids[i] = i;
#ifdef _WIN32
    threads[i] = CreateThread(NULL, 0, threadFunction, &thread_ids[i], 0, NULL);
#else
    pthread_create(&threads[i], NULL, threadFunction, &thread_ids[i]);
#endif
  }

  for (i = 0; i < MAX_THREADS; i++) {
#ifdef _WIN32
    WaitForSingleObject(threads[i], INFINITE);
    CloseHandle(threads[i]);
#else
    pthread_join(threads[i], NULL);
#endif
  }

#ifdef _WIN32
  QueryPerformanceCounter(&end);
  elapsed = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;
#else
  gettimeofday(&end, NULL);
  elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
#endif
  printf("Elapsed time: %fs\n", elapsed);

  return 0;
}
