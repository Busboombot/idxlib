/*
 *  Class for an IDX Robot Limit Switches
 */

#ifndef  idx_limit_h
#define idx_limit_h


class IDXLimit {

    public:

        IDXLimit();
        
        void begin();
        
        bool is_in_limit();
      
};

class IDXPinLimit :  public IDXLimit {

    public:

        IDXPinLimit(int pin);
      


};

class IDXMcpLimit :  public IDXLimit {

    public:

        IDXMcpLimit(int pin, int mcp_address=0);
      


};


#endif