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
        // Software
        if (this->isSoftware()) {
            /*
            wdt_enable(WDTO_15MS);
            while(1) {}
            */
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
