#pragma once

#include "DataSource.hpp"
#include <string>
#include <mutex>

namespace HeartBeat{

class HeartBeatPulsoidDataSource:public DataSource{
    private:
        volatile int the_heart;
        volatile bool has_unread_heart_data = false;
        
        bool closed = false; // set to true to close the thread

        void CreateSocket();

        bool resetRequest = false;

        bool safe_pair_wanted = false;
        bool safe_pairing = false;

        bool safe_pair_done_wanted = false;
    public:
        HeartBeatPulsoidDataSource();
        bool GetData(int& heartbeat) override;
    
        static void * ServerThread(void *self);
        
        void ResetConnection(){
            resetRequest = true;
        }

        void RequestPair(std::string pair_str);
        void RequestSafePair();
        void CancelSafePair(){
            safe_pair_wanted = false;
            safe_pairing = false;
        }
        bool IsSafePairing(){
            return safe_pairing;
        }

        void SafePairDone(){
            safe_pair_done_wanted = true;
        }


        bool modconfig_is_dirty = false;

        // a async message from service thread to main thread.
        bool url_open_wanted = false;
        std::string url;
        std::mutex url_mutex;


        bool err_message_dirty = false;
        std::string err_message;
        std::mutex err_message_mutex;
    private:

        void err(std::string message){
            std::lock_guard<std::mutex> g(err_message_mutex);
            err_message = message;
            err_message_dirty = true;
        }
};
    



}