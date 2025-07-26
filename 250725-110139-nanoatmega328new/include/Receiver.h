class Receiver{
    public:
        bool disable_fwd;
        int channel_info[8][2] = {-1}; //8 channels with 3 attributes
        Receiver();
    void config_channel(int channel, int input, int output);
    void connect_receiver(int ch);
    private:
};