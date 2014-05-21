/*
 *  opencl.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef opencl_h
#define opencl_h

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <OpenGL/OpenGL.h>
#else
#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <CL/cl_gl.h>
#endif


class opencl;
typedef std::shared_ptr<opencl> opencl_ptr;
class opencl_platform;
typedef std::shared_ptr<opencl_platform> opencl_platform_ptr;
typedef std::vector<opencl_platform_ptr> opencl_platform_list;
class opencl_device;
typedef std::shared_ptr<opencl_device> opencl_device_ptr;
typedef std::vector<opencl_device_ptr> opencl_device_list;
class opencl_context;
typedef std::shared_ptr<opencl_context> opencl_context_ptr;
typedef std::vector<opencl_context_ptr> opencl_context_list;
class opencl_program;
typedef std::shared_ptr<opencl_program> opencl_program_ptr;
typedef std::vector<opencl_program_ptr> opencl_program_list;
class opencl_buffer;
typedef std::shared_ptr<opencl_buffer> opencl_buffer_ptr;
class opencl_kernel;
typedef std::shared_ptr<opencl_kernel> opencl_kernel_ptr;
typedef std::map<std::string,opencl_kernel_ptr> opencl_kernel_map;
class opencl_event;
typedef std::shared_ptr<opencl_event> opencl_event_ptr;
class opencl_command_queue;
typedef std::shared_ptr<opencl_command_queue> opencl_command_queue_ptr;
typedef std::vector<opencl_command_queue_ptr> opencl_command_queue_list;


/* opencl_platform */

class opencl_platform
{
protected:
    friend class opencl;
    friend class opencl_context;
    
    std::set<std::string> extension_set;

    cl_platform_id platform_id;
    cl_uint platform_index;
    std::string name;
    std::string vendor;
    std::string version;
    std::string profile;
    std::string extensions;

    opencl_platform(cl_platform_id platform_id, cl_uint platform_index);

public:
    virtual ~opencl_platform();
    
    void parseExtensions();
    bool hasExtension(std::string extname);

    cl_platform_id getPlatformId() { return platform_id; }
    cl_uint getPlatformIndex() { return platform_index; }
    std::string getName() { return name; }
    std::string getVendor() { return vendor; }
    std::string getVersion() { return version; }
    std::string getProfile() { return profile; }
    std::string getExtensions() { return extensions; }

    void print();
};


/* opencl_device */

class opencl_device
{
protected:
    friend class opencl;
    friend class opencl_platform;
    friend class opencl_context;
    friend class opencl_program;
    friend class opencl_command_queue;
    
    std::set<std::string> extension_set;
    
    opencl_platform_ptr platform;
    cl_device_id deviceId;
    cl_uint deviceIndex;
    cl_device_type deviceType;
    std::string name;
    std::string vendor;
    std::string deviceVersion;
    std::string driverVersion;
    std::string profile;
    std::string extensions;
    cl_bool available;
    cl_uint addressBits;
    cl_bool littleEndian;
    cl_device_fp_config floatSingle;
    cl_device_fp_config floatDouble;
    cl_ulong globalMemCacheSize;
    cl_device_mem_cache_type globalMemCacheType;
    cl_uint globalMemCacheLineSize;
    cl_ulong globalMemSize;
    cl_bool imageSupport;
    size_t image2DmaxWidth;
    size_t image2DmaxHeight;
    size_t image3DmaxWidth;
    size_t image3DmaxHeight;
    size_t image3DmaxDepth;
    cl_ulong localMemSize;
    cl_device_local_mem_type localMemType;
    cl_uint maxClockFrequency;
    cl_uint maxComputeUnits;
    cl_uint maxConstantArgs;
    cl_ulong maxConstantBufferSize;
    cl_ulong maxMemAllocSize;
    size_t maxParameterSize;
    cl_uint maxReadImageArgs;
    cl_uint maxWriteImageArgs;
    cl_uint maxSamplers;
    size_t maxWorkGroupSize;
    cl_uint maxWorkItemDim;
    std::vector<size_t> maxWorkItemSizes;
    cl_uint memBaseAddrAlign;
    cl_uint minDataAlignSize;
    cl_uint prefVecWidthChar;
    cl_uint prefVecWidthShort;
    cl_uint prefVecWidthInt;
    cl_uint prefVecWidthLong;
    cl_uint prefVecWidthFloat;
    cl_uint prefVecWidthDouble;
    size_t profilingTimerResolution;

    opencl_device(opencl_platform_ptr platform, cl_device_id deviceId, cl_uint deviceIndex);

public:
    virtual ~opencl_device();
    
    void parseExtensions();
    bool hasExtension(std::string extname);

    std::string deviceTypeString();
    std::string globalMemCacheTypeString();
    std::string localMemTypeString();
    std::string fpconfigString(cl_device_fp_config fpConfig);
    std::string sizeArrayString(std::vector<size_t> &sizeArray);
    
    opencl_platform_ptr getPlatform() { return platform; }
    cl_device_id getDeviceId() { return deviceId; }
    cl_uint getDeviceIndex() { return deviceIndex; }
    cl_device_type getDeviceType() { return deviceType; }
    std::string getName() { return name; }
    std::string getVendor() { return vendor; }
    std::string getDeviceVersion() { return deviceVersion; }
    std::string getDriverVersion() { return driverVersion; }
    std::string getProfile() { return profile; }
    std::string getExtensions() { return extensions; }
    cl_bool isAvailable() { return available; }
    cl_uint getAddressBits() { return addressBits; }
    cl_bool isLittleEndian() { return littleEndian; }
    cl_device_fp_config getFloatConfig() { return floatSingle; }
    cl_device_fp_config getDoubleConfig() { return floatDouble; }
    cl_ulong getGlobalMemCacheSize() { return globalMemCacheSize; }
    cl_device_mem_cache_type getGlobalMemCacheType() { return globalMemCacheType; }
    cl_uint getGlobalMemCacheLineSize() { return globalMemCacheLineSize; }
    cl_ulong getGlobalMemSize() { return globalMemSize; }
    cl_bool hasImageSupport() { return imageSupport; }
    size_t getImage2DmaxWidth() { return image2DmaxWidth; }
    size_t getImage2DmaxHeight() { return image2DmaxHeight; }
    size_t getImage3DmaxWidth() { return image3DmaxWidth; }
    size_t getImage3DmaxHeight() { return image3DmaxHeight; }
    size_t getImage3DmaxDepth() { return image3DmaxDepth; }
    cl_ulong getLocalMemSize() { return localMemSize; }
    cl_device_local_mem_type getLocalMemType() { return localMemType; }
    cl_uint getMaxClockFrequency() { return maxClockFrequency; }
    cl_uint getMaxComputeUnits() { return maxComputeUnits; }
    cl_uint getMaxConstantArgs() { return maxConstantArgs; }
    cl_ulong getMaxConstantBufferSize() { return maxConstantBufferSize; }
    cl_ulong getMaxMemAllocSize() { return maxMemAllocSize; }
    size_t getMaxParameterSize() { return maxParameterSize; }
    cl_uint getMaxReadImageArgs() { return maxReadImageArgs; }
    cl_uint getMaxWriteImageArgs() { return maxWriteImageArgs; }
    cl_uint getMaxSamplers() { return maxSamplers; }
    size_t getMaxWorkGroupSize() { return maxWorkGroupSize; }
    cl_uint getMaxWorkItemDim() { return maxWorkItemDim; }
    std::vector<size_t> getMaxWorkItemSizes() { return maxWorkItemSizes; }
    cl_uint getMemBaseAddrAlign() { return memBaseAddrAlign; }
    cl_uint getMinDataAlignSize() { return minDataAlignSize; }
    cl_uint getPrefVecWidthChar() { return prefVecWidthChar; }
    cl_uint getPrefVecWidthShort() { return prefVecWidthShort; }
    cl_uint getPrefVecWidthInt() { return prefVecWidthInt; }
    cl_uint getPrefVecWidthLong() { return prefVecWidthLong; }
    cl_uint getPrefVecWidthFloat() { return prefVecWidthFloat; }
    cl_uint getPrefVecWidthDouble() { return prefVecWidthDouble; }
    size_t getProfilingTimerResolution() { return profilingTimerResolution; }

    void print();
};


/* opencl_context */

class opencl_context
{
protected:
    friend class opencl;
    friend class opencl_program;
    friend class opencl_buffer;
    friend class opencl_command_queue;
    
    opencl_device_list devices;
    cl_context clContext;
    bool glSharing;
    opencl_command_queue_list cmdqueues;
    std::map<std::string,opencl_program_ptr> programCache;
    
#ifdef _WIN32
    static void WINAPI notify(const char *errinfo, const void *private_info, size_t cb, void *user_data);
#else
    static void notify(const char *errinfo, const void *private_info, size_t cb, void *user_data);
#endif

    opencl_context(opencl_device_list devices, bool request_gl_sharing);

public:
    virtual ~opencl_context();
    
    bool glSharingIsEnabled() { return glSharing; }
    cl_context getContext() { return clContext; }
    opencl_device_list& getDevices() { return devices; }
    opencl_program_ptr createProgram(std::string src, std::string options = "");
    opencl_command_queue_ptr createCommandQueue(opencl_device_ptr device, cl_command_queue_properties properties = 0);
    opencl_buffer_ptr createBuffer(cl_mem_flags flags, size_t size, void *host_ptr);
    opencl_buffer_ptr createBufferFromGLBuffer(cl_mem_flags flags, cl_uint glbuffer);
};


/* opencl_program */

class opencl_program
{
protected:
    friend class opencl_context;
    
    opencl_context *context;
    std::string src;
    std::string options;
    cl_program clProgram;
    opencl_kernel_map kernels;

    opencl_program(opencl_context *context, std::string src, std::string options);

public:
    virtual ~opencl_program();

    opencl_kernel_ptr getKernel(std::string name);
};


/* opencl_kernel */

class opencl_kernel
{
protected:
    friend class opencl_program;
    friend class opencl_command_queue;
    
    opencl_program *program;
    cl_kernel clKernel;
    std::string name;

    opencl_kernel(opencl_program *program, cl_kernel clKernel, std::string name);

public:
    virtual ~opencl_kernel();
    
    void setArgLocalMemory(cl_uint arg_index, size_t size);
    void setArg(cl_uint arg_index, size_t size, void* param);
    void setArg(cl_uint arg_index, cl_mem memval);
    void setArg(cl_uint arg_index, cl_int intval);
    void setArg(cl_uint arg_index, cl_float floatval);
    void setArg(cl_uint arg_index, cl_double doubleval);
    void setArg(cl_uint arg_index, opencl_buffer_ptr &buffer);
};


/* opencl_buffer */

class opencl_buffer
{
protected:
    friend class opencl_context;
    friend class opencl_kernel;
    friend class opencl_command_queue;
    
    opencl_context *context;
    cl_mem clBuffer;
    cl_mem_flags flags;
    size_t size;
  	void *host_ptr;
    
    opencl_buffer(cl_mem clBuffer);
    opencl_buffer(opencl_context *context, cl_mem_flags flags, size_t size, void *host_ptr);
    
public:
    virtual ~opencl_buffer();
    
    cl_mem_object_type getType();
    cl_mem_flags getFlags();
    size_t getSize();
};


/* opencl_buffer_list */

struct opencl_buffer_list : public std::vector<opencl_buffer_ptr>
{
    opencl_buffer_list() {}
    
    opencl_buffer_list(const opencl_buffer_ptr &buffer1)
    {
        push_back(buffer1);
    }
    
    opencl_buffer_list(const opencl_buffer_ptr &buffer1, const opencl_buffer_ptr &buffer2)
    {
        push_back(buffer1);
        push_back(buffer2);
    }

    opencl_buffer_list(const opencl_buffer_ptr &buffer1, const opencl_buffer_ptr &buffer2, const opencl_buffer_ptr &buffer3)
    {
        push_back(buffer1);
        push_back(buffer2);
        push_back(buffer3);
    }

    opencl_buffer_list(const opencl_buffer_ptr &buffer1, const opencl_buffer_ptr &buffer2, const opencl_buffer_ptr &buffer3, const opencl_buffer_ptr &buffer4)
    {
        push_back(buffer1);
        push_back(buffer2);
        push_back(buffer3);
        push_back(buffer4);
    }
};


/* openclProfilingInfo */

struct openclProfilingInfo
{
    cl_ulong queued;
    cl_ulong submit;
    cl_ulong start;
    cl_ulong end;
    
    openclProfilingInfo(cl_ulong queued, cl_ulong submit, cl_ulong start, cl_ulong end);
    
    std::string toString();
};


/* opencl_event */

class opencl_event
{
protected:
    friend class opencl_command_queue;
    
    cl_event evt;
    
    opencl_event(cl_event evt);

public:
    virtual ~opencl_event();
    
    void wait();
    
    openclProfilingInfo getProfilingInfo();
    cl_int getExecutionStatus();
};


/* opencl_event_list */

struct opencl_event_list : public std::vector<opencl_event_ptr>
{
    opencl_event_list() {}

    opencl_event_list(const opencl_event_ptr &event1)
    {
        push_back(event1);
    }

    opencl_event_list(const opencl_event_ptr &event1, const opencl_event_ptr &event2)
    {
        push_back(event1);
        push_back(event2);
    }
};


/* opencl_dim */

struct opencl_dim : public std::vector<size_t>
{
    opencl_dim() {}
    
    opencl_dim(const size_t dim1)
    {
        push_back(dim1);
    }
    
    opencl_dim(const size_t dim1, const size_t dim2)
    {
        push_back(dim1);
        push_back(dim2);
    }
};


/* opencl_command_queue */

class opencl_command_queue
{
protected:
    friend class opencl_context;
    
    opencl_context *context;
    opencl_device_ptr device;
    cl_command_queue_properties properties;
    cl_command_queue clCommandQueue;
    bool printProfilingInfo;

    opencl_command_queue(opencl_context *context, opencl_device_ptr device, cl_command_queue_properties properties);

public:
    virtual ~opencl_command_queue();
    
    void finish();
    opencl_device_ptr getDevice() { return device; }
    cl_command_queue getCommandQueue() { return clCommandQueue; }
    bool profilingIsEnabled() { return properties & CL_QUEUE_PROFILING_ENABLE; }
    void setPrintProfilingInfo(bool printProfilingInfo) { this->printProfilingInfo = printProfilingInfo; }
    opencl_event_ptr enqueueTask(opencl_kernel_ptr &kernel, opencl_event_list eventWait_list = opencl_event_list());
    opencl_event_ptr enqueueNDRangeKernel(opencl_kernel_ptr &kernel, const opencl_dim &globalWorkSize,
         const opencl_dim &localWorkSize, opencl_event_list eventWait_list = opencl_event_list());
    opencl_event_ptr enqueueNDRangeKernel(opencl_kernel_ptr &kernel, const opencl_dim &globalWorkSize,
        opencl_event_list eventWait_list = opencl_event_list());
    opencl_event_ptr enqueueReadBuffer(opencl_buffer_ptr &buffer, cl_bool blocking_read, size_t offset, size_t cb, void *ptr,
        opencl_event_list eventWait_list = opencl_event_list());
    opencl_event_ptr enqueueWriteBuffer(opencl_buffer_ptr &buffer, cl_bool blocking_write, size_t offset, size_t cb, const void *ptr,
        opencl_event_list eventWait_list = opencl_event_list());
    opencl_event_ptr enqueueAcquireGLObjects(opencl_buffer_list buffer_list, opencl_event_list eventWait_list = opencl_event_list());
    opencl_event_ptr enqueueReleaseGLObjects(opencl_buffer_list buffer_list, opencl_event_list eventWait_list = opencl_event_list());
};


/* opencl */

class opencl
{
protected:
    friend class opencl_platform;
    friend class opencl_device;
    friend class opencl_context;
    
    opencl_platform_list platforms;
    opencl_device_list devices;
    opencl_context_list contexts;

    static bool getPlatformIDs(cl_platform_id* &platforms, cl_uint &num_platforms);
    static bool getPlatformInfo(cl_platform_id platform, cl_platform_info param_name, std::string &info);
    static bool getDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_device_id* &devices, cl_uint &num_devices);
    static bool getDeviceInfoType(cl_device_id device, cl_device_type &deviceType);
    static bool getDeviceInfoMemCacheType(cl_device_id device, cl_device_mem_cache_type &memCacheType);
    static bool getDeviceInfoLocalMemType(cl_device_id device, cl_device_local_mem_type &localMemType);
    static bool getDeviceInfoFPConfig(cl_device_id device, cl_device_info param_name, cl_device_fp_config &devicefpConfig);
    static bool getDeviceInfoBool(cl_device_id device, cl_device_info param_name, cl_bool &boolVal);
    static bool getDeviceInfoUint(cl_device_id device, cl_device_info param_name, cl_uint &uintVal);
    static bool getDeviceInfoUlong(cl_device_id device, cl_device_info param_name, cl_ulong &ulongVal);
    static bool getDeviceInfoSize(cl_device_id device, cl_device_info param_name, size_t &sizeVal);
    static bool getDeviceInfoSizeArray(cl_device_id device, cl_device_info param_name, std::vector<size_t> &sizeArray);
    static bool getDeviceInfoString(cl_device_id device, cl_device_info param_name, std::string &info);
    
public:
    opencl();
    virtual ~opencl();
    
    bool init();
    opencl_platform_list& getPlatforms() { return platforms; }
    opencl_device_list& getDevices() { return devices; }
    opencl_context_list& getContexts() { return contexts; }
    opencl_device_list getDevices(std::string device_match);
    opencl_context_ptr createContext(std::string device_match, bool request_gl_sharing = false);
    opencl_context_ptr createContext(opencl_device_list device_list, bool request_gl_sharing = false);
};

#endif
