// Structures for movement commands

#ifndef idx_command_h
#define idx_command_h

#include <Arduino.h>
#include <limits.h>
#include <LinkedList.h>  // https://github.com/ivanseidel/LinkedList
#define IDX_COMMAND_NACK 0  // Failed to read payload
#define IDX_COMMAND_ACK 1   // Payload successfully stored in meggage list
#define IDX_COMMAND_DONE 2  // COmmand completed
#define IDX_COMMAND_POSITION 10
#define IDX_COMMAND_VELOCITY 11
#define IDX_COMMAND_ACCELERATION 12
#define IDX_COMMAND_POSITIONQUERY 20

// A Header packet that preceedes all commands, and also serves as
// ACK and NACK

struct command_header {
    byte sync[4] = {'I','D','X','C'};
    uint16_t code; // command code
    uint16_t length; // payload length
    int32_t seq; // Packet sequence
    int32_t crc; // Payload CRC
    int32_t checksum; // Checksum for header. Actaull a uint16_t, but gets padded to 32b anyway. 
};

struct acceleration_command {
    float t;
    float accelerations[6] = {NAN,NAN,NAN,NAN,NAN,NAN}; 
    int32_t positions[6] = {LONG_MIN,LONG_MIN,LONG_MIN,LONG_MIN,LONG_MIN,LONG_MIN}; 
};

struct velocity_command {
    float t;
    float velocities[6] = {NAN,NAN,NAN,NAN,NAN,NAN}; 
    int32_t positions[6] = {LONG_MIN,LONG_MIN,LONG_MIN,LONG_MIN,LONG_MIN,LONG_MIN}; 
};

struct position_command {
    int32_t positions[6] = {LONG_MIN,LONG_MIN,LONG_MIN,LONG_MIN,LONG_MIN,LONG_MIN}; 
};

struct message {
    struct command_header *header;
    void* command;
    void* scratch;
};

uint16_t fletcher16( uint8_t const *data, size_t bytes );

class IDXCommandBuffer {

private:
    
    LinkedList<struct message*> messages;
    
    // Buffer to load incomming command headers
    char header_buf[sizeof(struct command_header)];
    int header_buf_pos = 0;
    
    Serial_ &ser; // SerialUSB on the Arduino Due
    
    struct command_header response;
    
public:
    
    IDXCommandBuffer(Serial_ &ser) : ser(ser) {
        
    }

    inline void sendHeader(struct command_header & header){
              
        header.checksum = fletcher16( (uint8_t*)&header, 
                                    sizeof(struct command_header)-sizeof(int32_t) );
                               
        ser.write( (uint8_t*)&header, sizeof(struct command_header));
    }

    int runForHeader(){
        
        while(ser.available()){
          header_buf[header_buf_pos++] =  ser.read();

          if (header_buf_pos == sizeof(struct command_header)){
            struct message *msg = new message();
            msg->header =  new command_header();
            memcpy(msg->header, header_buf, sizeof(struct command_header));
            memcpy(&response, header_buf, sizeof(struct command_header));
            header_buf_pos = 0;
            messages.add(msg);
            
            response.code = IDX_COMMAND_ACK;
            response.length = 0;

            sendHeader(response);
            
          } 
        }
        
        return header_buf_pos;
    }

    
    // After a header is loaded, load a payload. 
    void runForPayload(){
        
    }
    
    inline int size(){
        return messages.size();
    }
    
    inline void freeMsg(struct message * msg){
        delete msg->header;
        delete msg;
    }
    
    inline struct message * nextMessage(){
        if ( size() == 0){
            return 0;
        }
        
        return messages.pop();
        
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