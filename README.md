# TestOpenCl

TestOpenCl uses OpenCL to practie GPU acceleration. This was going to be used only for testing and playing around but lots of progress has led to this becoming an actual repository. The plan overall is to create a buffer of n by n pixels and a buffer of triangles and have the GPU do the 'castRay' function from my ray tracing solution. There is also benchmarking setup so I can compare the results of using the GPU to just single threadded CPU and parallel threadding on the CPU. The main concerns for this are writting to and reading from buffers as those are probably going to be the slower operations. To get 60 fps, there is only 16.6 ms per frame so we need to cut down on any wasteful or slow operations such as reallocating buffers on the GPU.
<br>
<br>
Requirements:
You will need the OpenCL .h files and .lib files. <br>
The directory layout is listed at the top of SourceSingleDevice.cpp which is where those two files will be placed.

<br>
Current benchmarks to beat (800x800 image):
<br>
Write to buffer: .747 ms
<br>
Processing time: 10.115 ms
<br>
Total time taken: 14.77 ms
<br>
<br>

Note: This is for no real math behind the 'castRay' function so being at 14.77 ms is already concerning. I will check this out soon.
