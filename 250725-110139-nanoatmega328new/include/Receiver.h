#include <vector> // Required for std::vector

class Receiver{
    public:
        bool interrupt;
        std::vector<int[3]> channel_info;
        Receiver();
    void config_channel(int channel, int pins[2]);
    void connect_receiver(int input_ch, int output_ch);
    private:
};