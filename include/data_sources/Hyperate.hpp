#pragma once

#include "DataSource.hpp"

namespace HeartBeat{

class HeartBeatHypeRateDataSource:public DataSource{
    private:
        volatile int the_heart;
        volatile bool has_unread_heart_data = false;
        
        bool closed = false; // set to true to close the thread

        void CreateSocket();

        bool resetRequest = false;


    public:
        HeartBeatHypeRateDataSource();
        bool GetData(int& heartbeat) override;
    
        static void * ServerThread(void *self);
        
        void ResetConnection(){
            resetRequest = true;
        }

        bool has_message_from_server = false;
        char message_from_server[256];
        std::mutex message_from_server_mutex;
    private:
};


}