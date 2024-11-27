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
    unsigned int device_count, fan_speed, temp;
    nvmlDevice_t device;
    char name[NVML_DEVICE_NAME_BUFFER_SIZE];
    char version[NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE];
    
    // Initialize NVML
    result = nvmlInit();
    check_nvml_error(result, "initializing NVML");
    
    // Get NVML version
    result = nvmlSystemGetDriverVersion(version, NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);
    check_nvml_error(result, "getting driver version");
    printf("NVML Driver Version: %s\n", version);
    
    // Get device count
    result = nvmlDeviceGetCount(&device_count);
    check_nvml_error(result, "getting device count");
    
    printf("Found %d NVIDIA devices\n", device_count);
    
    // Get handle for first GPU
    result = nvmlDeviceGetHandleByIndex(0, &device);
    check_nvml_error(result, "getting device handle");
    
    // Get device name
    result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
    check_nvml_error(result, "getting device name");
    printf("GPU: %s\n", name);
    
    while (1) {
        // Get fan speed
        result = nvmlDeviceGetFanSpeed(device, &fan_speed);
        if (result != NVML_SUCCESS) {
            printf("Fan speed read error: %s\n", nvmlErrorString(result));
        }
        
        // Get temperature
        result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
        if (result != NVML_SUCCESS) {
            printf("Temperature read error: %s\n", nvmlErrorString(result));
        } else {
            printf("GPU Temperature: %u°C  ", temp);
        }
        
        printf("Fan Speed: %u%%\n", fan_speed);
        sleep(1);
    }
    
    nvmlShutdown();
    return 0;
}