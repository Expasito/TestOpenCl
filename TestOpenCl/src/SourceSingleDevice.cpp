#define _CRT_SECURE_NO_WARNINGS
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
#include <chrono>

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


/*
* readInKernelCode takes in a filepath and converts the text into a 
* char* for OPENCL to take in
* const char* the filepath to the file
* 
* return a pointer to the character array
* 
*/
char* readInKernelCode(const char* path) {
	FILE* f = fopen(path, "r");

	if (f == NULL) {
		printf("Error loading file\n");
		exit(1);
	}

	int index = 0;
	int size = 10;
	// allocate space for size chars
	char* data = (char*)malloc(sizeof(char) * size);
	char t;
	int err = 1;
	while (1 == 1) {
		err = fscanf(f, "%c", &t);
		if (err == 0) {
			printf("Encounted an error\n");
			exit(1);
		}
		if (err == -1) {
			printf("End of file\n");
			break;
		}
		data[index] = t;
		index++;
		if (index == size) {
			data = (char*)realloc(data, size+size);
			size = size + size;
		}

	}

	data[index++] = '\0';
	return data;
}


int main() {

	



	// handle getting platforms
	cl_uint num_entries=10;
	cl_platform_id platforms[10];
	cl_uint num_platforms;
	clGetPlatformIDs(num_entries, platforms, &num_platforms);
	// look over all platforms avaliable

	printf("There are %d platforms on this computer\n\n", num_platforms);
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
		cl_uint num_entries_device = 10;
		cl_device_id devices[10];
		cl_uint num_devices;
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_entries_device, devices, &num_devices);
		printf("    Devices: %d\n", num_devices);

		for (int j = 0; j < num_devices; j++) {
			// helper function for getting device information
			printf("Device Info:\n");
			getDeviceInfo(devices[j]);

			// create subdevices
			const cl_device_partition_property properties[] = { CL_DEVICE_PARTITION_EQUALLY,4,0 };
			cl_uint num_devices_sub = 20;
			cl_device_id out_devices_sub[20];
			cl_uint num_devices_sub_ret;
			cl_uint err = clCreateSubDevices(devices[j], properties, num_devices_sub, out_devices_sub, &num_devices_sub_ret);
			if (err == CL_DEVICE_PARTITION_FAILED) {
				printf("      ERR: No more devices can be created\n");
			}
			else if (err == CL_INVALID_VALUE) {
				printf("      ERR: Error with subpartition this device\n");
			}

			// we do not need to retain devices since we cannot do subdevices


		}

		// now create contexts
		const cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, cl_context_properties(platforms[i]), 0 };
		cl_int errcode_ret;
		cl_context cont = clCreateContext(properties, num_devices, devices, create_context, NULL, &errcode_ret);

		if (errcode_ret != CL_SUCCESS) {
			printf("ERR: issue with creating context\n");
			printf("    Code: %d\n", errcode_ret);
		}



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



		// Program objects

		printf("Program Objects:\n");


		const char* code = readInKernelCode("kernels/Main.kernel");
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



		// we will use the first kernel since that is our 'main' function
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




		/*
		*
		*
		* Create the buffer objects for the program and then send them over to the kernel arguments
		*
		*
		*/

		// we will consider this to be the start of the program since we will update buffers and stuff
		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

		//Buffer objects

		printf("Buffer Objects:\n");


		// test using structs for sending data over
		struct vec3 {
			float x, y, z;
		};


		// give first array of vectors
		vec3 vectors1[] = {
			{0,0,0},
			{1,1,1},
			{2,2,2},
			{3,3,3},
			{4,4,4},
			{5,5,5},
			{6,6,6},
			{7,7,7},
			{8,8,8},
			{9,9,9},
			{10,10,10}
		};

		// give second array of vectors
		vec3 vectors2[] = {
			{0,0,0},
			{-1,-1,-1},
			{-2,-2,-2},
			{-3,-3,-3},
			{-4,-4,-4},
			{-5,-5,-5},
			{-6,-6,-6},
			{-7,-7,-7},
			{-8,-8,-8},
			{-9,-9,-9},
			{-10,-10,-10}
		};
		size_t vectors1Size = sizeof(vectors1);
		cl_mem vectors1Memory = clCreateBuffer(cont, CL_MEM_USE_HOST_PTR, vectors1Size, &vectors1, NULL);


		size_t vectors2Size = sizeof(vectors2);
		cl_mem vectors2Memory = clCreateBuffer(cont, CL_MEM_USE_HOST_PTR, vectors2Size, &vectors2, NULL);


		// this records the number of elements to process
		int elements = sizeof(vectors1) / sizeof(vec3);
		printf("Computing %d elements\n", elements);

		int width = 80;
		int height = 80;
		int workers = width * height;

		// create a return array of vectors
		// this needs to be with*height in size
		cl_mem outputVectors = clCreateBuffer(cont, CL_MEM_READ_WRITE, sizeof(vec3) * width * height, NULL, NULL);


		cl_mem maxElements = clCreateBuffer(cont, CL_MEM_READ_WRITE, sizeof(elements), &elements, NULL);


		std::chrono::high_resolution_clock::time_point buffersDone = std::chrono::high_resolution_clock::now();
		
		printf("Time for Buffers: %lf [ms]\n", (buffersDone - start).count() / 1000000.0);

		// set kernel arguments

		// First is the output vector
		cl_int kern_arg = clSetKernelArg(kernels[0], 0, sizeof(cl_mem), &outputVectors);


		// set the first input vector
		cl_int kernel_arg2 = clSetKernelArg(kernels[0], 1, sizeof(cl_mem), &vectors1Memory);

		// set the second input vector
		cl_int kernel_arg3 = clSetKernelArg(kernels[0], 2, sizeof(cl_mem), &vectors2Memory);

		cl_int kernel_arg4 = clSetKernelArg(kernels[0], 3, sizeof(int),&elements);


		std::chrono::high_resolution_clock::time_point argumentsDone = std::chrono::high_resolution_clock::now();

		printf("Time for Args: %lf [ms]\n", (argumentsDone - buffersDone).count() / 1000000.0);


		//// get kernel info
		size_t work_group_size;
		//size_t param_value_size_ret_kern;
		cl_int group = clGetKernelWorkGroupInfo(kernels[0], devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);
		printf("err: %d", group);
		printf("    work_group_size: %lu\n", (unsigned long)work_group_size);



		//edit work group size
		work_group_size = workers;
		size_t local_group_size = 1;

		printf("Executing Kernels:\n");


		cl_int ex = clEnqueueNDRangeKernel(queue, kernels[0], 1, NULL, &work_group_size, &local_group_size, 0, NULL, NULL);


		std::chrono::high_resolution_clock::time_point enqueuedDone = std::chrono::high_resolution_clock::now();
		printf("Time for Enqueue: %lf [ms]\n", (enqueuedDone - argumentsDone).count() / 1000000.0);



		// wait for command to finish
		clFinish(queue);

		std::chrono::high_resolution_clock::time_point clFinished = std::chrono::high_resolution_clock::now();
		printf("Time for Finish: %lf [ms]\n", (clFinished - enqueuedDone).count() / 1000000.0);



		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		printf("Total time taken: %lf [ms]\n", (end-start).count() / 1000000.0);


		//// read in values
		printf("Read in values from kernel\n");
		printf("Reading Buffer Data:\n");
		int* output = (int*)malloc(sizeof(int) * (workers));
		//vec3 output[50];
		cl_int ret = 0;
		ret = clEnqueueReadBuffer(queue, outputVectors, CL_TRUE, 0, sizeof(vec3) * elements, output, 0, NULL, NULL);

		printf("Return from read buffer: %d\n", ret);
		for (int i = 0; i < elements; i++) {
			int x = output[i];
			printf("I: %d :: (%f)\n", i, x);
		}

		//int* read_in = (int*)malloc(sizeof(int) * size_of_array);
		
		//cl_int read_buff_out = clEnqueueReadBuffer(queue, mem_in, CL_TRUE, 0, size_data, read_in, 0, NULL, NULL);
		//if (read_buff_out != CL_SUCCESS) {
		//	printf("ERR: issue with reading from buffer\n");
		//	printf("    Code: %d\n", read_buff_out);
		//}





		//release memory
		clReleaseMemObject(vectors1Memory);
		clReleaseMemObject(vectors2Memory);
		clReleaseMemObject(outputVectors);


		exit(1);


		printf("\n\n");
	}

	printf("END\n");
	return 0;
}
