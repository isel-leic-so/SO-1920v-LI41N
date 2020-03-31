


typedef unsigned char byte;
typedef unsigned long ulong;

typedef struct  {
	int fd; // associated file descriptor 
	void *base; // map start address
	int len;
} file_map;

int map_file(const char *path, file_map *fmap, int newsize);
	
void unmap_file(file_map *fmap);

int get_major_pf_count();
	 

int get_minor_pf_count();
	 

void show_avail_mem(const char *msg);
 

void phase_start(const char* phaseName);
	 
