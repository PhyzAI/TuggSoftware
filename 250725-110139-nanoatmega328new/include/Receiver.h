class Receiver{
    public:
        int channel_info[8][2] = {-1}; //8 channels with 3 attributes
        Receiver();
    void config_channel(int channel, int input, int output);
    void connect_receiver(int ch); //Not being used (too laggy, if you do use it, you need to connect channels 1 and 2)
    void disable_forward(int forward_channel, int relay_pin); //should normally be 1
    private:
};