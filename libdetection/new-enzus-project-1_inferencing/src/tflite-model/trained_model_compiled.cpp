/* Generated by Edge Impulse
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
// Generated on: 01.09.2022 20:00:32

#include <stdio.h>
#include <stdlib.h>
#include "edge-impulse-sdk/tensorflow/lite/c/builtin_op_data.h"
#include "edge-impulse-sdk/tensorflow/lite/c/common.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"

#if EI_CLASSIFIER_PRINT_STATE
#if defined(__cplusplus) && EI_C_LINKAGE == 1
extern "C" {
    extern void ei_printf(const char *format, ...);
}
#else
extern void ei_printf(const char *format, ...);
#endif
#endif

#if defined __GNUC__
#define ALIGN(X) __attribute__((aligned(X)))
#elif defined _MSC_VER
#define ALIGN(X) __declspec(align(X))
#elif defined __TASKING__
#define ALIGN(X) __align(X)
#endif

#ifndef EI_MAX_SCRATCH_BUFFER_COUNT
#define EI_MAX_SCRATCH_BUFFER_COUNT 4
#endif // EI_MAX_SCRATCH_BUFFER_COUNT

#ifndef EI_MAX_OVERFLOW_BUFFER_COUNT
#define EI_MAX_OVERFLOW_BUFFER_COUNT 10
#endif // EI_MAX_OVERFLOW_BUFFER_COUNT

using namespace tflite;
using namespace tflite::ops;
using namespace tflite::ops::micro;

namespace {

constexpr int kTensorArenaSize = 9040;

#if defined(EI_CLASSIFIER_ALLOCATION_STATIC)
uint8_t tensor_arena[kTensorArenaSize] ALIGN(16);
#elif defined(EI_CLASSIFIER_ALLOCATION_STATIC_HIMAX)
#pragma Bss(".tensor_arena")
uint8_t tensor_arena[kTensorArenaSize] ALIGN(16);
#pragma Bss()
#elif defined(EI_CLASSIFIER_ALLOCATION_STATIC_HIMAX_GNU)
uint8_t tensor_arena[kTensorArenaSize] ALIGN(16) __attribute__((section(".tensor_arena")));
#else
#define EI_CLASSIFIER_ALLOCATION_HEAP 1
uint8_t* tensor_arena = NULL;
#endif

static uint8_t* tensor_boundary;
static uint8_t* current_location;

template <int SZ, class T> struct TfArray {
  int sz; T elem[SZ];
};
enum used_operators_e {
  OP_RESHAPE, OP_CONV_2D, OP_MAX_POOL_2D, OP_FULLY_CONNECTED, OP_SOFTMAX,  OP_LAST
};
struct TensorInfo_t { // subset of TfLiteTensor used for initialization from constant memory
  TfLiteAllocationType allocation_type;
  TfLiteType type;
  void* data;
  TfLiteIntArray* dims;
  size_t bytes;
  TfLiteQuantization quantization;
};
struct NodeInfo_t { // subset of TfLiteNode used for initialization from constant memory
  struct TfLiteIntArray* inputs;
  struct TfLiteIntArray* outputs;
  void* builtin_data;
  used_operators_e used_op_index;
};

TfLiteContext ctx{};
TfLiteTensor tflTensors[23];
TfLiteEvalTensor tflEvalTensors[23];
TfLiteRegistration registrations[OP_LAST];
TfLiteNode tflNodes[11];

const TfArray<2, int> tensor_dimension0 = { 2, { 1,3960 } };
const TfArray<1, float> quant0_scale = { 1, { 0.00390625, } };
const TfArray<1, int> quant0_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant0 = { (TfLiteFloatArray*)&quant0_scale, (TfLiteIntArray*)&quant0_zero, 0 };
const ALIGN(16) int32_t tensor_data1[4] = { 1, 1, 99, 40, };
const TfArray<1, int> tensor_dimension1 = { 1, { 4 } };
const ALIGN(16) int32_t tensor_data2[4] = { 1, 99, 1, 8, };
const TfArray<1, int> tensor_dimension2 = { 1, { 4 } };
const ALIGN(16) int32_t tensor_data3[4] = { 1, 1, 50, 8, };
const TfArray<1, int> tensor_dimension3 = { 1, { 4 } };
const ALIGN(16) int32_t tensor_data4[4] = { 1, 50, 1, 16, };
const TfArray<1, int> tensor_dimension4 = { 1, { 4 } };
const ALIGN(8) int32_t tensor_data5[2] = { -1, 400, };
const TfArray<1, int> tensor_dimension5 = { 1, { 2 } };
const ALIGN(16) int8_t tensor_data6[8*1*3*40] = { 
  /* [0][0][][] */ -108,13,-23,-26,-22,-70,-122,-114,28,-6,8,46,13,-6,-71,-115,14,34,-127,-43,-2,59,83,-24,59,50,27,73,5,6,34,-77,-76,-48,-24,-3,-68,-7,-38,-54, 29,-39,-21,-30,-17,38,53,0,18,-26,11,25,8,-22,11,27,-3,23,-48,-82,-25,23,45,-107,41,-39,-30,43,-7,47,48,52,44,45,1,-18,-35,-17,-55,-16, 90,14,26,49,58,-56,-18,-33,-55,-42,-41,23,31,36,-4,-42,-20,20,-74,-55,-64,-23,-25,-108,-13,-37,-36,-72,-105,-6,-26,-42,38,70,25,17,-8,28,61,50, 
  /* [1][0][][] */ -101,-38,-56,6,44,67,23,-25,10,28,6,-47,14,-29,2,40,26,11,35,-59,-74,5,-7,9,45,20,7,12,-1,46,6,21,-35,-38,-14,-53,-9,-24,14,31, -110,-5,-16,41,0,82,51,-12,9,21,-20,36,-98,-74,5,-37,-1,23,-19,-57,-23,9,56,50,27,-7,-32,-33,-6,10,-19,70,-20,-1,-11,9,-50,10,47,64, -127,-35,-25,101,81,23,7,-27,44,17,-20,-4,10,-26,-25,47,-2,-41,-6,-88,-67,-53,-22,51,-20,-10,15,-27,37,-8,-20,56,3,18,21,-10,19,16,37,19, 
  /* [2][0][][] */ 53,-35,-32,-57,-111,-127,-37,13,6,18,31,49,-8,5,-20,-4,8,48,9,69,-2,15,6,20,-4,1,-17,-32,-32,-40,-33,-20,-22,-28,22,25,-27,-34,25,36, 44,-58,-51,-25,-51,-39,-9,-24,9,-18,18,-9,-31,-4,-18,5,15,28,-27,30,-14,2,-10,6,-8,7,-40,-9,-7,-23,-20,-6,-24,-9,42,2,20,18,26,48, 93,-10,40,52,73,-62,-22,-34,-33,-32,2,-13,-46,29,-17,7,-39,21,-34,17,13,1,1,1,-8,18,25,18,6,-24,-19,-6,0,1,14,25,-16,-46,34,3, 
  /* [3][0][][] */ 69,47,-9,5,5,-73,1,38,0,18,14,35,46,11,17,-31,-37,-109,-82,-23,3,-14,-8,-127,-16,-25,-4,-50,-92,4,-36,-58,21,-11,-28,34,20,62,-34,-99, 61,28,32,-47,27,17,45,21,-6,44,-40,38,5,-39,-36,9,-58,-64,-45,-17,14,12,2,-108,-57,-10,24,-27,-39,27,27,-73,-5,40,-4,25,2,-30,27,-88, 95,-7,77,-7,49,5,74,54,32,36,65,77,74,11,41,-34,45,-29,-8,34,6,-36,-42,-82,-21,5,-8,76,-50,36,-22,-79,31,66,38,75,15,22,29,6, 
  /* [4][0][][] */ -77,30,-22,-17,5,-70,16,14,-54,16,23,121,-43,-60,-34,7,30,-50,-10,-46,-30,1,18,-17,25,39,40,1,73,-41,18,-6,-15,-39,-16,-70,13,46,2,49, -127,45,5,-36,-4,-18,-38,-23,19,-16,-2,56,-90,-70,-88,30,46,-54,-14,-4,-60,-73,-16,55,21,49,-8,36,31,5,62,63,-65,-22,75,-25,5,-51,42,11, -126,4,62,-10,1,-105,-42,-20,30,-1,13,67,-19,-59,-69,-2,-23,-5,6,-28,-31,-16,59,-30,25,-29,32,16,33,-55,-40,10,-39,-16,39,6,10,27,55,61, 
  /* [5][0][][] */ 32,-59,-54,16,10,-37,-2,-5,2,-29,5,-42,20,14,-42,14,12,40,-15,45,-47,-14,20,0,-5,2,43,-45,-27,-1,45,64,-51,-13,10,25,-2,-8,-17,-18, 12,-117,-127,69,27,15,-12,-5,-33,45,-7,-117,-15,5,10,-12,14,-5,32,-13,-65,-52,-38,-32,-20,12,20,-14,-55,13,26,82,-21,5,33,9,-45,46,20,4, 17,-55,-74,-7,32,-41,16,24,6,-31,2,-3,30,63,7,54,45,20,26,50,-20,3,11,-1,30,57,50,-7,6,5,-13,11,-61,-50,44,-57,-36,2,1,33, 
  /* [6][0][][] */ -102,2,-18,-44,-14,69,69,41,2,55,-7,67,-82,-72,-34,19,-7,-15,35,-52,-32,11,-2,64,25,-14,-45,4,55,-29,-38,-3,-40,41,31,29,-11,-47,-14,52, -89,-17,-2,22,-16,90,-16,-42,57,1,18,40,-90,-37,-89,46,-62,-12,-43,46,-13,26,44,50,-14,-4,-49,-28,36,-23,10,15,-62,-44,27,2,-2,39,27,52, -127,31,15,-16,-10,47,33,9,40,12,-9,49,-97,-48,-47,-9,-68,-59,-10,26,4,-16,-32,21,-38,-29,17,-15,44,-20,-2,7,-32,21,59,11,10,-20,59,49, 
  /* [7][0][][] */ -112,-109,-85,121,115,30,-56,-6,-45,39,-50,-73,12,74,6,-31,-47,33,50,-102,14,-3,82,85,59,42,-100,-115,52,29,-51,49,-28,-52,5,-66,57,-24,73,93, -101,-41,-50,88,-16,88,19,16,32,9,-3,-25,-22,-5,86,-75,29,44,9,-110,-34,-25,70,-21,66,49,0,-89,49,44,7,78,-51,27,-47,-106,-88,39,47,-7, -121,-102,-98,66,127,74,25,32,-82,56,-21,-60,10,65,7,38,12,27,-29,-53,-11,55,-2,-24,-42,36,10,-23,-68,-1,-58,51,-96,-23,40,-3,-87,-64,10,95, 
};
const TfArray<4, int> tensor_dimension6 = { 4, { 8,1,3,40 } };
const TfArray<8, float> quant6_scale = { 8, { 0.0058754999190568924, 0.0043329494073987007, 0.0071158474311232567, 0.0040149749256670475, 0.0041494374163448811, 0.0050068171694874763, 0.004116046242415905, 0.0029266765341162682, } };
const TfArray<8, int> quant6_zero = { 8, { 0,0,0,0,0,0,0,0 } };
const TfLiteAffineQuantization quant6 = { (TfLiteFloatArray*)&quant6_scale, (TfLiteIntArray*)&quant6_zero, 0 };
const ALIGN(16) int32_t tensor_data7[8] = { 1682, -14417, -123, -1328, -6637, -3592, -6202, -8365, };
const TfArray<1, int> tensor_dimension7 = { 1, { 8 } };
const TfArray<8, float> quant7_scale = { 8, { 2.2951171558815986e-05, 1.6925583622651175e-05, 2.7796279027825221e-05, 1.5683495803386904e-05, 1.6208739907597192e-05, 1.9557879568310454e-05, 1.6078305634437129e-05, 1.1432330211391672e-05, } };
const TfArray<8, int> quant7_zero = { 8, { 0,0,0,0,0,0,0,0 } };
const TfLiteAffineQuantization quant7 = { (TfLiteFloatArray*)&quant7_scale, (TfLiteIntArray*)&quant7_zero, 0 };
const ALIGN(16) int8_t tensor_data8[16*1*3*8] = { 
  /* [0][0][][] */ -104,-45,-122,59,-63,-54,-35,-79, -79,-40,-127,-3,-96,-80,-28,-57, -63,-29,-56,65,-67,-33,5,-12, 
  /* [1][0][][] */ 68,49,11,-23,-6,-7,41,5, -55,-27,3,-15,-41,47,-20,-8, 127,54,36,2,-24,14,-14,14, 
  /* [2][0][][] */ -18,29,4,-18,4,52,-7,-13, 127,18,-11,-2,33,-43,-13,1, 48,-4,-38,-12,12,1,22,-6, 
  /* [3][0][][] */ -120,-79,-64,-27,-71,28,-19,-8, -109,-87,-121,89,-81,-7,-17,-81, -76,-108,-65,31,-127,-8,-25,-59, 
  /* [4][0][][] */ -114,-56,-29,-28,-38,-55,-99,35, -116,-70,-38,-54,-127,-70,-64,42, -82,-72,-12,33,-126,-28,-90,41, 
  /* [5][0][][] */ -111,-88,-95,52,-20,25,-30,30, -111,-127,-110,43,-51,-5,-71,-14, -84,-78,-65,-15,-94,-5,-42,6, 
  /* [6][0][][] */ 127,34,44,29,23,67,7,54, 97,-2,22,11,-13,87,5,-43, 27,74,13,-3,28,-34,0,14, 
  /* [7][0][][] */ -4,16,82,5,2,94,39,12, 127,-24,6,0,55,30,38,-40, -17,4,-2,-14,36,27,-45,47, 
  /* [8][0][][] */ -10,5,127,-7,-13,24,5,65, 48,11,-17,0,24,49,9,61, 90,-11,17,-34,-21,2,37,29, 
  /* [9][0][][] */ 93,-49,27,-32,55,104,-77,-7, 127,92,121,-34,14,56,95,-20, 8,-20,97,-57,-5,24,2,48, 
  /* [10][0][][] */ 26,18,56,-8,11,-21,-13,9, 127,18,14,-8,46,37,39,-26, 74,8,22,-16,-21,0,29,15, 
  /* [11][0][][] */ -107,-75,-127,77,-104,-98,-64,-67, -106,-6,-86,-35,-101,-85,-49,9, -105,11,-106,84,-17,-42,2,13, 
  /* [12][0][][] */ 127,31,-25,-21,23,54,17,-30, -1,-4,83,-22,18,-27,20,22, 90,38,-14,-21,-18,49,-9,48, 
  /* [13][0][][] */ -117,-25,-105,46,-64,-48,-59,-88, -127,-40,-54,-33,-58,-42,-68,-83, -111,-33,-85,32,-102,-24,-15,-47, 
  /* [14][0][][] */ -61,-14,-63,78,-83,-33,-56,-127, -64,19,-87,58,-57,-49,-42,-70, -54,17,3,-20,-79,-51,-16,-36, 
  /* [15][0][][] */ -72,4,-109,127,-55,-70,-68,28, -94,-7,-76,-60,17,-33,-22,81, -89,-43,-78,-113,-79,15,-87,76, 
};
const TfArray<4, int> tensor_dimension8 = { 4, { 16,1,3,8 } };
const TfArray<16, float> quant8_scale = { 16, { 0.0059617641381919384, 0.0095695154741406441, 0.010669284500181675, 0.0048551964573562145, 0.0055704796686768532, 0.005627073347568512, 0.0071159531362354755, 0.0069796978496015072, 0.0086085889488458633, 0.0050568156875669956, 0.0088381590321660042, 0.0046288012526929379, 0.0080094179138541222, 0.0059651690535247326, 0.0058660837821662426, 0.0042828870937228203, } };
const TfArray<16, int> quant8_zero = { 16, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } };
const TfLiteAffineQuantization quant8 = { (TfLiteFloatArray*)&quant8_scale, (TfLiteIntArray*)&quant8_zero, 0 };
const ALIGN(16) int32_t tensor_data9[16] = { 4625, -1302, -41, 5592, 6020, 4535, -109, -1036, -1228, -2175, -458, 5265, -420, 5267, 4458, 4064, };
const TfArray<1, int> tensor_dimension9 = { 1, { 16 } };
const TfArray<16, float> quant9_scale = { 16, { 3.391371137695387e-05, 5.4436539357993752e-05, 6.0692615079460666e-05, 2.7618962121778168e-05, 3.1687875889474526e-05, 3.2009811548050493e-05, 4.0479357267031446e-05, 3.9704264054307714e-05, 4.8970272473525256e-05, 2.8765880415448919e-05, 5.0276190449949354e-05, 2.6331104891141877e-05, 4.5561864681076258e-05, 3.3933079976122826e-05, 3.3369429729646072e-05, 2.436335853417404e-05, } };
const TfArray<16, int> quant9_zero = { 16, { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } };
const TfLiteAffineQuantization quant9 = { (TfLiteFloatArray*)&quant9_scale, (TfLiteIntArray*)&quant9_zero, 0 };
const ALIGN(16) int8_t tensor_data10[2*400] = { 
  93, -16, 14, 51, 80, 42, 4, -33, -86, -36, -40, 100, -7, 73, 88, -5, 59, -76, -31, 65, 40, 67, -22, -34, -27, -64, 6, 10, -50, 89, 85, 79, 71, 1, -72, 33, 46, 16, 7, -18, -10, -20, -43, 59, -17, 62, 86, 62, 31, -74, -49, 71, 42, 57, 17, 7, 10, -19, -29, 25, -42, 42, 48, 84, 28, -51, 23, 8, 25, 33, -15, 11, -30, -54, -50, 15, -60, 74, 33, 39, 21, -85, 19, 6, 16, -15, -26, -49, -89, -59, -48, -4, -39, 43, 24, 14, 41, -46, -16, 27, 19, 5, -44, -59, -81, -30, -35, 12, -54, 46, 8, -14, 67, -42, -19, 24, 11, 2, -27, -12, 5, -26, -10, 65, -30, 57, 36, 10, 80, -17, 30, 58, 33, 28, -17, -14, 1, -25, 11, 73, -47, 80, 42, 22, 46, -121, -69, 15, 39, 29, -39, -40, -44, -7, 0, 71, -17, 60, 52, 30, -10, -57, -56, -3, 22, -5, -44, 0, -18, -27, -33, -21, -54, 9, 7, 1, 15, -63, -52, 21, 38, 16, -20, -50, -34, -9, -57, 17, -70, 9, 0, 62, 5, -23, -45, 21, 75, 46, -2, -2, 14, -14, -9, 10, -44, 47, -17, 8, 76, -43, -7, 39, 46, 25, -20, 12, -5, -29, -5, 29, -53, 50, 33, 28, 35, -113, -77, 25, 66, 67, -2, -21, 12, 7, -45, 28, -7, 13, 25, 127, 9, -103, -35, 47, 27, 53, -45, -15, 17, -8, -33, 12, -47, 55, 27, 81, 45, -41, -21, 60, 1, 54, -19, -25, -32, -26, -41, -15, -44, 37, 47, -41, 24, -50, -21, 32, 13, 44, 8, -12, -17, -31, 1, 7, -66, 48, 36, 47, 29, -61, -44, 21, 28, 18, -61, -26, -58, -30, -7, 13, -27, 28, -14, 29, 17, -58, -103, -2, 16, 23, -22, -23, -30, -20, -46, 23, -39, 54, -2, 21, 12, -58, -37, 15, 26, 10, -47, 4, 27, -45, -24, -4, -56, 31, -9, 29, 46, -75, 5, 50, 52, 47, -40, -51, -39, -77, -24, 69, -82, 36, 51, 59, -3, -7, -44, -27, 68, -38, -49, -95, -19, -11, -3, 4, -29, 24, -4, 50, 67, -112, -64, 78, 105, 76, -53, -55, -3, -55, -59, 34, -55, 44, 78, 21, 57, 33, -50, 46, 30, 40, -33, -113, -35, -59, -38, 40, -50, 70, 37, 15, 
  -88, -22, -17, -53, -63, -44, 14, 45, 91, 43, 41, -94, 6, -72, -95, -17, -44, 62, 54, -62, -50, -61, 10, 27, -2, 52, 30, -23, 54, -79, -78, -54, -74, 3, 75, -50, -44, -38, 7, 11, -12, 36, 52, -32, 4, -82, -71, -52, -16, 76, 62, -54, -41, -58, -24, 12, 2, -8, 52, -27, 43, -72, -45, -72, -39, 62, -30, -47, -28, -51, 31, 4, 27, 23, 34, -14, 50, -64, -50, -47, -39, 92, 4, -23, -29, 3, 51, 38, 70, 32, 45, -25, 31, -35, -20, -13, -29, 69, 49, -23, -21, -6, 72, 58, 100, 35, 34, -8, 29, -56, -2, 14, -58, 28, 23, -28, -25, -33, 27, 23, 13, 28, 16, -47, 22, -68, -26, -12, -83, 16, -23, -36, -22, -38, 15, 26, 31, 25, 12, -61, 45, -58, -41, -22, -43, 102, 85, 14, -21, -37, 38, 29, 66, 16, 22, -64, 29, -68, -37, -60, 9, 72, 81, 17, -11, -1, 43, 23, -16, 1, 55, 9, 22, 4, 26, -18, 3, 81, 54, -5, -69, -26, 41, 51, -2, 37, 59, -4, 65, -2, -13, -62, -4, 25, 29, -42, -47, -58, 7, -6, 14, -1, 15, 3, 54, -46, 19, 9, -48, 28, 27, -45, -55, -45, 23, -8, -28, 39, -4, -40, 63, -56, -48, -28, -22, 111, 93, -6, -31, -41, 9, 7, -8, 18, 41, -41, 12, -44, -1, -106, -27, 108, 46, -24, -45, -51, 20, 13, -17, 34, 42, -33, 50, -52, -20, -91, -12, 47, 50, -48, -11, -17, 39, 16, 18, 29, 7, 4, 49, -52, -25, 42, -19, 50, -11, -37, -45, -47, 12, 14, -10, 25, 10, -17, 59, -63, -34, -51, -22, 60, 52, -17, -55, -31, 45, 35, 37, 38, 45, -5, 31, -56, 11, 2, -48, 68, 99, -22, -27, -32, 55, 11, 21, 2, 63, -27, 74, -51, -37, -14, -24, 95, 30, -25, -14, -9, 21, 16, -45, 33, 17, -13, 55, -5, -19, -34, -48, 56, -14, -27, -66, -33, 35, 69, 26, 55, 47, -29, 82, -65, -68, -65, 30, 39, 57, 3, -79, 37, 39, 94, 8, 14, 22, 12, 8, -14, 22, -51, -40, 74, 60, -89, -108, -82, 60, 54, 10, 37, 55, -29, 57, -71, -45, -11, -54, -30, 67, -43, -21, -10, 32, 108, 68, 51, 45, -18, 83, -62, -32, 12, 
};
const TfArray<2, int> tensor_dimension10 = { 2, { 2,400 } };
const TfArray<1, float> quant10_scale = { 1, { 0.0059432340785861015, } };
const TfArray<1, int> quant10_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant10 = { (TfLiteFloatArray*)&quant10_scale, (TfLiteIntArray*)&quant10_zero, 0 };
const ALIGN(8) int32_t tensor_data11[2] = { 3161, -3161, };
const TfArray<1, int> tensor_dimension11 = { 1, { 2 } };
const TfArray<1, float> quant11_scale = { 1, { 3.3520012948429212e-05, } };
const TfArray<1, int> quant11_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant11 = { (TfLiteFloatArray*)&quant11_scale, (TfLiteIntArray*)&quant11_zero, 0 };
const TfArray<4, int> tensor_dimension12 = { 4, { 1,1,99,40 } };
const TfArray<1, float> quant12_scale = { 1, { 0.00390625, } };
const TfArray<1, int> quant12_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant12 = { (TfLiteFloatArray*)&quant12_scale, (TfLiteIntArray*)&quant12_zero, 0 };
const TfArray<4, int> tensor_dimension13 = { 4, { 1,1,99,8 } };
const TfArray<1, float> quant13_scale = { 1, { 0.0056885364465415478, } };
const TfArray<1, int> quant13_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant13 = { (TfLiteFloatArray*)&quant13_scale, (TfLiteIntArray*)&quant13_zero, 0 };
const TfArray<4, int> tensor_dimension14 = { 4, { 1,99,1,8 } };
const TfArray<1, float> quant14_scale = { 1, { 0.0056885364465415478, } };
const TfArray<1, int> quant14_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant14 = { (TfLiteFloatArray*)&quant14_scale, (TfLiteIntArray*)&quant14_zero, 0 };
const TfArray<4, int> tensor_dimension15 = { 4, { 1,50,1,8 } };
const TfArray<1, float> quant15_scale = { 1, { 0.0056885364465415478, } };
const TfArray<1, int> quant15_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant15 = { (TfLiteFloatArray*)&quant15_scale, (TfLiteIntArray*)&quant15_zero, 0 };
const TfArray<4, int> tensor_dimension16 = { 4, { 1,1,50,8 } };
const TfArray<1, float> quant16_scale = { 1, { 0.0056885364465415478, } };
const TfArray<1, int> quant16_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant16 = { (TfLiteFloatArray*)&quant16_scale, (TfLiteIntArray*)&quant16_zero, 0 };
const TfArray<4, int> tensor_dimension17 = { 4, { 1,1,50,16 } };
const TfArray<1, float> quant17_scale = { 1, { 0.0056400289759039879, } };
const TfArray<1, int> quant17_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant17 = { (TfLiteFloatArray*)&quant17_scale, (TfLiteIntArray*)&quant17_zero, 0 };
const TfArray<4, int> tensor_dimension18 = { 4, { 1,50,1,16 } };
const TfArray<1, float> quant18_scale = { 1, { 0.0056400289759039879, } };
const TfArray<1, int> quant18_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant18 = { (TfLiteFloatArray*)&quant18_scale, (TfLiteIntArray*)&quant18_zero, 0 };
const TfArray<4, int> tensor_dimension19 = { 4, { 1,25,1,16 } };
const TfArray<1, float> quant19_scale = { 1, { 0.0056400289759039879, } };
const TfArray<1, int> quant19_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant19 = { (TfLiteFloatArray*)&quant19_scale, (TfLiteIntArray*)&quant19_zero, 0 };
const TfArray<2, int> tensor_dimension20 = { 2, { 1,400 } };
const TfArray<1, float> quant20_scale = { 1, { 0.0056400289759039879, } };
const TfArray<1, int> quant20_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant20 = { (TfLiteFloatArray*)&quant20_scale, (TfLiteIntArray*)&quant20_zero, 0 };
const TfArray<2, int> tensor_dimension21 = { 2, { 1,2 } };
const TfArray<1, float> quant21_scale = { 1, { 0.10071095824241638, } };
const TfArray<1, int> quant21_zero = { 1, { -5 } };
const TfLiteAffineQuantization quant21 = { (TfLiteFloatArray*)&quant21_scale, (TfLiteIntArray*)&quant21_zero, 0 };
const TfArray<2, int> tensor_dimension22 = { 2, { 1,2 } };
const TfArray<1, float> quant22_scale = { 1, { 0.00390625, } };
const TfArray<1, int> quant22_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant22 = { (TfLiteFloatArray*)&quant22_scale, (TfLiteIntArray*)&quant22_zero, 0 };
const TfLiteReshapeParams opdata0 = { { 0, 0, 0, 0, 0, 0, 0, 0, }, 0 };
const TfArray<2, int> inputs0 = { 2, { 0,1 } };
const TfArray<1, int> outputs0 = { 1, { 12 } };
const TfLiteConvParams opdata1 = { kTfLitePaddingSame, 1,1, kTfLiteActRelu, 1,1 };
const TfArray<3, int> inputs1 = { 3, { 12,6,7 } };
const TfArray<1, int> outputs1 = { 1, { 13 } };
const TfLiteReshapeParams opdata2 = { { 0, 0, 0, 0, 0, 0, 0, 0, }, 0 };
const TfArray<2, int> inputs2 = { 2, { 13,2 } };
const TfArray<1, int> outputs2 = { 1, { 14 } };
const TfLitePoolParams opdata3 = { kTfLitePaddingSame, 1,2, 1,2, kTfLiteActNone, { { 0,0, 0, 0 } } };
const TfArray<1, int> inputs3 = { 1, { 14 } };
const TfArray<1, int> outputs3 = { 1, { 15 } };
const TfLiteReshapeParams opdata4 = { { 0, 0, 0, 0, 0, 0, 0, 0, }, 0 };
const TfArray<2, int> inputs4 = { 2, { 15,3 } };
const TfArray<1, int> outputs4 = { 1, { 16 } };
const TfLiteConvParams opdata5 = { kTfLitePaddingSame, 1,1, kTfLiteActRelu, 1,1 };
const TfArray<3, int> inputs5 = { 3, { 16,8,9 } };
const TfArray<1, int> outputs5 = { 1, { 17 } };
const TfLiteReshapeParams opdata6 = { { 0, 0, 0, 0, 0, 0, 0, 0, }, 0 };
const TfArray<2, int> inputs6 = { 2, { 17,4 } };
const TfArray<1, int> outputs6 = { 1, { 18 } };
const TfLitePoolParams opdata7 = { kTfLitePaddingSame, 1,2, 1,2, kTfLiteActNone, { { 0,0, 0, 0 } } };
const TfArray<1, int> inputs7 = { 1, { 18 } };
const TfArray<1, int> outputs7 = { 1, { 19 } };
const TfLiteReshapeParams opdata8 = { { 0, 0, 0, 0, 0, 0, 0, 0, }, 0 };
const TfArray<2, int> inputs8 = { 2, { 19,5 } };
const TfArray<1, int> outputs8 = { 1, { 20 } };
const TfLiteFullyConnectedParams opdata9 = { kTfLiteActNone, kTfLiteFullyConnectedWeightsFormatDefault, false, false };
const TfArray<3, int> inputs9 = { 3, { 20,10,11 } };
const TfArray<1, int> outputs9 = { 1, { 21 } };
const TfLiteSoftmaxParams opdata10 = { 1 };
const TfArray<1, int> inputs10 = { 1, { 21 } };
const TfArray<1, int> outputs10 = { 1, { 22 } };
const TensorInfo_t tensorData[] = {
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 3968, (TfLiteIntArray*)&tensor_dimension0, 3960, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant0))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data1, (TfLiteIntArray*)&tensor_dimension1, 16, {kTfLiteNoQuantization, nullptr}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data2, (TfLiteIntArray*)&tensor_dimension2, 16, {kTfLiteNoQuantization, nullptr}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data3, (TfLiteIntArray*)&tensor_dimension3, 16, {kTfLiteNoQuantization, nullptr}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data4, (TfLiteIntArray*)&tensor_dimension4, 16, {kTfLiteNoQuantization, nullptr}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data5, (TfLiteIntArray*)&tensor_dimension5, 8, {kTfLiteNoQuantization, nullptr}, },
  { kTfLiteMmapRo, kTfLiteInt8, (void*)tensor_data6, (TfLiteIntArray*)&tensor_dimension6, 960, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant6))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data7, (TfLiteIntArray*)&tensor_dimension7, 32, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant7))}, },
  { kTfLiteMmapRo, kTfLiteInt8, (void*)tensor_data8, (TfLiteIntArray*)&tensor_dimension8, 384, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant8))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data9, (TfLiteIntArray*)&tensor_dimension9, 64, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant9))}, },
  { kTfLiteMmapRo, kTfLiteInt8, (void*)tensor_data10, (TfLiteIntArray*)&tensor_dimension10, 800, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant10))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data11, (TfLiteIntArray*)&tensor_dimension11, 8, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant11))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension12, 3960, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant12))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 3968, (TfLiteIntArray*)&tensor_dimension13, 792, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant13))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension14, 792, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant14))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 800, (TfLiteIntArray*)&tensor_dimension15, 400, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant15))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension16, 400, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant16))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 800, (TfLiteIntArray*)&tensor_dimension17, 800, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant17))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension18, 800, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant18))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 800, (TfLiteIntArray*)&tensor_dimension19, 400, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant19))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension20, 400, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant20))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 400, (TfLiteIntArray*)&tensor_dimension21, 2, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant21))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension22, 2, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant22))}, },
};const NodeInfo_t nodeData[] = {
  { (TfLiteIntArray*)&inputs0, (TfLiteIntArray*)&outputs0, const_cast<void*>(static_cast<const void*>(&opdata0)), OP_RESHAPE, },
  { (TfLiteIntArray*)&inputs1, (TfLiteIntArray*)&outputs1, const_cast<void*>(static_cast<const void*>(&opdata1)), OP_CONV_2D, },
  { (TfLiteIntArray*)&inputs2, (TfLiteIntArray*)&outputs2, const_cast<void*>(static_cast<const void*>(&opdata2)), OP_RESHAPE, },
  { (TfLiteIntArray*)&inputs3, (TfLiteIntArray*)&outputs3, const_cast<void*>(static_cast<const void*>(&opdata3)), OP_MAX_POOL_2D, },
  { (TfLiteIntArray*)&inputs4, (TfLiteIntArray*)&outputs4, const_cast<void*>(static_cast<const void*>(&opdata4)), OP_RESHAPE, },
  { (TfLiteIntArray*)&inputs5, (TfLiteIntArray*)&outputs5, const_cast<void*>(static_cast<const void*>(&opdata5)), OP_CONV_2D, },
  { (TfLiteIntArray*)&inputs6, (TfLiteIntArray*)&outputs6, const_cast<void*>(static_cast<const void*>(&opdata6)), OP_RESHAPE, },
  { (TfLiteIntArray*)&inputs7, (TfLiteIntArray*)&outputs7, const_cast<void*>(static_cast<const void*>(&opdata7)), OP_MAX_POOL_2D, },
  { (TfLiteIntArray*)&inputs8, (TfLiteIntArray*)&outputs8, const_cast<void*>(static_cast<const void*>(&opdata8)), OP_RESHAPE, },
  { (TfLiteIntArray*)&inputs9, (TfLiteIntArray*)&outputs9, const_cast<void*>(static_cast<const void*>(&opdata9)), OP_FULLY_CONNECTED, },
  { (TfLiteIntArray*)&inputs10, (TfLiteIntArray*)&outputs10, const_cast<void*>(static_cast<const void*>(&opdata10)), OP_SOFTMAX, },
};
static void* overflow_buffers[EI_MAX_OVERFLOW_BUFFER_COUNT];
static size_t overflow_buffers_ix = 0;
static void * AllocatePersistentBuffer(struct TfLiteContext* ctx,
                                       size_t bytes) {
  void *ptr;
  if (current_location - bytes < tensor_boundary) {
    if (overflow_buffers_ix > EI_MAX_OVERFLOW_BUFFER_COUNT - 1) {
      ei_printf("ERR: Failed to allocate persistent buffer of size %d, does not fit in tensor arena and reached EI_MAX_OVERFLOW_BUFFER_COUNT\n",
        (int)bytes);
      return NULL;
    }

    // OK, this will look super weird, but.... we have CMSIS-NN buffers which
    // we cannot calculate beforehand easily.
    ptr = ei_calloc(bytes, 1);
    if (ptr == NULL) {
      ei_printf("ERR: Failed to allocate persistent buffer of size %d\n", (int)bytes);
      return NULL;
    }
    overflow_buffers[overflow_buffers_ix++] = ptr;
    return ptr;
  }

  current_location -= bytes;

  ptr = current_location;
  memset(ptr, 0, bytes);

  return ptr;
}
typedef struct {
  size_t bytes;
  void *ptr;
} scratch_buffer_t;
static scratch_buffer_t scratch_buffers[EI_MAX_SCRATCH_BUFFER_COUNT];
static size_t scratch_buffers_ix = 0;

static TfLiteStatus RequestScratchBufferInArena(struct TfLiteContext* ctx, size_t bytes,
                                                int* buffer_idx) {
  if (scratch_buffers_ix > EI_MAX_SCRATCH_BUFFER_COUNT - 1) {
    ei_printf("ERR: Failed to allocate scratch buffer of size %d, reached EI_MAX_SCRATCH_BUFFER_COUNT\n",
      (int)bytes);
    return kTfLiteError;
  }

  scratch_buffer_t b;
  b.bytes = bytes;

  b.ptr = AllocatePersistentBuffer(ctx, b.bytes);
  if (!b.ptr) {
    ei_printf("ERR: Failed to allocate scratch buffer of size %d\n",
      (int)bytes);
    return kTfLiteError;
  }

  scratch_buffers[scratch_buffers_ix] = b;
  *buffer_idx = scratch_buffers_ix;

  scratch_buffers_ix++;

  return kTfLiteOk;
}

static void* GetScratchBuffer(struct TfLiteContext* ctx, int buffer_idx) {
  if (buffer_idx > (int)scratch_buffers_ix) {
    return NULL;
  }
  return scratch_buffers[buffer_idx].ptr;
}

static TfLiteTensor* GetTensor(const struct TfLiteContext* context,
                               int tensor_idx) {
  return &tflTensors[tensor_idx];
}

static TfLiteEvalTensor* GetEvalTensor(const struct TfLiteContext* context,
                                       int tensor_idx) {
  return &tflEvalTensors[tensor_idx];
}

} // namespace

TfLiteStatus trained_model_init( void*(*alloc_fnc)(size_t,size_t) ) {
#ifdef EI_CLASSIFIER_ALLOCATION_HEAP
  tensor_arena = (uint8_t*) alloc_fnc(16, kTensorArenaSize);
  if (!tensor_arena) {
    ei_printf("ERR: failed to allocate tensor arena\n");
    return kTfLiteError;
  }
#else
  memset(tensor_arena, 0, kTensorArenaSize);
#endif
  tensor_boundary = tensor_arena;
  current_location = tensor_arena + kTensorArenaSize;
  ctx.AllocatePersistentBuffer = &AllocatePersistentBuffer;
  ctx.RequestScratchBufferInArena = &RequestScratchBufferInArena;
  ctx.GetScratchBuffer = &GetScratchBuffer;
  ctx.GetTensor = &GetTensor;
  ctx.GetEvalTensor = &GetEvalTensor;
  ctx.tensors = tflTensors;
  ctx.tensors_size = 23;
  for (size_t i = 0; i < 23; ++i) {
    tflTensors[i].type = tensorData[i].type;
    tflEvalTensors[i].type = tensorData[i].type;
    tflTensors[i].is_variable = 0;

#if defined(EI_CLASSIFIER_ALLOCATION_HEAP)
    tflTensors[i].allocation_type = tensorData[i].allocation_type;
#else
    tflTensors[i].allocation_type = (tensor_arena <= tensorData[i].data && tensorData[i].data < tensor_arena + kTensorArenaSize) ? kTfLiteArenaRw : kTfLiteMmapRo;
#endif
    tflTensors[i].bytes = tensorData[i].bytes;
    tflTensors[i].dims = tensorData[i].dims;
    tflEvalTensors[i].dims = tensorData[i].dims;

#if defined(EI_CLASSIFIER_ALLOCATION_HEAP)
    if(tflTensors[i].allocation_type == kTfLiteArenaRw){
      uint8_t* start = (uint8_t*) ((uintptr_t)tensorData[i].data + (uintptr_t) tensor_arena);

     tflTensors[i].data.data =  start;
     tflEvalTensors[i].data.data =  start;
    }
    else {
       tflTensors[i].data.data = tensorData[i].data;
       tflEvalTensors[i].data.data = tensorData[i].data;
    }
#else
    tflTensors[i].data.data = tensorData[i].data;
    tflEvalTensors[i].data.data = tensorData[i].data;
#endif // EI_CLASSIFIER_ALLOCATION_HEAP
    tflTensors[i].quantization = tensorData[i].quantization;
    if (tflTensors[i].quantization.type == kTfLiteAffineQuantization) {
      TfLiteAffineQuantization const* quant = ((TfLiteAffineQuantization const*)(tensorData[i].quantization.params));
      tflTensors[i].params.scale = quant->scale->data[0];
      tflTensors[i].params.zero_point = quant->zero_point->data[0];
    }
    if (tflTensors[i].allocation_type == kTfLiteArenaRw) {
      auto data_end_ptr = (uint8_t*)tflTensors[i].data.data + tensorData[i].bytes;
      if (data_end_ptr > tensor_boundary) {
        tensor_boundary = data_end_ptr;
      }
    }
  }
  if (tensor_boundary > current_location /* end of arena size */) {
    ei_printf("ERR: tensor arena is too small, does not fit model - even without scratch buffers\n");
    return kTfLiteError;
  }
  registrations[OP_RESHAPE] = Register_RESHAPE();
  registrations[OP_CONV_2D] = Register_CONV_2D();
  registrations[OP_MAX_POOL_2D] = Register_MAX_POOL_2D();
  registrations[OP_FULLY_CONNECTED] = Register_FULLY_CONNECTED();
  registrations[OP_SOFTMAX] = Register_SOFTMAX();

  for (size_t i = 0; i < 11; ++i) {
    tflNodes[i].inputs = nodeData[i].inputs;
    tflNodes[i].outputs = nodeData[i].outputs;
    tflNodes[i].builtin_data = nodeData[i].builtin_data;
tflNodes[i].custom_initial_data = nullptr;
      tflNodes[i].custom_initial_data_size = 0;
if (registrations[nodeData[i].used_op_index].init) {
      tflNodes[i].user_data = registrations[nodeData[i].used_op_index].init(&ctx, (const char*)tflNodes[i].builtin_data, 0);
    }
  }
  for (size_t i = 0; i < 11; ++i) {
    if (registrations[nodeData[i].used_op_index].prepare) {
      TfLiteStatus status = registrations[nodeData[i].used_op_index].prepare(&ctx, &tflNodes[i]);
      if (status != kTfLiteOk) {
        return status;
      }
    }
  }
  return kTfLiteOk;
}

static const int inTensorIndices[] = {
  0, 
};
TfLiteTensor* trained_model_input(int index) {
  return &ctx.tensors[inTensorIndices[index]];
}

static const int outTensorIndices[] = {
  22, 
};
TfLiteTensor* trained_model_output(int index) {
  return &ctx.tensors[outTensorIndices[index]];
}

TfLiteStatus trained_model_invoke() {
  for (size_t i = 0; i < 11; ++i) {
    TfLiteStatus status = registrations[nodeData[i].used_op_index].invoke(&ctx, &tflNodes[i]);

#if EI_CLASSIFIER_PRINT_STATE
    ei_printf("layer %lu\n", i);
    ei_printf("    inputs:\n");
    for (size_t ix = 0; ix < tflNodes[i].inputs->size; ix++) {
      auto d = tensorData[tflNodes[i].inputs->data[ix]];

      size_t data_ptr = (size_t)d.data;

      if (d.allocation_type == kTfLiteArenaRw) {
        data_ptr = (size_t)tensor_arena + data_ptr;
      }

      if (d.type == TfLiteType::kTfLiteInt8) {
        int8_t* data = (int8_t*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes; jx++) {
          ei_printf("%d ", data[jx]);
        }
      }
      else {
        float* data = (float*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes / 4; jx++) {
          ei_printf("%f ", data[jx]);
        }
      }
      ei_printf("\n");
    }
    ei_printf("\n");

    ei_printf("    outputs:\n");
    for (size_t ix = 0; ix < tflNodes[i].outputs->size; ix++) {
      auto d = tensorData[tflNodes[i].outputs->data[ix]];

      size_t data_ptr = (size_t)d.data;

      if (d.allocation_type == kTfLiteArenaRw) {
        data_ptr = (size_t)tensor_arena + data_ptr;
      }

      if (d.type == TfLiteType::kTfLiteInt8) {
        int8_t* data = (int8_t*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes; jx++) {
          ei_printf("%d ", data[jx]);
        }
      }
      else {
        float* data = (float*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes / 4; jx++) {
          ei_printf("%f ", data[jx]);
        }
      }
      ei_printf("\n");
    }
    ei_printf("\n");
#endif // EI_CLASSIFIER_PRINT_STATE

    if (status != kTfLiteOk) {
      return status;
    }
  }
  return kTfLiteOk;
}

TfLiteStatus trained_model_reset( void (*free_fnc)(void* ptr) ) {
#ifdef EI_CLASSIFIER_ALLOCATION_HEAP
  free_fnc(tensor_arena);
#endif

  // scratch buffers are allocated within the arena, so just reset the counter so memory can be reused
  scratch_buffers_ix = 0;

  // overflow buffers are on the heap, so free them first
  for (size_t ix = 0; ix < overflow_buffers_ix; ix++) {
    ei_free(overflow_buffers[ix]);
  }
  overflow_buffers_ix = 0;
  return kTfLiteOk;
}
