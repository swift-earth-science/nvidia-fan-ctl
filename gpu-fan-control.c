#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <nvml.h>

// Global variables for cleanup
static volatile int running = 1;
static nvmlDevice_t device;

// Signal handler for graceful shutdown
void handle_signal(int signum) {
    printf("\nReceived signal %d, shutting down...\n", signum);
    running = 0;
}

// Set fan speed and return 1 if successful, 0 if failed
int set_fan_speed(unsigned int speed) {
    nvmlReturn_t result;

    // Set manual fan control
    result = nvmlDeviceSetFanControlPolicy(device, 0, NVML_FAN_POLICY_MANUAL);
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "Failed to set fan control policy: %s\n", nvmlErrorString(result));
        return 0;
    }

    // Set primary fan
    result = nvmlDeviceSetFanSpeed_v2(device, 0, speed);
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "Failed to set fan 0 speed: %s\n", nvmlErrorString(result));
        return 0;
    }

    // Try to set second fan if it exists
    result = nvmlDeviceSetFanSpeed_v2(device, 1, speed);
    // We don't check for errors here as not all cards have two fans

    printf("Set fan speed to %u%%\n", speed);
    return 1;
}

// Calculate fan speed based on temperature
unsigned int get_target_fan_speed(unsigned int temp) {
    if (temp >= 50) return 100;
    if (temp >= 40) return 80;
    if (temp >= 30) return 30; 
    return 0;  // Stop fans below 30°C
}

// Reset fans to automatic control
void restore_auto_fan_control() {
    nvmlDeviceSetFanControlPolicy(device, 0, NVML_FAN_POLICY_TEMPERATURE_CONTINOUS_SW);
    nvmlDeviceSetFanControlPolicy(device, 1, NVML_FAN_POLICY_TEMPERATURE_CONTINOUS_SW);
    printf("Restored automatic fan control\n");
}

int main() {
    nvmlReturn_t result;
    unsigned int temperature, current_speed = 0, target_speed;

    // Set up signal handlers
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    // Initialize NVML
    result = nvmlInit();
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "Failed to initialize NVML: %s\n", nvmlErrorString(result));
        return 1;
    }

    // Get the device handle
    result = nvmlDeviceGetHandleByIndex(0, &device);
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "Failed to get device handle: %s\n", nvmlErrorString(result));
        nvmlShutdown();
        return 1;
    }

    printf("GPU Fan Control Service Started\n");

    // Main control loop
    while (running) {
        // Get GPU temperature
        result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temperature);
        if (result != NVML_SUCCESS) {
            fprintf(stderr, "Failed to get temperature: %s\n", nvmlErrorString(result));
            sleep(1);
            continue;
        }

        // Calculate target speed based on temperature
        target_speed = get_target_fan_speed(temperature);

        // Only update if speed needs to change
        if (target_speed != current_speed) {
            if (set_fan_speed(target_speed)) {
                current_speed = target_speed;
            }
        }

        // Log current status
        printf("Temperature: %u°C, Fan Speed: %u%%\n", temperature, current_speed);

        // Wait before next check
        sleep(2);
    }

    // Cleanup
    printf("Shutting down...\n");
    restore_auto_fan_control();
    nvmlShutdown();
    return 0;
}