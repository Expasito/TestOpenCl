/*
*
* Directory Setup
* TestOpenCL
* --Depend
* ---include
* ----CL
* -----Header Files
* ---lib
* ----lib files
* --src
* ---Source.cpp
*/


/*
* 
* Note: SVM does not work with Nvidia gpu right now. Intel does work
* 
*/
#include <vector>
#include <CL/cl.hpp>
#include <iostream>
#include <stdlib.h>

//callback function
void create_context(const char* errinfo, const void* private_info, size_t cb, void* user_data) {
	printf("Error: %s\n", errinfo);
}

void context_destruct(cl_context context, void* user_data) {
	printf("\n\nContext destroyed\n\n");
}

void program_build(cl_program program, void* user_data) {
	printf("\n\nProgram Built\n\n");
}

void getDeviceInfo(cl_device_id device) {
	size_t param_value_size_device = sizeof(char) * 150;
	char param_value_device[150];
	size_t param_value_size_ret_device;
	clGetDeviceInfo(device, CL_DEVICE_NAME, param_value_size_device, param_value_device, &param_value_size_ret_device);
	printf("    %s\n", param_value_device);
	cl_uint max_units;
	clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &max_units, NULL);
	printf("    max(cu): %d\n", max_units);
	cl_uint max_sub_devices;
	clGetDeviceInfo(device, CL_DEVICE_PARTITION_MAX_SUB_DEVICES, sizeof(cl_uint), &max_sub_devices, NULL);
	printf("    max(sub): %d\n", max_sub_devices);
	//char profile[1000];
	//clGetDeviceInfo(device, CL_DEVICE_PROFILE, sizeof(char) * 1000, profile, NULL);
	//printf(" %s\n", profile);
	cl_uint work;
	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &work, NULL);
	printf("    work item dims: %d\n", work);
}

void getContextInfo(cl_context context) {
	cl_uint reference_count;
	cl_uint num_devices;
	clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(cl_uint), &reference_count, NULL);
	printf("    ref count: %d\n", reference_count);
	clGetContextInfo(context, CL_CONTEXT_NUM_DEVICES, sizeof(cl_uint), &num_devices, NULL);
	printf("    num devices: %d\n", num_devices);

}

void getKernelInfo(cl_kernel kernel) {
	size_t param_value_size = sizeof(char) * 50;
	char name[50];
	clGetKernelInfo(kernel, CL_KERNEL_FUNCTION_NAME, param_value_size, name, NULL);
	printf("    kernel name: %s\n", name);

	cl_uint num_args;
	clGetKernelInfo(kernel, CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &num_args, NULL);
	printf("    kernel arg num: %d", num_args);
}



int main() {

	// handle getting platforms
	cl_uint num_entries=10;
	cl_platform_id platforms[10];
	cl_uint num_platforms;
	clGetPlatformIDs(num_entries, platforms, &num_platforms);
	// look over all platforms avaliable
	for (int i = 0; i < num_platforms; i++) {
		char platform_name[150];
		size_t param_value_size = sizeof(char) * 150;
		size_t param_value_size_ret;
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, param_value_size, platform_name, &param_value_size_ret);
		printf("%s : ", platform_name);
		char platform_version[150];
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, param_value_size, platform_version, NULL);
		printf("%s\n", platform_version);

		// now get all of the devices for that platform
		cl_uint num_entries_device=10;
		cl_device_id devices[10];
		cl_uint num_devices;
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_entries_device, devices, &num_devices);
		printf("    Devices: %d\n", num_devices);

		for (int j = 0; j < num_devices; j++) {
			// helper function for getting device information
			printf("Device Info:\n");
			getDeviceInfo(devices[j]);

			// create subdevices
			const cl_device_partition_property properties[] = {CL_DEVICE_PARTITION_EQUALLY,4,0};
			cl_uint num_devices_sub=20;
			cl_device_id out_devices_sub[20];
			cl_uint num_devices_sub_ret;
			cl_uint err = clCreateSubDevices(devices[j], properties, num_devices_sub, out_devices_sub, &num_devices_sub_ret);
			if (err == CL_DEVICE_PARTITION_FAILED) {
				printf("      ERR: No more devices can be created\n");
			}
			else if(err==CL_INVALID_VALUE) {
				printf("      ERR: Error with subpartition this device\n");
			}

			// we do not need to retain devices since we cannot do subdevices


		}

		// now create contexts
		const cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, cl_context_properties(platforms[i]), 0};
		cl_int errcode_ret;
		cl_context cont = clCreateContext(properties, num_devices,devices,create_context,NULL,&errcode_ret);

		if (errcode_ret != CL_SUCCESS) {
			printf("ERR: issue with creating context\n");
			printf("    Code: %d\n", errcode_ret);
		}

		// we do not need this code
		//cl_int ret_cont = clRetainContext(cont);
		//if (ret_cont != CL_SUCCESS) {
		//	printf("ERR: issue with retaining context\n");
		//}

		printf("Context Info:\n");
		getContextInfo(cont);

		// attach a destructor callback
		cl_int context_dest_set = clSetContextDestructorCallback(cont, context_destruct, NULL);
		if (context_dest_set != CL_SUCCESS) {
			printf("ERR: issue with attaching callback\n");
			printf("    Code: %d\n", context_dest_set);
		}

		//Command Queue

		printf("Command Queue:\n");

		//since we only have one device, we only need the first element
		cl_int errcode_ret_queue;
		//const cl_queue_properties properties_queue[] = {CL_QUEUE_PROPERTIES,CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,0};
		const cl_queue_properties properties_queue[] = { NULL,0 };
		cl_command_queue queue = clCreateCommandQueueWithProperties(cont, devices[0], properties_queue, &errcode_ret_queue);
		if (errcode_ret_queue != CL_SUCCESS) {
			printf("ERR: issue with creating queue\n");
			printf("    Code: %d\n", errcode_ret_queue);
		}


		//Buffer objects

		printf("Buffer Objects:\n");

		int data[5] = {5,4,3,2,1};

		size_t size_data = sizeof(int) * 5;
		cl_int errcode_ret_mem;

		cl_mem mem = clCreateBuffer(cont, CL_MEM_USE_HOST_PTR, size_data, &data, &errcode_ret_mem);

		if (errcode_ret_mem != CL_SUCCESS) {
			printf("ERR: issue with creating buffer\n");
			printf("    Code: %d\n", errcode_ret_mem);
		}

		// write to buffer

		printf("Writing Buffer Data:\n");

		printf("    Values to write: ");
		for (int k = 0; k < 5; k++) {
			printf("%d, ",data[k]);
		}
		printf("\n");
		cl_int write_buff = clEnqueueWriteBuffer(queue, mem, CL_TRUE, 0, size_data, &data, 0, NULL, NULL);
		if (write_buff != CL_SUCCESS) {
			printf("ERR: issue with writing to buffer\n");
			printf("    Code: %d\n", write_buff);
		}



		// read from buffer

		printf("Reading Buffer Data:\n");
		int read[5];
		cl_int read_buff = clEnqueueReadBuffer(queue, mem, CL_TRUE, 0, size_data, &read, 0, NULL, NULL);
		if (read_buff != CL_SUCCESS) {
			printf("ERR: issue with reading from buffer\n");
			printf("    Code: %d\n", read_buff);
		}

		printf("    Values read in: ");
		for (int k = 0; k < 5; k++) {
			printf("%d, ", read[k]);
		}
		printf("\n");

		// shared virtual memory

		printf("Shared Memory:\n");

		// allocate memory
		double from = 500;
		double* to = (double*)clSVMAlloc(cont, CL_MEM_READ_WRITE, sizeof(double), 0);

		if (to == (double*)NULL) {
			printf("Failed to make memroy\n");
		}

		// copy data to the memory buffer

		cl_int mem_cpy = clEnqueueSVMMemcpy(queue, CL_TRUE, to, &from, sizeof(double), 0, NULL, NULL);
		if (mem_cpy != CL_SUCCESS) {
			printf("ERR: issue with copying memory\n");
			printf("    Code: %d\n", mem_cpy);
		}


		printf("    SVM output: %lf\n", *to);

		// clear the memory
		clSVMFree(cont, to);



		// Program objects

		printf("Program Objects:\n");

		const char* code = "__kernel void test(__global int* out, const int index){int sum = 0;for (int i = 0; i < 100000; i++) {sum += i;} int gid = get_global_id(0);out[gid]=gid;};";
		//printf(\"%d, %d,,, \",index,gid);
		cl_int errcode_ret_prog;
		cl_program program = clCreateProgramWithSource(cont, 1, &code, NULL, &errcode_ret_prog);
		if (errcode_ret_prog != CL_SUCCESS) {
			printf("ERR: issue with creating program from source\n");
			printf("    Code: %d\n", errcode_ret_prog);
		}

		// build the program

		printf("Build Program:\n");
		cl_int build = clBuildProgram(program, num_devices, devices, NULL, program_build, NULL);
		if (build != CL_SUCCESS) {
			printf("ERR: issue with building the program\n");
			printf("    Code: %d\n", build);
		}
		

		// get build errors here!
		char log[1000];
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(char) * 1000, log, NULL);
		printf("\n\nBuild output\n\n");
		printf("%s", log);

		printf("\n\n");

		// create kernel object

		printf("Kernel Objects:\n");

		// Option A: create individual kernel objects
		
		//cl_int errcode_ret_kernel;
		//// test is the name of the kernel created above
		//cl_kernel kernel = clCreateKernel(program, "test", &errcode_ret_kernel);
		//if (errcode_ret_kernel != CL_SUCCESS) {
		//	printf("ERR: issue with creating the kernel\n");
		//	printf("    Code: %d\n", errcode_ret_kernel);
		//}
		
		//Option B: create a list of kernel objects from the program

		cl_kernel kernels[20];
		cl_uint num_kernels_ret;
		cl_int kern = clCreateKernelsInProgram(program, 20, kernels, &num_kernels_ret);
		if (kern != CL_SUCCESS) {
			printf("ERR: issue with creating the kernel\n");
			printf("    Code: %d\n", kern);
		}
		printf("Kernels:\n");
		for (int k = 0; k < num_kernels_ret; k++) {
			getKernelInfo(kernels[k]);
			printf("\n");
		}

		/////

		int size_of_array = 1024*1024;

		// create an array of size_of_array elements
		int* data_in = (int*)malloc(sizeof(int) * size_of_array);

		size_data = sizeof(int)*size_of_array;


		cl_mem mem_in = clCreateBuffer(cont, CL_MEM_USE_HOST_PTR, size_data, data_in, &errcode_ret_mem);


		cl_int write_buff_in = clEnqueueWriteBuffer(queue, mem_in, CL_TRUE, 0, size_data, data_in, 0, NULL, NULL);
		if (write_buff != CL_SUCCESS) {
			printf("ERR: issue with writing to buffer\n");
			printf("    Code: %d\n", write_buff);
		}





		////

		// set kernel arguments
		cl_int kern_arg = clSetKernelArg(kernels[0], 0, sizeof(cl_mem), &mem_in);
		//cl_int kern_work = clSetKernelArg(kernels[0], 0, sizeof(int), &data_in);
		printf("kern arg: %d\n", kern_arg);
		//printf("kern work: %d\n", kern_work);

		////// get kernel info
		//size_t work_group_size;

		////size_t param_value_size_ret_kern;
		//cl_int group = clGetKernelWorkGroupInfo(kernels[0], devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);
		//printf("err: %d", group);
		//printf("    work_group_size: %lu\n", (unsigned long)work_group_size);
		// 
		// 
		//// get kernel info
		size_t work_group_size;
		//size_t param_value_size_ret_kern;
		cl_int group = clGetKernelWorkGroupInfo(kernels[0], devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);
		printf("err: %d", group);
		printf("    work_group_size: %lu\n", (unsigned long)work_group_size);
		//printf("kern size: %lu, %lu, %lu\n", (unsigned long)size_kern[0], (unsigned long)size_kern[1], (unsigned long)size_kern[2]);

		////size_t local_work_group_size[3];
		////clGetKernelSubGroupInfo(kernels[0], devices[0], CL_KERNEL_LOCAL_SIZE_FOR_SUB_GROUP_COUNT, sizeof(size_t)*3, &work_group_size, NULL);
		//////printf(" work group size: %lu\n", (unsigned long)local_work_group_size);
		//
		//// executing kernels

		//edit work group size
		work_group_size = size_of_array;
		size_t local_group_size = 256;

		printf("Executing Kernels:\n");
		int index = 0;
		clSetKernelArg(kernels[0], 1, sizeof(int), &index);

		for (int h = 0; h < 80*10000; h++) {
			cl_int ex = clEnqueueNDRangeKernel(queue, kernels[0], 1, NULL, &work_group_size, &local_group_size, 0, NULL, NULL);
			//printf("%d\n", ex);
		}

		//size_t global_work_size[1] = { 500/(2*4)};
		//size_t local_work_size[1] = { 500};
		//std::vector<cl_event> events(size_of_array);
		
		//for (int i = 0; i < size_of_array; i++) {
		//	if (i % 100000 == 0) {
		//		printf("%d\n", i);

		//	}
		//	//create event
		//	cl_event event;

		//	cl_int ex = clEnqueueNDRangeKernel(queue,kernels[0],1,NULL,&work_group_size, &local_group_size, 0, NULL, &event);
		//	if (ex != CL_SUCCESS) {
		//		printf("ERR: %d", ex);

		//	}
		//	//events.push_back(event);
		//	//for (int i = 0; i < events.size(); i++) {
		//	//	char stat[80];
		//	//	cl_int check = clGetEventInfo(events[i], CL_EVENT_COMMAND_EXECUTION_STATUS | CL_COMPLETE, sizeof(char) * 80, stat, NULL);
		//	//	printf("%s\n", stat);
		//	//	printf("%d\n", check);
		//	//	//if(events[i])
		//	//}

		//}


		// wait for command to finish
		clFinish(queue);

		// read in values
		printf("Read in values from kernel\n");
		printf("Reading Buffer Data:\n");
		int* read_in = (int*)malloc(sizeof(int) * size_of_array);
		cl_int read_buff_out = clEnqueueReadBuffer(queue, mem_in, CL_TRUE, 0, size_data, read_in, 0, NULL, NULL);
		if (read_buff != CL_SUCCESS) {
			printf("ERR: issue with reading from buffer\n");
			printf("    Code: %d\n", read_buff);
		}

		//for (int l = 0; l < size_of_array; l++) {
		//	printf("Read in value: %d\n", read_in[l]);

		//}



		//release memory
		clReleaseMemObject(mem_in);
		clReleaseMemObject(mem);


		printf("\n\n");
	}

	printf("END\n");
	return 0;
}
