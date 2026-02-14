
namespace HeartBeat {
    class UIManager{
        private:
        int readerCount = 0;
        public:
        void addReader(){readerCount++;}
        void decReader(){readerCount--;}
        bool hasReader(){return !!readerCount;}

        static UIManager * getInstance();  
    };
}