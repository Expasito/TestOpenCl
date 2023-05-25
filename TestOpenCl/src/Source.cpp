///*
//* 
//* Directory Setup
//* TestOpenCL
//* --Depend
//* ---include
//* ----CL
//* -----Header Files
//* ---lib
//* ----lib files
//* --src
//* ---Source.cpp
//*/
//
//#include <vector>
//#include <CL/cl.hpp>
//#include <iostream>
//
////callback function
//void pfn_notify(const char* errinfo, const void* private_info, size_t cb, void* user_data) {
//	printf("Error: %s\n", errinfo);
//}
//
//int main() {
//	cl_uint num_entries = 10;
//	cl_platform_id platforms[10];
//	cl_uint num_platforms;
//	cl_device_id devices[10];
//	cl_uint num_devices;
//	cl_int a = clGetPlatformIDs(5, platforms, &num_platforms);
//	char name[100];
//	size_t size;
//	cl_context* contexts = (cl_context*)malloc(sizeof(cl_context) * num_platforms);
//	for (int i = 0; i < num_platforms; i++) {
//		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 100, &name, &size);
//		printf("%s, ", name);
//		//clGetPlatformInfo(platforms[i]);
//		printf("\n");
//		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 10, devices, &num_devices);
//		for (int j = 0; j < num_devices; j++) {
//			cl_uint compute = 0;
//			size_t size2;
//			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &compute,&size2);
//			printf("  CU: %d\n", compute);
//			char name2[100];
//			size_t size3;
//			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(char)*100, name2, &size3);
//			printf("  Name: %s\n", name2);
//
//		}
//		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM,cl_context_properties(platforms[i]),0};
//		cl_int errcode_ret;
//		cl_context out = clCreateContext(properties, num_devices, devices, pfn_notify, NULL, &errcode_ret);
//		if (errcode_ret !=CL_SUCCESS) {
//			printf("Failed on this\n");
//		}
//		clRetainContext(out);
//		contexts[i] = out;
//		// get context data
//		size_t param_value_size;
//		cl_uint param_value;
//		size_t param_value_size_ret;
//		clGetContextInfo(out, CL_CONTEXT_NUM_DEVICES, sizeof(cl_uint), &param_value, &param_value_size_ret);
//		printf("  number of devices in context: %d\n", param_value);
//
//		//cl_command_queue queue = clCreateCommandQueue(out, devices[j])
//		printf("\n");
//
//	}
//	cl_command_queue* queues = (cl_command_queue*)malloc(sizeof(cl_command_queue) * num_platforms);
//	for (int i = 0; i < num_platforms; i++) {
//
//	}
//	
//	
//	return 0;
//}
