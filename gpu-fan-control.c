#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <nvml.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <fan_speed_percentage>\n", argv[0]);
        return 1;
    }

    nvmlReturn_t result;
    nvmlDevice_t device;
    unsigned int fan_speed = atoi(argv[1]);

    if (fan_speed > 100) {
        printf("Fan speed must be between 0 and 100\n");
        return 1;
    }

    // Initialize NVML
    result = nvmlInit();
    if (result != NVML_SUCCESS) {
        printf("Failed to initialize NVML: %s\n", nvmlErrorString(result));
        return 1;
    }

    // Get the device handle
    result = nvmlDeviceGetHandleByIndex(0, &device);
    if (result != NVML_SUCCESS) {
        printf("Failed to get device handle: %s\n", nvmlErrorString(result));
        nvmlShutdown();
        return 1;
    }

    // Set manual fan control
    result = nvmlDeviceSetFanControlPolicy(device, 0, NVML_FAN_POLICY_MANUAL);
    if (result != NVML_SUCCESS) {
        printf("Failed to set fan control policy: %s\n", nvmlErrorString(result));
        nvmlShutdown();
        return 1;
    }

    // Set the fan speed
    result = nvmlDeviceSetFanSpeed_v2(device, 0, fan_speed);
    if (result != NVML_SUCCESS) {
        printf("Failed to set fan speed: %s\n", nvmlErrorString(result));
        nvmlShutdown();
        return 1;
    }

    printf("Successfully set fan 0 to %u%%\n", fan_speed);

    // Set the second fan if it exists
    result = nvmlDeviceSetFanSpeed_v2(device, 1, fan_speed);
    if (result == NVML_SUCCESS) {
        printf("Successfully set fan 1 to %u%%\n", fan_speed);
    }

    nvmlShutdown();
    return 0;
}