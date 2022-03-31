//
// Created by Thibault PLET on 19/05/2020.
//

#ifndef COM_OSTERES_ARDUINORESTART_H
#define COM_OSTERES_ARDUINORESTART_H

#include <ArduinoProperty.h>

class ArduinoRestart {
public:
    /**
     * Constructor
     *
     * @param interval    Delay before restart
     * @param software    Software mode or hardware if false
     * @param pinReset    Pin for restart in hardware mode
     *                    Note that to use hardware restart, you need to link RESET_PIN to RST
     */
    ArduinoRestart(
        unsigned long interval = 60000U, // 1 min
        bool software = true,
        unsigned int pinReset = 2 // D2
    ) {
        this->interval = new DataBuffer(interval, interval);
        this->software = software;
        this->pinReset = pinReset;
    }

    /**
     * Destructor
     */
    ~ArduinoRestart()
    {
        delete this->interval;
    }

    /**
     * Setup
     */
    void setup()
    {
        if (this->isEnable()) {
            // Keep reset pin HIGH (need to be the first command to prevent infinite reset!)
            digitalWrite(this->pinReset, HIGH);

            // Configure pin
            pinMode(this->pinReset, OUTPUT);
        }
    }

    /**
     * Process
     */
    void loop()
    {
        if (this->isEnable()) {
            // Check interval
            if (this->interval->isOutdated()) {
              this->interval->reset();
              // Restart
              this->restart();
              // Code here: Never executed
            }
        }
    }

    /**
     * Restart
     */
    void restart()
    {

        #if defined(MySensors_h) && defined(MY_DEBUG)
        this->sendLog(String("Do restart").c_str());
        #endif

        // Software
        if (this->isSoftware()) {
            /*
            // Watchdog method: Don't work on Arduino clone
            wdt_enable(WDTO_15MS);
            while(1) {}
            */
            delay(5);
            // Restart program to beginning (and reset all timers)
            asm volatile ("  jmp 0");
        } 
        // Hardware
        else {
            digitalWrite(this->pinReset, LOW);
        }
    }

    /**
     * Flag to indicate if restart done on software mode or hardware
     */
    bool isSoftware()
    {
        return this->software;
    }

    /**
     * Set feature enable or not
     */
    void setEnable(bool enable)
    {
        this->enable = enable;
    }

    /**
     * Flag for enable feature
     */
    bool isEnable()
    {
        return this->enable;
    }
    

protected:

    #if defined(MySensors_h)
    /**
     * Send log
     *
     * @param char * message Log message (max 25 bytes). To confirm: 10 char max
     */
    void sendLog(const char * message)
    {
        MyMessage msg;
        msg.sender = getNodeId();
        msg.destination = GATEWAY_ADDRESS;
        msg.sensor = NODE_SENSOR_ID;
        msg.type = I_LOG_MESSAGE;
        mSetCommand(msg, C_INTERNAL);
        mSetRequestEcho(msg, true);
        mSetEcho(msg, false);
        
        msg.set(message);
                
        _sendRoute(msg);
    }
    #endif

    /*
     * Enable feature or not
     */
    bool enable = true;

    /**
     * Flag to indicate if reastart should be done on software mode or hardware
     */
    bool software = true;
    
    /**
     * Interval for restart
     */
    DataBuffer * interval;

    /**
     * Pin for reset in hardware mode
     */
    unsigned int pinReset = 2; // D2
};

#endif //COM_OSTERES_ARDUINORESTART_H
