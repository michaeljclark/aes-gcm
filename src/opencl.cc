/*
 *  opencl.cc
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "logging.h"
#include "opencl.h"


static const char class_name[] = "opencl";


/* opencl_platform */
 
opencl_platform::opencl_platform(cl_platform_id platform_id, uint platform_index)
    : platform_id(platform_id), platform_index(platform_index)
{
    opencl::getPlatformInfo(platform_id, CL_PLATFORM_NAME, name);
    opencl::getPlatformInfo(platform_id, CL_PLATFORM_VENDOR, vendor);
    opencl::getPlatformInfo(platform_id, CL_PLATFORM_VERSION, version);
    opencl::getPlatformInfo(platform_id, CL_PLATFORM_PROFILE, profile);
    opencl::getPlatformInfo(platform_id, CL_PLATFORM_EXTENSIONS, extensions);
    
    parseExtensions();
}

opencl_platform::~opencl_platform() {}

void opencl_platform::parseExtensions()
{
    size_t current;
    size_t next = -1;
    do {
        current = next + 1;
        next = extensions.find_first_of(" ", current);
        std::string extname = extensions.substr(current, next - current);
        std::transform(extname.begin(), extname.end(), extname.begin(), ::tolower);
        extension_set.insert(extname);
    } while (next != std::string::npos);
}

bool opencl_platform::hasExtension(std::string extname)
{
    std::transform(extname.begin(), extname.end(), extname.begin(), ::tolower);
    return (extension_set.find(extname) != extension_set.end());
}

void opencl_platform::print()
{
    log_debug("platform[%u].name                = %s", platform_index, name.c_str());
    log_debug("platform[%u].vendor              = %s", platform_index, vendor.c_str());
    log_debug("platform[%u].profile             = %s", platform_index, profile.c_str());
    log_debug("platform[%u].extensions          = %s", platform_index, extensions.c_str());
    log_debug("platform[%u].version             = %s", platform_index, version.c_str());
}


/* opencl_device */

opencl_device::opencl_device(opencl_platform_ptr platform, cl_device_id deviceId, uint deviceIndex)
    : platform(platform), deviceId(deviceId), deviceIndex(deviceIndex)
{
    opencl::getDeviceInfoType(deviceId, deviceType);
    opencl::getDeviceInfoString(deviceId, CL_DEVICE_NAME, name);
    opencl::getDeviceInfoString(deviceId, CL_DEVICE_VENDOR, vendor);
    opencl::getDeviceInfoString(deviceId, CL_DEVICE_VERSION, deviceVersion);
    opencl::getDeviceInfoString(deviceId, CL_DRIVER_VERSION, driverVersion);
    opencl::getDeviceInfoString(deviceId, CL_DEVICE_PROFILE, profile);
    opencl::getDeviceInfoString(deviceId, CL_DEVICE_EXTENSIONS, extensions);
    opencl::getDeviceInfoSize(deviceId, CL_DEVICE_PROFILING_TIMER_RESOLUTION, profilingTimerResolution);
    opencl::getDeviceInfoBool(deviceId, CL_DEVICE_AVAILABLE, available);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_ADDRESS_BITS, addressBits);
    opencl::getDeviceInfoBool(deviceId, CL_DEVICE_ENDIAN_LITTLE, littleEndian);
    opencl::getDeviceInfoFPConfig(deviceId, CL_DEVICE_SINGLE_FP_CONFIG, floatSingle);
    opencl::getDeviceInfoFPConfig(deviceId, CL_DEVICE_DOUBLE_FP_CONFIG, floatDouble);
    opencl::getDeviceInfoUlong(deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, globalMemCacheSize);
    opencl::getDeviceInfoMemCacheType(deviceId, globalMemCacheType);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, globalMemCacheLineSize);
    opencl::getDeviceInfoUlong(deviceId, CL_DEVICE_GLOBAL_MEM_SIZE, globalMemSize);
    opencl::getDeviceInfoBool(deviceId, CL_DEVICE_IMAGE_SUPPORT, imageSupport);
    opencl::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE2D_MAX_WIDTH, image2DmaxWidth);
    opencl::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE2D_MAX_HEIGHT, image2DmaxHeight);
    opencl::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE3D_MAX_WIDTH, image3DmaxWidth);
    opencl::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE3D_MAX_HEIGHT, image3DmaxHeight);
    opencl::getDeviceInfoSize(deviceId, CL_DEVICE_IMAGE3D_MAX_DEPTH, image3DmaxDepth);
    opencl::getDeviceInfoUlong(deviceId, CL_DEVICE_LOCAL_MEM_SIZE, localMemSize);
    opencl::getDeviceInfoLocalMemType(deviceId, localMemType);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY, maxClockFrequency);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS, maxComputeUnits);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_CONSTANT_ARGS, maxConstantArgs);
    opencl::getDeviceInfoUlong(deviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, maxConstantBufferSize);
    opencl::getDeviceInfoUlong(deviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE, maxMemAllocSize);
    opencl::getDeviceInfoSize(deviceId, CL_DEVICE_MAX_PARAMETER_SIZE, maxParameterSize);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_READ_IMAGE_ARGS, maxReadImageArgs);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, maxWriteImageArgs);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_SAMPLERS, maxSamplers);
    opencl::getDeviceInfoSize(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, maxWorkGroupSize);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, maxWorkItemDim);
    opencl::getDeviceInfoSizeArray(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, maxWorkItemSizes);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_MEM_BASE_ADDR_ALIGN, memBaseAddrAlign);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, minDataAlignSize);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, prefVecWidthChar);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, prefVecWidthShort);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, prefVecWidthInt);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, prefVecWidthLong);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, prefVecWidthFloat);
    opencl::getDeviceInfoUint(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, prefVecWidthDouble);
    
    parseExtensions();
}

opencl_device::~opencl_device() {}

void opencl_device::parseExtensions()
{
    size_t current;
    size_t next = -1;
    do {
        current = next + 1;
        next = extensions.find_first_of(" ", current);
        std::string extname = extensions.substr(current, next - current);
        std::transform(extname.begin(), extname.end(), extname.begin(), ::tolower);
        extension_set.insert(extname);
    } while (next != std::string::npos);
}

bool opencl_device::hasExtension(std::string extname)
{
    std::transform(extname.begin(), extname.end(), extname.begin(), ::tolower);
    return (extension_set.find(extname) != extension_set.end());
}

std::string opencl_device::deviceTypeString()
{
    switch (deviceType) {
        case CL_DEVICE_TYPE_CPU: return "CL_DEVICE_TYPE_CPU";
        case CL_DEVICE_TYPE_GPU: return "CL_DEVICE_TYPE_GPU";
        case CL_DEVICE_TYPE_ACCELERATOR: return "CL_DEVICE_TYPE_ACCELERATOR";
        case CL_DEVICE_TYPE_DEFAULT: return "CL_DEVICE_TYPE_DEFAULT";
        default: return "UNKNOWN";
    }
}

std::string opencl_device::globalMemCacheTypeString()
{
    switch (globalMemCacheType) {
        case CL_NONE: return "CL_NONE";
        case CL_READ_ONLY_CACHE: return "CL_READ_ONLY_CACHE";
        case CL_READ_WRITE_CACHE: return "CL_READ_WRITE_CACHE";
        default: return "UNKNOWN";
    }
}

std::string opencl_device::localMemTypeString()
{
    switch (localMemType) {
        case CL_LOCAL: return "CL_LOCAL";
        case CL_GLOBAL: return "CL_GLOBAL";
        default: return "UNKNOWN";
    }
}

std::string opencl_device::fpconfigString(cl_device_fp_config fpConfig)
{
    std::stringstream ss;
    if (fpConfig & CL_FP_DENORM) ss << "+CL_FP_DENORM";
    if (fpConfig & CL_FP_INF_NAN) ss << "+CL_FP_INF_NAN";
    if (fpConfig & CL_FP_ROUND_TO_NEAREST) ss << "+CL_FP_ROUND_TO_NEAREST";
    if (fpConfig & CL_FP_ROUND_TO_ZERO) ss << "+CL_FP_ROUND_TO_ZERO";
    if (fpConfig & CL_FP_FMA) ss << "+CL_FP_FMA ";
    return ss.str();
}

std::string opencl_device::sizeArrayString(std::vector<size_t> &sizeArray)
{
    std::stringstream ss;
    ss << "[";
    for (std::vector<size_t>::iterator si = sizeArray.begin(); si != sizeArray.end(); si++) {
        if (si != sizeArray.begin()) ss << ",";
        ss << *si;
    }
    ss << "]";
    return ss.str();
}

void opencl_device::print()
{
    log_debug("device[%u].name                     = %s", deviceIndex, name.c_str());
    log_debug("device[%u].vendor                   = %s", deviceIndex, vendor.c_str());
    log_debug("device[%u].type                     = %s", deviceIndex, deviceTypeString().c_str());
    log_debug("device[%u].profile                  = %s", deviceIndex, profile.c_str());
    log_debug("device[%u].extensions               = %s", deviceIndex, extensions.c_str());
    log_debug("device[%u].deviceVersion            = %s", deviceIndex, deviceVersion.c_str());
    log_debug("device[%u].driverVersion            = %s", deviceIndex, driverVersion.c_str());
    log_debug("device[%u].available                = %s", deviceIndex, available ? "TRUE" : "FALSE");
    log_debug("device[%u].addressBits              = %u", deviceIndex, addressBits);
    log_debug("device[%u].littleEndian             = %s", deviceIndex, available ? "TRUE" : "FALSE");
    log_debug("device[%u].floatSingle              = %s", deviceIndex, fpconfigString(floatSingle).c_str());
    log_debug("device[%u].floatDouble              = %s", deviceIndex, fpconfigString(floatDouble).c_str());
    log_debug("device[%u].globalMemCacheSize       = %lu", deviceIndex, globalMemCacheSize);
    log_debug("device[%u].globalMemCacheType       = %s", deviceIndex, globalMemCacheTypeString().c_str());
    log_debug("device[%u].globamMemCacheLineSize   = %u", deviceIndex, globalMemCacheLineSize);
    log_debug("device[%u].globalMemSize            = %lu", deviceIndex, globalMemSize);
    log_debug("device[%u].imageSupport             = %s", deviceIndex, imageSupport ? "TRUE" : "FALSE");
    log_debug("device[%u].image2DmaxWidth          = %u", deviceIndex, (uint)image2DmaxWidth);
    log_debug("device[%u].image2DmaxHeight         = %u", deviceIndex, (uint)image2DmaxHeight);
    log_debug("device[%u].image3DmaxWidth          = %u", deviceIndex, (uint)image3DmaxWidth);
    log_debug("device[%u].image3DmaxHeight         = %u", deviceIndex, (uint)image3DmaxHeight);
    log_debug("device[%u].image3DmaxDepth          = %u", deviceIndex, (uint)image3DmaxDepth);
    log_debug("device[%u].localMemSize             = %lu", deviceIndex, localMemSize);
    log_debug("device[%u].localMemType             = %s", deviceIndex, localMemTypeString().c_str());
    log_debug("device[%u].maxClockFrequency        = %u", deviceIndex, maxClockFrequency);
    log_debug("device[%u].maxComputeUnits          = %u", deviceIndex, maxComputeUnits);
    log_debug("device[%u].maxConstantArgs          = %u", deviceIndex, maxConstantArgs);
    log_debug("device[%u].maxConstantBufferSize    = %lu", deviceIndex, maxConstantBufferSize);
    log_debug("device[%u].maxMemAllocSize          = %lu", deviceIndex, maxMemAllocSize);
    log_debug("device[%u].maxParameterSize         = %u", deviceIndex, (uint)maxParameterSize);
    log_debug("device[%u].maxReadImageArgs         = %u", deviceIndex, maxReadImageArgs);
    log_debug("device[%u].maxWriteImageArgs        = %u", deviceIndex, maxWriteImageArgs);
    log_debug("device[%u].maxSamplers              = %u", deviceIndex, maxSamplers);
    log_debug("device[%u].maxWorkGroupSize         = %u", deviceIndex, (uint)maxWorkGroupSize);
    log_debug("device[%u].maxWorkItemDim           = %u", deviceIndex, maxWorkItemDim);
    log_debug("device[%u].maxWorkItemSizes         = %s", deviceIndex, sizeArrayString(maxWorkItemSizes).c_str());
    log_debug("device[%u].memBaseAddrAlign         = %u", deviceIndex, memBaseAddrAlign);
    log_debug("device[%u].minDataAlignSize         = %u", deviceIndex, minDataAlignSize);
    log_debug("device[%u].prefVecWidthChar         = %u", deviceIndex, prefVecWidthChar);
    log_debug("device[%u].prefVecWidthShort        = %u", deviceIndex, prefVecWidthShort);
    log_debug("device[%u].prefVecWidthInt          = %u", deviceIndex, prefVecWidthInt);
    log_debug("device[%u].prefVecWidthLong         = %u", deviceIndex, prefVecWidthLong);
    log_debug("device[%u].prefVecWidthFloat        = %u", deviceIndex, prefVecWidthFloat);
    log_debug("device[%u].prefVecWidthDouble       = %u", deviceIndex, prefVecWidthDouble);
    log_debug("device[%u].profilingTimerResolution = %u", deviceIndex, (uint)profilingTimerResolution);
}


/* opencl_context */

void opencl_context::notify(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
    log_error("%s:%s %s", class_name, __func__, errinfo);
}

opencl_context::opencl_context(opencl_device_list devices, bool request_gl_sharing) : devices(devices)
{
 	cl_int ret;
    
    bool hasSharing = false;
    cl_device_id *devices_arr = new cl_device_id[devices.size()];
    opencl_platform_ptr platform;
    for (size_t i = 0; i < devices.size(); i++) {
        if (!platform) platform = devices[i]->platform;
        hasSharing |= (devices[i]->hasExtension("cl_apple_gl_sharing") || devices[i]->hasExtension("cl_khr_gl_sharing"));
        devices_arr[i] = devices[i]->deviceId;
    }
    
    if (hasSharing && request_gl_sharing)
    {
        #if defined (EGL_VERSION)
            cl_context_properties properties[] = {
                CL_GL_CONTEXT_KHR, (cl_context_properties) eglGetCurrentContext(),
                CL_EGL_DISPLAY_KHR, (cl_context_properties) eglGetCurrentDisplay(),
                CL_CONTEXT_PLATFORM, (cl_context_properties) platform->platform_id,
                NULL
            };
        #elif defined (__APPLE__)
            CGLContextObj glContext = CGLGetCurrentContext();
            CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);
            cl_context_properties properties[] = {
                CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
                (cl_context_properties)shareGroup,
                NULL
            };
        #elif defined(__linux__)
#if 0
            cl_context_properties properties[] = {
                CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
                CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
                CL_CONTEXT_PLATFORM, (cl_context_properties) platform->platform_id,
                NULL
            };
#else
        cl_context_properties properties[] = { NULL };
#endif
        #elif defined(_WIN32)
            cl_context_properties properties[] = {
                CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
                CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
                CL_CONTEXT_PLATFORM, (cl_context_properties) platform->platform_id,
                NULL
            };
        #else
            cl_context_properties properties[] = { NULL };
        #endif
        log_debug("%s Enabling OpenGL Sharing", __func__);
        
#if 0
        // Find CL capable devices in the current GL context (required for WGL and GLX)
        cl_device_id devices[32];
        size_t size;
        clGetGLContextInfoKHR(properties, CL_DEVICES_FOR_GL_CONTEXT_KHR, 32 * sizeof(cl_device_id), devices, &size);
#endif
        
        clContext = clCreateContext(properties, (cl_uint)devices.size(), devices_arr, &notify, this, &ret);
        glSharing = true;
    } else {
        clContext = clCreateContext(NULL, (cl_uint)devices.size(), devices_arr, &notify, this, &ret);
        glSharing = false;
    }
    delete [] devices_arr;
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clCreateContext failed: ret=%d", class_name, __func__, ret);
    }
}

opencl_context::~opencl_context()
{
    clReleaseContext(clContext);
}

opencl_program_ptr opencl_context::createProgram(std::string src, std::string options)
{
    std::string cacheKey = options + std::string(",") + src;
    std::map<std::string,opencl_program_ptr>::iterator cacheEnt = programCache.find(cacheKey);
    if (cacheEnt != programCache.end()) {
        return (*cacheEnt).second;
    } else {
        opencl_program_ptr program(new opencl_program(this, src, options));
        programCache.insert(std::pair<std::string,opencl_program_ptr>(cacheKey, program));
        return program;
    }
}

opencl_command_queue_ptr opencl_context::createCommandQueue(opencl_device_ptr device, cl_command_queue_properties properties)
{
    opencl_command_queue_ptr cmdqueue(new opencl_command_queue(this, device, properties));
    cmdqueues.push_back(cmdqueue);
    return cmdqueue;
}

opencl_buffer_ptr opencl_context::createBuffer(cl_mem_flags flags, size_t size, void *host_ptr)
{
    return opencl_buffer_ptr(new opencl_buffer(this, flags, size, host_ptr));
}

opencl_buffer_ptr opencl_context::createBufferFromGLBuffer(cl_mem_flags flags, uint glbuffer)
{
 	cl_int ret;
    cl_mem clBuffer = clCreateFromGLBuffer(clContext, flags, glbuffer, &ret);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clCreateFromGLBuffer failed: ret=%d", class_name, __func__, ret);
        return opencl_buffer_ptr();
    } else {
        return opencl_buffer_ptr(new opencl_buffer(clBuffer));
    }
}


/* opencl_program */

opencl_program::opencl_program(opencl_context *context, std::string src, std::string options) : context(context), src(src), options(options)
{
    FILE *src_file = nullptr;
    char* src_buf = nullptr;
    struct stat src_statbuf;
    
    // get file length
    if (stat(src.c_str(), &src_statbuf) < 0) {
        log_error("%s:%s failed to stat %s: %s\n", class_name, __func__, src.c_str(), strerror(errno));
        return;
    }
    
    // open input file
    if ((src_file = fopen(src.c_str(), "r")) == NULL) {
        log_error("%s:%s failed to open %s: %s\n", class_name, __func__, src.c_str(), strerror(errno));
        return;
    }
    
    size_t len, src_size = src_statbuf.st_size, src_offset = 0;
    src_buf = new char[src_size];
    if (!src_buf) {
        log_error("%s:%s source buffer allocation failed\n", class_name);
        return;
    }
    while((len = fread(src_buf + src_offset, 1, src_size - src_offset, src_file)) > 0) {
        src_offset += len;
    }
    
    const char* src_strings[] = { (const char*)src_buf };
    const size_t src_lengths[] = { (size_t)src_size };
    
 	cl_int ret;
    clProgram = clCreateProgramWithSource(context->clContext, 1, src_strings, src_lengths, &ret);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clCreateProgramWithSource failed: ret=%d", class_name, __func__, ret);
        delete [] src_buf;
        return;
    } else {
        log_debug("%s:%s created program: %s", class_name, __func__, src.c_str());
    }
    delete [] src_buf;

    cl_device_id *devices_arr = new cl_device_id[context->devices.size()];
    for (size_t i = 0; i < context->devices.size(); i++) {
        devices_arr[i] = context->devices[i]->deviceId;
    }

    ret = clBuildProgram(clProgram, (cl_uint)context->devices.size(), devices_arr, options.c_str(), NULL, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clBuildProgram failed: ret=%d", class_name, __func__, ret);
    } else {
        log_debug("%s:%s built program: %s", class_name, __func__, src.c_str());
    }

    delete [] devices_arr;

    for (size_t i = 0; i < context->devices.size(); i++) {
        size_t build_log_size;
        ret = clGetProgramBuildInfo(clProgram, context->devices[i]->deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
        if (ret != CL_SUCCESS) {
            log_error("%s:%s clGetProgramBuildInfo failed: ret=%d", class_name, __func__, ret);
            continue;
        }
        char *build_log = new char[build_log_size + 1];
        build_log[build_log_size] = '\0';
        ret = clGetProgramBuildInfo(clProgram, context->devices[i]->deviceId, CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
        if (ret != CL_SUCCESS) {
            log_error("%s:%s clGetProgramBuildInfo failed: ret=%d", class_name, __func__, ret);
        } else {
            log_debug("%s:%s device[%u] program build log = [%s]", class_name, __func__, context->devices[i]->deviceIndex, build_log);
        }
        delete [] build_log;
    }
    
    cl_uint  num_kernels;
    ret = clCreateKernelsInProgram(clProgram, 0, NULL, &num_kernels);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clCreateKernelsInProgram failed: ret=%d", class_name, __func__, ret);
        return;
    }
    
    cl_kernel *kernel_arr = new cl_kernel[num_kernels];
    ret = clCreateKernelsInProgram(clProgram, num_kernels, kernel_arr, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clCreateKernelsInProgram failed: ret=%d", class_name, __func__, ret);
        delete [] kernel_arr;
        return;
    }
    for (size_t i = 0; i < num_kernels; i++) {
        size_t kernel_name_size;
        ret = clGetKernelInfo(kernel_arr[i], CL_KERNEL_FUNCTION_NAME, 0, NULL, &kernel_name_size);
        if (ret != CL_SUCCESS) {
            log_error("%s:%s clGetKernelInfo failed: ret=%d", class_name, __func__, ret);
            continue;
        }
        char *kernel_name = new char[kernel_name_size + 1];
        kernel_name[kernel_name_size] = '\0';
        ret = clGetKernelInfo(kernel_arr[i], CL_KERNEL_FUNCTION_NAME, kernel_name_size, kernel_name, NULL);
        if (ret != CL_SUCCESS) {
            log_error("%s:%s clGetKernelInfo failed: ret=%d", class_name, __func__, ret);
        } else {
            log_debug("%s:%s created kernel: %s", class_name, __func__, kernel_name);
            opencl_kernel_ptr kernel(new opencl_kernel(this, kernel_arr[i], kernel_name));
            kernels.insert(std::pair<std::string,opencl_kernel_ptr>(kernel_name, kernel));
        }
        delete [] kernel_name;
    }
    delete [] kernel_arr;
}

opencl_program::~opencl_program()
{
    log_debug("%s:%s released program: %s", class_name, __func__, src.c_str());
    clReleaseProgram(clProgram);
}

opencl_kernel_ptr opencl_program::getKernel(std::string name)
{
    opencl_kernel_map::iterator ki = kernels.find(name);
    if (ki != kernels.end()) {
        return (*ki).second;
    } else {
        return opencl_kernel_ptr();
    }
}


/* opencl_kernel */

opencl_kernel::opencl_kernel(opencl_program *program, cl_kernel clKernel, std::string name) : program(program), clKernel(clKernel), name(name)
{
}

opencl_kernel::~opencl_kernel()
{
    clReleaseKernel(clKernel);
}

void opencl_kernel::setArgLocalMemory(cl_uint arg_index, size_t size)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, size, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clSetKernelArg failed: ret=%d", class_name, __func__, ret);
    }
}

void opencl_kernel::setArg(cl_uint arg_index, size_t size, void* param)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, size, param);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clSetKernelArg failed: ret=%d", class_name, __func__, ret);
    }
}

void opencl_kernel::setArg(cl_uint arg_index, cl_mem memval)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_mem), &memval);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clSetKernelArg failed: ret=%d", class_name, __func__, ret);
    }
}

void opencl_kernel::setArg(cl_uint arg_index, cl_int intval)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_int), &intval);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clSetKernelArg failed: ret=%d", class_name, __func__, ret);
    }
}

void opencl_kernel::setArg(cl_uint arg_index, cl_float floatval)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_float), &floatval);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clSetKernelArg failed: ret=%d", class_name, __func__, ret);
    }
}

void opencl_kernel::setArg(cl_uint arg_index, cl_double doubleval)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_double), &doubleval);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clSetKernelArg failed: ret=%d", class_name, __func__, ret);
    }
}

void opencl_kernel::setArg(cl_uint arg_index, opencl_buffer_ptr &buffer)
{
    cl_int ret = clSetKernelArg(clKernel, arg_index, sizeof(cl_mem), &buffer->clBuffer);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clSetKernelArg failed: ret=%d", class_name, __func__, ret);
    }
}


/* opencl_buffer */

opencl_buffer::opencl_buffer(cl_mem clBuffer) : clBuffer(clBuffer) {}

opencl_buffer::opencl_buffer(opencl_context *context, cl_mem_flags flags, size_t size, void *host_ptr)
{
    cl_int ret;
    clBuffer = clCreateBuffer(context->clContext, flags, size, host_ptr, &ret);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clCreateBuffer failed: ret=%d", class_name, __func__, ret);
    }
}

opencl_buffer::~opencl_buffer()
{
    clReleaseMemObject(clBuffer);
}

cl_mem_object_type opencl_buffer::getType()
{
    cl_mem_object_type type;
    cl_int ret = clGetMemObjectInfo (clBuffer, CL_MEM_SIZE, sizeof(cl_mem_object_type), &type, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetMemObjectInfo failed: ret=%d", class_name, __func__, ret);
    }
    return type;
}

cl_mem_flags opencl_buffer::getFlags()
{
    cl_mem_flags flags;
    cl_int ret = clGetMemObjectInfo (clBuffer, CL_MEM_SIZE, sizeof(cl_mem_flags), &flags, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetMemObjectInfo failed: ret=%d", class_name, __func__, ret);
    }
    return flags;
}

size_t opencl_buffer::getSize()
{
    size_t size;
    cl_int ret = clGetMemObjectInfo (clBuffer, CL_MEM_SIZE, sizeof(size_t), &size, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetMemObjectInfo failed: ret=%d", class_name, __func__, ret);
    }
    return size;
}

/* openclProfilingInfo */

openclProfilingInfo::openclProfilingInfo(cl_ulong queued, cl_ulong submit, cl_ulong start, cl_ulong end)
    : queued(queued), submit(submit), start(start), end(end) {}

std::string openclProfilingInfo::toString()
{
    std::stringstream ss;
    ss << "totaltime = " << std::left << std::setw(9) << std::setfill(' ') << (end - queued)
        << " ns, queuedtime = " << std::left << std::setw(9) << std::setfill(' ') << (start - queued)
        << " ns, runtime = " << std::left << std::setw(9) << std::setfill(' ') << (end - start) << " ns";
    return ss.str();
}


/* opencl_event */

opencl_event::opencl_event(cl_event evt) : evt(evt) {}

opencl_event::~opencl_event()
{
    clReleaseEvent(evt);
}

void opencl_event::wait()
{
    cl_int ret = clWaitForEvents(1, &evt);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clWaitForEvents failed: ret=%d", class_name, __func__, ret);
    }
}

openclProfilingInfo opencl_event::getProfilingInfo()
{
    cl_ulong queued = 0, submit = 0, start = 0, end = 0;
    clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &queued, NULL);
    clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &submit, NULL);
    clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    return openclProfilingInfo(queued, submit, start, end);
}

cl_int opencl_event::getExecutionStatus()
{
    cl_int eventInfo;
    cl_int ret = clGetEventInfo(evt, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &eventInfo, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetEventInfo failed: ret=%d", class_name, __func__, ret);
    }
    return eventInfo;
}


/* opencl_command_queue */

opencl_command_queue::opencl_command_queue(opencl_context *context, opencl_device_ptr device, cl_command_queue_properties properties)
    : context(context), device(device), properties(properties), printProfilingInfo(false)
{
 	cl_int ret;
    clCommandQueue = clCreateCommandQueue(context->clContext, device->deviceId, properties, &ret);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clCreateCommandQueue failed: ret=%d", class_name, __func__, ret);
    }
}

opencl_command_queue::~opencl_command_queue()
{
    log_debug("%s:%s released command queue", class_name, __func__);
    clReleaseCommandQueue(clCommandQueue);
}

void opencl_command_queue::finish()
{
    clFinish(clCommandQueue);
}

opencl_event_ptr opencl_command_queue::enqueueTask(opencl_kernel_ptr &kernel,
                                                   opencl_event_list eventWait_list)
{
    cl_event *wait_list = NULL;
    if (eventWait_list.size()) {
        wait_list = new cl_event[eventWait_list.size()];
        for (size_t i = 0; i < eventWait_list.size(); i++) {
            wait_list[i] = eventWait_list.at(i)->evt;
        }
    }

    cl_event evt;
    cl_int ret = clEnqueueTask(clCommandQueue, kernel->clKernel, (cl_uint)eventWait_list.size(), wait_list, &evt);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clEnqueueTask failed: ret=%d", class_name, __func__, ret);
        return opencl_event_ptr();
    } else {
        opencl_event *event = new opencl_event(evt);
        if (printProfilingInfo) {
            event->wait();
            log_debug("%-45s task   %-30s : %s", __func__, kernel->name.c_str(), event->getProfilingInfo().toString().c_str());
        }
        return opencl_event_ptr(event);
    }
}

opencl_event_ptr opencl_command_queue::enqueueNDRangeKernel(opencl_kernel_ptr &kernel,
                                                            const opencl_dim &globalWorkSize,
                                                            opencl_event_list eventWait_list)
{
    return enqueueNDRangeKernel(kernel, globalWorkSize, opencl_dim(), eventWait_list);
}

opencl_event_ptr opencl_command_queue::enqueueNDRangeKernel(opencl_kernel_ptr &kernel,
                                                            const opencl_dim &globalWorkSize,
                                                            const opencl_dim &localWorkSize,
                                                            opencl_event_list eventWait_list)
{
    cl_event evt;
    size_t *global_work_size = NULL, *local_work_size = NULL;
    cl_event *wait_list = NULL;
    
    global_work_size= new size_t[globalWorkSize.size()];
    for (size_t i = 0; i < globalWorkSize.size(); i++) {
        global_work_size[i] = globalWorkSize[i];
    }
    
    if (localWorkSize.size() > 0) {
        local_work_size = new size_t[localWorkSize.size()];
        for (size_t i = 0; i < localWorkSize.size(); i++) {
            local_work_size[i] = localWorkSize[i];
        }
    }

    if (eventWait_list.size()) {
        wait_list = new cl_event[eventWait_list.size()];
        for (size_t i = 0; i < eventWait_list.size(); i++) {
            wait_list[i] = eventWait_list.at(i)->evt;
        }
    }
    
    cl_int ret = clEnqueueNDRangeKernel(clCommandQueue, kernel->clKernel, (cl_uint)globalWorkSize.size(),
                                        NULL, global_work_size, local_work_size, (cl_uint)eventWait_list.size(), wait_list, &evt);
    
    delete [] global_work_size;
    if (local_work_size) {
        delete [] local_work_size;
    }
    if (wait_list) {
        delete [] wait_list;
    }
    
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clEnqueueNDRangeKernel failed: ret=%d", class_name, __func__, ret);
        return opencl_event_ptr();
    } else {
        opencl_event *event = new opencl_event(evt);
        if (printProfilingInfo) {
            event->wait();
            log_debug("%-45s kernel %-30s : %s", __func__, kernel->name.c_str(), event->getProfilingInfo().toString().c_str());
        }
        return opencl_event_ptr(event);
    }
}

opencl_event_ptr opencl_command_queue::enqueueReadBuffer(opencl_buffer_ptr &buffer, cl_bool blocking_read, size_t offset, size_t cb, void *ptr,
                                                         opencl_event_list eventWait_list)
{
    cl_event evt;
    cl_event *wait_list = NULL;

    if (eventWait_list.size()) {
        wait_list = new cl_event[eventWait_list.size()];
        for (size_t i = 0; i < eventWait_list.size(); i++) {
            wait_list[i] = eventWait_list.at(i)->evt;
        }
    }

    cl_int ret = clEnqueueReadBuffer(clCommandQueue, buffer->clBuffer, blocking_read, offset, cb, ptr, (cl_uint)eventWait_list.size(), wait_list, &evt);

    if (wait_list) {
        delete [] wait_list;
    }

    if (ret != CL_SUCCESS) {
        log_error("%s:%s clEnqueueReadBuffer failed: ret=%d", class_name, __func__, ret);
        return opencl_event_ptr();
    } else {
        opencl_event *event = new opencl_event(evt);
        if (printProfilingInfo) {
            event->wait();
            log_debug("%-83s : %s", __func__, event->getProfilingInfo().toString().c_str());
        }
        return opencl_event_ptr(event);
    }
}

opencl_event_ptr opencl_command_queue::enqueueWriteBuffer(opencl_buffer_ptr &buffer, cl_bool blocking_write, size_t offset, size_t cb, const void *ptr,
                                                          opencl_event_list eventWait_list)
{
    cl_event evt;
    cl_event *wait_list = NULL;

    if (eventWait_list.size()) {
        wait_list = new cl_event[eventWait_list.size()];
        for (size_t i = 0; i < eventWait_list.size(); i++) {
            wait_list[i] = eventWait_list.at(i)->evt;
        }
    }

    cl_int ret = clEnqueueWriteBuffer(clCommandQueue, buffer->clBuffer, blocking_write, offset, cb, ptr, (cl_uint)eventWait_list.size(), wait_list, &evt);

    if (wait_list) {
        delete [] wait_list;
    }

    if (ret != CL_SUCCESS) {
        log_error("%s:%s clEnqueueWriteBuffer failed: ret=%d", class_name, __func__, ret);
        return opencl_event_ptr();
    } else {
        opencl_event *event = new opencl_event(evt);
        if (printProfilingInfo) {
            event->wait();
            log_debug("%-83s : %s", __func__, event->getProfilingInfo().toString().c_str());
        }
        return opencl_event_ptr(event);
    }
}

opencl_event_ptr opencl_command_queue::enqueueAcquireGLObjects(opencl_buffer_list buffer_list, opencl_event_list eventWait_list)
{
    cl_event evt;
    cl_mem *cl_buffer_list = NULL;
    cl_event *cl_wait_list = NULL;
    
    if (buffer_list.size()) {
        cl_buffer_list = new cl_mem[buffer_list.size()];
        for (size_t i = 0; i < buffer_list.size(); i++) {
            cl_buffer_list[i] = buffer_list.at(i)->clBuffer;
        }
    }

    if (eventWait_list.size()) {
        cl_wait_list = new cl_event[eventWait_list.size()];
        for (size_t i = 0; i < eventWait_list.size(); i++) {
            cl_wait_list[i] = eventWait_list.at(i)->evt;
        }
    }
    
    cl_int ret = clEnqueueAcquireGLObjects(clCommandQueue, (cl_uint)buffer_list.size(), cl_buffer_list, (cl_uint)eventWait_list.size(), cl_wait_list, &evt);
    
    if (cl_buffer_list) {
        delete [] cl_buffer_list;
    }
    
    if (cl_wait_list) {
        delete [] cl_wait_list;
    }

    if (ret != CL_SUCCESS) {
        log_error("%s:%s clEnqueueAcquireGLObjects failed: ret=%d", class_name, __func__, ret);
        return opencl_event_ptr();
    } else {
        opencl_event *event = new opencl_event(evt);
        if (printProfilingInfo) {
            event->wait();
            log_debug("%-83s : %s", __func__, event->getProfilingInfo().toString().c_str());
        }
        return opencl_event_ptr(event);
    }
}

opencl_event_ptr opencl_command_queue::enqueueReleaseGLObjects(opencl_buffer_list buffer_list, opencl_event_list eventWait_list)
{
    cl_event evt;
    cl_mem *cl_buffer_list = NULL;
    cl_event *cl_wait_list = NULL;
    
    if (buffer_list.size()) {
        cl_buffer_list = new cl_mem[buffer_list.size()];
        for (size_t i = 0; i < buffer_list.size(); i++) {
            cl_buffer_list[i] = buffer_list.at(i)->clBuffer;
        }
    }
    
    if (eventWait_list.size()) {
        cl_wait_list = new cl_event[eventWait_list.size()];
        for (size_t i = 0; i < eventWait_list.size(); i++) {
            cl_wait_list[i] = eventWait_list.at(i)->evt;
        }
    }
    
    cl_int ret = clEnqueueReleaseGLObjects(clCommandQueue, (cl_uint)buffer_list.size(), cl_buffer_list, (cl_uint)eventWait_list.size(), cl_wait_list, &evt);
    
    if (cl_buffer_list) {
        delete [] cl_buffer_list;
    }
    
    if (cl_wait_list) {
        delete [] cl_wait_list;
    }
    
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clEnqueueReleaseGLObjects failed: ret=%d", class_name, __func__, ret);
        return opencl_event_ptr();
    } else {
        opencl_event *event = new opencl_event(evt);
        if (printProfilingInfo) {
            event->wait();
            log_debug("%-83s : %s", __func__, event->getProfilingInfo().toString().c_str());
        }
        return opencl_event_ptr(event);
    }
}


/* opencl */

opencl::opencl() : platforms()
{
    if (!init()) {
        log_error("%s:%s initialization failed", class_name, __func__);
    }
}

opencl::~opencl() {}

bool opencl::getPlatformIDs(cl_platform_id* &platforms, cl_uint &num_platforms)
{
    cl_int ret;
    
    // find number of platforms
    ret = clGetPlatformIDs(0, NULL, &num_platforms);
	if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetPlatformIDs failed: ret=%d", class_name, __func__, ret);
        return false;
    }
	log_debug("%s:%s found %d platforms", class_name, __func__, num_platforms);
    
    // get platform ids
	platforms = new cl_platform_id[num_platforms];
	ret = clGetPlatformIDs(num_platforms, platforms, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetPlatformIDs failed: ret=%d", class_name, __func__, ret);
        delete [] platforms;
        platforms = NULL;
        return false;
    }
    
    return true;
}

bool opencl::getPlatformInfo(cl_platform_id platform, cl_platform_info param_name, std::string &info)
{
    cl_int ret;
    size_t param_value_size;
    char *param_value;
    
    // get platform info size
    ret = clGetPlatformInfo(platform, param_name, 0, NULL, &param_value_size);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetPlatformInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }
    
    // get platform info
    param_value = new char[param_value_size+1];
    ret = clGetPlatformInfo(platform, param_name, param_value_size, (void*)param_value, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetPlatformInfo failed: ret=%d", class_name, __func__, ret);
        delete [] param_value;
        return false;
    }
    
    // null terminate and copy to string
    param_value[param_value_size] = '\0';
    info = param_value;
    delete [] param_value;
    
    return true;
}

bool opencl::getDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_device_id* &devices, cl_uint &num_devices)
{
    cl_int ret;
    
    // find number of devices
    ret = clGetDeviceIDs(platform, device_type, 0, NULL, &num_devices);
	if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceIDs failed: ret=%d", class_name, __func__, ret);
        return false;
    }
	log_debug("%s:%s found %d devices", class_name, __func__, num_devices);
    
    // get device ids
	devices = new cl_device_id[num_devices];
	ret = clGetDeviceIDs(platform, device_type, num_devices, devices, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceIDs failed: ret=%d", class_name, __func__, ret);
        delete [] devices;
        devices = NULL;
        return false;
    }
    
    return true;
}

bool opencl::getDeviceInfoType(cl_device_id device, cl_device_type &deviceType)
{
    cl_int ret = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), (void*)&deviceType, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }    
    return true;
}

bool opencl::getDeviceInfoMemCacheType(cl_device_id device, cl_device_mem_cache_type &memCacheType)
{
    cl_int ret = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(cl_device_mem_cache_type), (void*)&memCacheType, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }
    return true;
}

bool opencl::getDeviceInfoLocalMemType(cl_device_id device, cl_device_local_mem_type &localMemType)
{
    cl_int ret = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cl_device_local_mem_type), (void*)&localMemType, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }
    return true;
}

bool opencl::getDeviceInfoFPConfig(cl_device_id device, cl_device_info param_name, cl_device_fp_config &devicefpConfig)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(cl_device_fp_config), (void*)&devicefpConfig, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }    
    return true;
}

bool opencl::getDeviceInfoBool(cl_device_id device, cl_device_info param_name, cl_bool &boolVal)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(cl_bool), (void*)&boolVal, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }
    return true;
}

bool opencl::getDeviceInfoUint(cl_device_id device, cl_device_info param_name, cl_uint &uintVal)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(cl_uint), (void*)&uintVal, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }    
    return true;
}

bool opencl::getDeviceInfoUlong(cl_device_id device, cl_device_info param_name, cl_ulong &ulongVal)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(cl_ulong), (void*)&ulongVal, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }    
    return true;
}

bool opencl::getDeviceInfoSize(cl_device_id device, cl_device_info param_name, size_t &sizeVal)
{
    cl_int ret = clGetDeviceInfo(device, param_name, sizeof(size_t), (void*)&sizeVal, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }
    return true;
}

bool opencl::getDeviceInfoSizeArray(cl_device_id device, cl_device_info param_name, std::vector<size_t> &sizeArray)
{
    cl_int ret;
    size_t param_value_size;
    size_t *param_value;
    
    // get device info string size
    ret = clGetDeviceInfo(device, param_name, 0, NULL, &param_value_size);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }
    
    // get device info string
    size_t nelem = param_value_size / sizeof(size_t);
    param_value = new size_t[nelem];
    ret = clGetDeviceInfo(device, param_name, param_value_size, (void*)param_value, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        delete [] param_value;
        return false;
    }
    
    // copy values to array
    for (size_t i = 0; i < nelem; i++) {
        sizeArray.push_back(param_value[i]);
    }
    delete [] param_value;

    return true;
}

bool opencl::getDeviceInfoString(cl_device_id device, cl_device_info param_name, std::string &info)
{
    cl_int ret;
    size_t param_value_size;
    char *param_value;
    
    // get device info string size
    ret = clGetDeviceInfo(device, param_name, 0, NULL, &param_value_size);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        return false;
    }
    
    // get device info string
    param_value = new char[param_value_size+1];
    ret = clGetDeviceInfo(device, param_name, param_value_size, (void*)param_value, NULL);
    if (ret != CL_SUCCESS) {
        log_error("%s:%s clGetDeviceInfo failed: ret=%d", class_name, __func__, ret);
        delete [] param_value;
        return false;
    }
    
    // null terminate and copy to string
    param_value[param_value_size] = '\0';
    info = param_value;
    delete [] param_value;
    
    return true;
}

bool opencl::init()
{
    cl_uint num_platforms;
    cl_uint num_devices;
	cl_platform_id *cl_platform_arr;
    cl_device_id *cl_device_arr;
    
    // read platform ids
    if (!getPlatformIDs(cl_platform_arr, num_platforms)) return false;
    
    // read platform info
    for (uint platform_index = 0; platform_index < num_platforms; platform_index++) {
        opencl_platform_ptr platform(new opencl_platform(cl_platform_arr[platform_index], platform_index));
        platforms.push_back(platform);
        
        // read device ids
        if (!getDeviceIDs(cl_platform_arr[platform_index], CL_DEVICE_TYPE_ALL, cl_device_arr, num_devices)) goto fail_platforms;
        
        // read device info
        for (uint deviceIndex = 0; deviceIndex < num_devices; deviceIndex++) {
            opencl_device_ptr device(new opencl_device(platform, cl_device_arr[deviceIndex], deviceIndex));
            devices.push_back(device);
        }
        
        delete [] cl_device_arr;
    }
    
    delete [] cl_platform_arr;
    return true;
    
fail_platforms:
    delete [] cl_platform_arr;
    return false;
}

opencl_device_list opencl::getDevices(std::string device_match)
{
    opencl_device_list device_list;
    opencl_platform_ptr platform;
    for (opencl_device_list::iterator di = devices.begin(); di != devices.end(); di++) {
        opencl_device_ptr device = *di;
        if (!device->available) continue;
        if (platform && device->platform != platform) continue; // make sure all devices are on the same platform
        if (device_match == "any") {
            device_list.push_back(device);
            break;
        } else if (device_match == "gpu" && device->deviceType == CL_DEVICE_TYPE_GPU) {
            device_list.push_back(device);
        } else if (device_match == "cpu" && device->deviceType == CL_DEVICE_TYPE_CPU) {
            device_list.push_back(device);
        } else if (device_match == "all") {
            platform = device->platform;
            device_list.push_back(device);
        } else if (device_match == device->name) {
            platform = device->platform;
            device_list.push_back(device);
        }
    }
    return device_list;
}

opencl_context_ptr opencl::createContext(std::string device_match, bool request_gl_sharing)
{
    return createContext(getDevices(device_match), request_gl_sharing);
}

opencl_context_ptr opencl::createContext(opencl_device_list device_list, bool request_gl_sharing)
{
    if (device_list.size() == 0) {
        log_error("%s:%s no OpenCL devices present", class_name, __func__);
        return opencl_context_ptr();
    }
    opencl_context_ptr context(new opencl_context(device_list, request_gl_sharing));
    contexts.push_back(context);
    return context;
}
