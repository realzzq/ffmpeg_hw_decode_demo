
#include "ThreadSafeQueue.h"
#include "utils/const.h"

ThreadSafeQueue::ThreadSafeQueue() {
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);

}

ThreadSafeQueue::~ThreadSafeQueue() {
    clearAvpacket();
}

int ThreadSafeQueue::putAvpacket(AVPacket *packet) {

    pthread_mutex_lock(&mutexPacket);
    AVPacket* tmp = av_packet_alloc();
    av_packet_ref(tmp, packet);
    queuePacket.push(tmp);
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);


    return 0;
}

int ThreadSafeQueue::getAvpacket(AVPacket **packet) {

    pthread_mutex_lock(&mutexPacket);

    while(true)
    {
        if(queuePacket.size() > 0)
        {
            *packet = queuePacket.front();
            queuePacket.pop();
            break;
        } else{
            pthread_cond_wait(&condPacket, &mutexPacket);
        }
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int ThreadSafeQueue::getQueueSize() {

    int size = 0;
    pthread_mutex_lock(&mutexPacket);
    size = queuePacket.size();
    pthread_mutex_unlock(&mutexPacket);

    return size;
}

void ThreadSafeQueue::clearAvpacket() {
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);

    while (!queuePacket.empty())
    {
        AVPacket *packet = queuePacket.front();
        queuePacket.pop();
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
    pthread_mutex_unlock(&mutexPacket);

}
