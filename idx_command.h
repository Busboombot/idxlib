// Structures for movement commands

#ifndef idx_command_h
#define idx_command_h

#include <Arduino.h>
#include <limits.h>
#include <LinkedList.h>  // https://github.com/ivanseidel/LinkedList
#define IDX_COMMAND_NACK 0  // Failed to read payload
#define IDX_COMMAND_ACK 1   // Payload successfully stored in meggage list
#define IDX_COMMAND_DONE 2  // Command completed
#define IDX_COMMAND_APOSITION 10
#define IDX_COMMAND_RPOSITION 11
#define IDX_COMMAND_VELOCITY 12
#define IDX_COMMAND_ACCELERATION 13
#define IDX_COMMAND_POSITIONQUERY 20


#define IDX_READING_HEADER 1
#define IDX_READING_BODY 2 

// A Header packet that preceedes all commands, and also serves as
// ACK and NACK


struct command {
    byte sync[4] = {'I','D','X','C'}; // 4
    uint16_t code = 0; // command code // 2
    uint16_t seq = 0; // Packet sequence //2
    int32_t directions =0 ; // Direction bits // 4
    uint16_t ticks[6] = {0,0,0,0,0,0}; // 12
    uint16_t steps[6] = {0,0,0,0,0,0};  // 12
    uint32_t crc = 0; // Payload CRC // 4
}; // 40

/*
Total 64b
uint16 code
uint16 seq
union { 
    uint32[56] commands;
    struct {
        int32_t pad; //4
        int32_t directions =0 ; // Direction bits // 4
        float velocities[6] = {NAN,NAN,NAN,NAN,NAN,NAN}; // 24
        int32_t positions[6] = {0,0,0,0,0,0};  // 24
    }
}
uint32 crc
*/

uint16_t fletcher16( uint8_t const *data, size_t bytes );

class IDXCommandBuffer {

private:

    LinkedList<struct command*> commands;
    
    struct command *last_command;
    
    int buf_pos = 0;
   
    Serial_ &ser; // SerialUSB on the Arduino Due
    
    struct command response; 
    
    
public:
    
    IDXCommandBuffer(Serial_ &ser) : ser(ser) {
        last_command =  new command();
       
       
    }

    void send(struct command & command);
    
    void sendResponse(struct command & command, int code);
    
    // Wait for a header sync string, then read the entire header. 
    int run();

    inline int size(){
        return commands.size();
    }
    
    inline int buflen(){
        return buf_pos;
    }
    
    inline struct command * getMessage(){
        if ( size() == 0){
            return 0 ;
        }
        
        return commands.shift();
        
    }
    

    
};


class IDXCommandPort {
    
    public:
        
        inline IDXCommandPort(Serial_ &ser) : ser(ser) {
            
        }


        
        void recieveHeader(struct command_header & header){
            
        }
        

        
    private:
        
        Serial_ &ser;
        
    
};
    
#endif