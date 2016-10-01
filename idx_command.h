// Structures for movement commands

#ifndef idx_command_h
#define idx_command_h

#include <Arduino.h>
#include <limits.h>
#include <LinkedList.h>  // https://github.com/ivanseidel/LinkedList
#define IDX_COMMAND_NACK 0  // Failed to read payload
#define IDX_COMMAND_ACK 1   // Payload successfully stored in meggage list
#define IDX_COMMAND_DONE 2  // COmmand completed
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
    byte sync[4] = {'I','D','X','C'};
    uint16_t code; // command code
    uint16_t seq; // Packet sequence
    int32_t directions; // Direction bits
    float velocities[6] = {NAN,NAN,NAN,NAN,NAN,NAN}; 
    int32_t positions[6] = {0,0,0,0,0,0}; 
    uint32_t crc; // Payload CRC
};


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
    
    
    inline void removeMessage(){
        if ( size() == 0){
            return;
        }
        
        delete commands.shift();
        
    }
    
    inline struct command * getMessage(){
        if ( size() == 0){
            return 0;
        }
        
        commands.get(0);
        
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