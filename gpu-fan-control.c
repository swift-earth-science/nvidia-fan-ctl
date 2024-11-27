#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <nvml.h>

void check_nvml_error(nvmlReturn_t result, const char* message) {
    if (result != NVML_SUCCESS) {
        fprintf(stderr, "Error %s: %s\n", message, nvmlErrorString(result));
        nvmlShutdown();
        exit(1);
    }
}

int main() {
    nvmlReturn_t result;
    unsigned int device_count, fan_speed;
    nvmlDevice_t device;
    
    // Initialize NVML
    result = nvmlInit();
    check_nvml_error(result, "initializing NVML");
    
    // Get device count
    result = nvmlDeviceGetCount(&device_count);
    check_nvml_error(result, "getting device count");
    
    printf("Found %d NVIDIA devices\n", device_count);
    
    // Get handle for first GPU
    result = nvmlDeviceGetHandleByIndex(0, &device);
    check_nvml_error(result, "getting device handle");
    
    while (1) {
        // Get fan speed
        result = nvmlDeviceGetFanSpeed(device, &fan_speed);
        check_nvml_error(result, "getting fan speed");
        
        printf("GPU Fan Speed: %u%%\r", fan_speed);
        fflush(stdout);
        
        sleep(1);  // Update every second
    }
    
    // Cleanup
    nvmlShutdown();
    return 0;
}