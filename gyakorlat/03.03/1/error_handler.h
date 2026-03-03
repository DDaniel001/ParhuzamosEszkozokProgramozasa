#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <CL/cl.h>

const char* get_error_string(cl_int error);

#endif