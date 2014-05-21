//
//  aes-opencl-test.cc
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <iomanip>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <chrono>

#include "aes.h"
#include "logging.h"
#include "opencl.h"

using namespace std::chrono;

/* aes_opencl_test */

struct aes_opencl_test
{
    opencl_ptr cl;
    opencl_device_list gpu_devices;
    opencl_device_ptr chosen_device;
    opencl_context_ptr clctx;
    opencl_command_queue_ptr clcmdqueue;
    
    void initCL()
    {
        // get gpu devices
        cl = opencl_ptr(new opencl());
        gpu_devices = cl->getDevices("gpu");
        if (gpu_devices.size() == 0) {
            log_error_exit("no OpenCL gpu devices found");
        }
        
        // find device with largest workgroup size
        size_t best_workgroupsize = 0;
        for (opencl_device_ptr device : gpu_devices) {
            if (device->getMaxWorkGroupSize() > best_workgroupsize) {
                best_workgroupsize = device->getMaxWorkGroupSize();
                chosen_device = device;
            }
        }
        log_debug("using device: %s", chosen_device->getName().c_str());
        chosen_device->print();
        
        // create context
        opencl_device_list use_devices{ chosen_device };
        clctx = cl->createContext(use_devices, false);
        clcmdqueue = clctx->createCommandQueue(chosen_device, 0);
    }
    
    void testCL()
    {
        int a[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        int b[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        int c[8];
        
        opencl_program_ptr testprog = clctx->createProgram("src/test.cl");
        
        opencl_buffer_ptr abuffer = clctx->createBuffer(CL_MEM_READ_ONLY, sizeof(a), NULL);
        opencl_buffer_ptr bbuffer = clctx->createBuffer(CL_MEM_READ_ONLY, sizeof(b), NULL);
        opencl_buffer_ptr cbuffer = clctx->createBuffer(CL_MEM_WRITE_ONLY, sizeof(c), NULL);
        
        opencl_kernel_ptr addkernel = testprog->getKernel("add");
        addkernel->setArg(0, abuffer);
        addkernel->setArg(1, bbuffer);
        addkernel->setArg(2, cbuffer);
        
        clcmdqueue->enqueueWriteBuffer(abuffer, true, 0, sizeof(a), a);
        clcmdqueue->enqueueWriteBuffer(bbuffer, true, 0, sizeof(b), b);
        clcmdqueue->enqueueNDRangeKernel(addkernel, opencl_dim(8));
        clcmdqueue->enqueueReadBuffer(cbuffer, true, 0, sizeof(c), c)->wait();
        
        bool error = false;
        for (int i = 0; i < 8; i++) {
            error |= (a[i] + b[i] != c[i]);
        }
        log_debug("%s Add Test: %s", __func__, error ? "Failed" : "Success");
    }

    void testAES()
    {
        opencl_program_ptr aesprog = clctx->createProgram("src/aes.cl");
        opencl_kernel_ptr aes_rijndael_encrypt_kernel = aesprog->getKernel("aes_rijndael_encrypt");
        opencl_kernel_ptr aes_rijndael_decrypt_kernel = aesprog->getKernel("aes_rijndael_decrypt");
        
        static const aes_uchar key[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
        static const size_t MEGA_BYTE = 1024 * 1024;
        static const size_t DATA_SIZE = 100 * MEGA_BYTE;
        
        aes_uchar *pt = new aes_uchar[DATA_SIZE];
        if (!pt) log_error_exit("pt alloc_failed");
        memset((void*)pt, 0xff, DATA_SIZE);
        
        aes_uchar *ct = new aes_uchar[DATA_SIZE];
        if (!ct) log_error_exit("pt alloc_failed");
        memset((void*)ct, 0x00, DATA_SIZE);
        
        void *rk = aes_encrypt_init(key, 16);

        opencl_buffer_ptr rk_buf = clctx->createBuffer(CL_MEM_READ_WRITE, AES_PRIV_SIZE, NULL);
        opencl_buffer_ptr pt_buf = clctx->createBuffer(CL_MEM_READ_WRITE, DATA_SIZE, NULL);
        opencl_buffer_ptr ct_buf = clctx->createBuffer(CL_MEM_READ_WRITE, DATA_SIZE, NULL);
        
        aes_rijndael_encrypt_kernel->setArg(0, rk_buf);
        aes_rijndael_encrypt_kernel->setArg(1, (cl_int)10);
        aes_rijndael_encrypt_kernel->setArg(2, pt_buf);
        aes_rijndael_encrypt_kernel->setArg(3, ct_buf);

        for (int i = 0; i < 10; i++) {
            const auto t1 = high_resolution_clock::now();
            clcmdqueue->enqueueWriteBuffer(rk_buf, true, 0, AES_PRIV_SIZE, rk);
            clcmdqueue->enqueueWriteBuffer(pt_buf, true, 0, DATA_SIZE, pt);
            clcmdqueue->enqueueNDRangeKernel(aes_rijndael_encrypt_kernel, opencl_dim(DATA_SIZE / 16));
            clcmdqueue->enqueueReadBuffer(ct_buf, true, 0, DATA_SIZE, ct)->wait();
            const auto t2 = high_resolution_clock::now();
            
            float time_sec = duration_cast<microseconds>(t2 - t1).count() / 1000000.0;
            log_debug("encrypted %ld MB in %f sec (%f MB/sec)", DATA_SIZE / MEGA_BYTE, time_sec, DATA_SIZE / MEGA_BYTE / time_sec);
        }
        
        aes_encrypt_deinit(rk);
        delete [] pt;
        delete [] ct;
    }
};

int main(int argc, const char * argv[])
{
    aes_opencl_test test;
    
    test.initCL();
    test.testCL();
    test.testAES();

    return 0;
}

