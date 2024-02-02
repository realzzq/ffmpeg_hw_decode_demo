
#ifndef MYMUSIC_WLQUEUE_H
#define MYMUSIC_WLQUEUE_H

#include "queue"
#include "pthread.h"

extern "C"
{
#include "libavcodec/avcodec.h"
};


class ThreadSafeQueue {

public:
    std::queue<AVPacket *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;

public:

    ThreadSafeQueue();
    ~ThreadSafeQueue();

    int putAvpacket(AVPacket *packet);
    int getAvpacket(AVPacket **packet);

    int getQueueSize();

    void clearAvpacket();




};


#endif //MYMUSIC_WLQUEUE_H
