**EntityManager** 

Lightweight C++ sprite management system focused on dynamic sprite creation, fast updates, cache-friendly rendering, and O(1) modifications.

**Overview** 

This project is a custom low-level rendering utility designed for games or graphical applications that need to create and manipulate sprites efficiently at runtime.

The system focuses specifically on sprite lifecycle and render data management:

	* Create sprites when needed
	* Modify sprite appearance and transform data
	* Remove sprites efficiently
	* Keep render data contiguous in memory
	* Optimize CPU-side iteration and cache usage
	
	
**Core Goals** 

	* Fast runtime sprite creation
	* O(1) removal and relocation
	* Cheap sprite updates
	* Cache-friendly batch storage
	* Minimal overhead
	* Simple API
	
**What It Manages**

Each sprite can be modified dynamically:

	* Position
	* Size
	* Render type
	* Visual data
	* Layer / Z order

The manager stores sprite render data in packed arrays for efficient rendering passes.

**Main Idea**

Traditional object-per-sprite storage can scatter memory and reduce cache efficiency.

This project separates sprite handles from packed render data so the renderer can iterate tight contiguous arrays.

That improves CPU-side traversal and batch processing.


**How to build** 

It is a visual studio project.
  