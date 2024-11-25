
// Conv Parameters
#define CONV_IN_SIZE 256
#define CONV_KERN_SIZE 4
#define CONV_FEAT_SIZE 4
#define CONV_OUT_SIZE (CONV_IN_SIZE - CONV_KERN_SIZE + 1)

// FC Parameters
#define FC_IN_SIZE 1024
#define FC_OUT_SIZE 1024

// Matmul Parameters
#define MATMUL_SIZE 256

// MaxPooling Parameters
#define MAXPOOL_IN_SIZE 256
#define MAXPOOL_FEAT_SIZE 8
#define MAXPOOL_OUT_SIZE (MAXPOOL_IN_SIZE / 2)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
