#include "Copter.h"

#include <AP_HAL/AP_HAL.h>
#include <AP_RangeFinder/AP_RangeFinder_Backend.h>
#include <GCS_MAVLink/GCS_Dummy.h>
#include <AP_RobotisServo.h>

static AP_SerialManager serial_manager;
AP_HAL::UARTDriver *lidar_uart = nullptr;
static RangeFinder sonar;
//static AP_HAL::GPIO *servo = nullptr;
static uint8_t servo_channel = 0; // Default servo channel
static AP_RobotisServo servo;

static uint8_t openServo = 240;
static uint8_t closedServo = 40; 

bool ModeBasketball::init(bool ignore_checks)
{
    hal.console->printf("Basketball start init\n");

    if (!ignore_checks) {
        hal.console->printf("Ignore checks is set true, exiting\n");
        return false;
    }
    hal.console->printf("Basketball continue init\n");


    // Connect lidar
    lidar_uart = serial_manager.find_serial(AP_SerialManager::SerialProtocol::SerialProtocol_Lidar360, 0);
    if (!lidar_uart) {
        hal.console->printf("Failed to initialize UART6\n");
        return false;
    }
    lidar_uart->begin(460800);
    hal.console->printf("Lidar init complete\n");

    // Initialize servo
    servo = AP_RobotisServo.AP_RobotisServo();
    servo.init();
    if (!servo) {
        hal.console->printf("Failed to initialize servo driver\n");
        return false;
    }
    hal.console->printf("Servo initialized on channel %u with default frequency 50 Hz\n", servo_channel);

    return true;
}

void ModeBasketball::run()
{
    uint8_t buffer[256];
    size_t bytes_read = 0;

    // Check if UART is initialized and has data
    hal.console->printf("Cycle start\n");

    if (lidar_uart && lidar_uart->available() > 0) {
        // Read data from UART6
        bytes_read = lidar_uart->read(buffer, 255);
        buffer[bytes_read] = '\0'; // Ensure it's null-terminated for debugging

        // Process the data
        for (size_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '$') { // Example: Start of a message
                hal.console->printf("Header found: ");
            }
            hal.console->printf("%c", buffer[i]); // Print as ASCII for debugging

            // Add parsing logic here for your specific Lidar protocol
        }
        hal.console->printf("\n");
    } else {
        hal.console->printf("No lidar/not available\n");
    }


    // Example: Change servo frequency dynamically
    static uint32_t last_time = 0;
    uint32_t now = AP_HAL::millis();

    if (now - last_time > 1000) { // Every second
        last_time = now;
        static int freq = 50;
        freq += 10;

        if (freq > 100) freq = 50; // Reset frequency after 100 Hz

        servo->set_freq(servo_channel, freq);
        hal.console->printf("Changed servo frequency to %d Hz\n", freq);
    }

    // Minimal delay to avoid overwhelming the system
    hal.scheduler->delay(10);
}
